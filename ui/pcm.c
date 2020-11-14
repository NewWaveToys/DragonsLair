#include <alsa/asoundlib.h>

unsigned char *sound_buf=NULL;



static void alsa_thread_override_device(char *alsa_dev, int size)
{
   const char *path;
   char buf[256] = "\0";
   int fd;

#define ALSA_DEVICE_CFG_PATH_ENV	"ALSA_DEVICE_CFG"
#define ALSA_DEVICE_CFG_PATH_DEFAULT	"/tmp/alsa_device.cfg"
   path = getenv(ALSA_DEVICE_CFG_PATH_ENV);
   if (!path)
      path = ALSA_DEVICE_CFG_PATH_DEFAULT;

   fd = open(path, O_RDONLY);
   read(fd, buf, sizeof(buf));
   close(fd);

   if (!buf[0])
      return;

   if (buf[strlen(buf) - 1] == '\n')
      buf[strlen(buf) - 1] = '\0';

   strncpy(alsa_dev, buf, size);
}


//alsa ==

static snd_pcm_t *pcm = NULL;
static struct {
	snd_pcm_format_t format;
	unsigned int channels;
	unsigned int rate;
} hwparams, rhwparams;



//static int mmap_flag = 0;
static int interleaved = 1;
static snd_pcm_uframes_t chunk_size = 1024;
static unsigned period_time = 0;
static unsigned buffer_time = 0;
static snd_pcm_uframes_t period_frames = 940;
static snd_pcm_uframes_t buffer_frames = 3760;//16384>>1;
static int monotonic = 0;

static int can_pause = 0;

static int avail_min = -1;
static char allow = 1;


static size_t  bits_per_sample, bits_per_frame;

static void set_params(void)
{
	snd_pcm_hw_params_t *params;
	snd_pcm_sw_params_t *swparams;
	snd_pcm_uframes_t buffer_size;
	int err;
	size_t n;
	unsigned int rate;
	snd_pcm_uframes_t start_threshold, stop_threshold;
	snd_pcm_hw_params_alloca(&params);
	//
	err = snd_pcm_hw_params_any(pcm, params);
	if (err < 0) {
		printf("Broken configuration for this PCM: no configurations available\n");
	}
	
	/*if (mmap_flag) {
		snd_pcm_access_mask_t *mask = alloca(snd_pcm_access_mask_sizeof());
		snd_pcm_access_mask_none(mask);
		snd_pcm_access_mask_set(mask, SND_PCM_ACCESS_MMAP_INTERLEAVED);
		snd_pcm_access_mask_set(mask, SND_PCM_ACCESS_MMAP_NONINTERLEAVED);
		snd_pcm_access_mask_set(mask, SND_PCM_ACCESS_MMAP_COMPLEX);
		err = snd_pcm_hw_params_set_access_mask(pcm, params, mask);
	} else */
	if (interleaved)
		err = snd_pcm_hw_params_set_access(pcm, params,
						   SND_PCM_ACCESS_RW_INTERLEAVED);
	else
		err = snd_pcm_hw_params_set_access(pcm, params,
						   SND_PCM_ACCESS_RW_NONINTERLEAVED);
	if (err < 0) {
		printf("Access type not available\n");
	}
	err = snd_pcm_hw_params_set_format(pcm, params, hwparams.format);
	if (err < 0) {
		printf("Sample format non available\n");
		//show_available_sample_formats(params);

	}
	err = snd_pcm_hw_params_set_channels(pcm, params, hwparams.channels);
	if (err < 0) {
		printf("Channels count non available\n");

	}

#if 0
	err = snd_pcm_hw_params_set_periods_min(pcm, params, 2);
	assert(err >= 0);
#endif
	rate = hwparams.rate;
int dir = 1;
	err = snd_pcm_hw_params_set_rate_near(pcm, params, &rate, 0);

	hwparams.rate = rate;
	#if 1
	if (buffer_time == 0 && buffer_frames == 0) {
		err = snd_pcm_hw_params_get_buffer_time_max(params,
							    &buffer_time, 0);
		assert(err >= 0);
		if (buffer_time > 500000)
			buffer_time = 500000;
	}
	if (period_time == 0 && period_frames == 0) {
		if (buffer_time > 0)
			period_time = buffer_time / 4;
		else
			period_frames = buffer_frames / 4;
	}
	if (period_time > 0)
		err = snd_pcm_hw_params_set_period_time_near(pcm, params,
							     &period_time, 0);
	else
	err = snd_pcm_hw_params_set_period_size_near(pcm, params,
							     &period_frames, 0);
	assert(err >= 0);
	if (buffer_time > 0) {
		err = snd_pcm_hw_params_set_buffer_time_near(pcm, params,
							     &buffer_time, 0);
	} else {
		err = snd_pcm_hw_params_set_buffer_size_near(pcm, params,
							     &buffer_frames);
	}
	assert(err >= 0);
	monotonic = snd_pcm_hw_params_is_monotonic(params);
	can_pause = snd_pcm_hw_params_can_pause(params);
	#endif
	//snd_pcm_hw_params_set_period_size(pcm,params,500,1);
	//snd_pcm_hw_params_set_buffer_size(pcm,params,200);
	
	err = snd_pcm_hw_params(pcm, params);
	if (err < 0) {
		printf("Unable to install hw params:\n");
		//snd_pcm_hw_params_dump(params, log);
		
	}
	snd_pcm_hw_params_get_period_size(params, &chunk_size, 0);
	snd_pcm_hw_params_get_buffer_size(params, &buffer_size);
	if (chunk_size == buffer_size) {
		printf("Can't use period equal to buffer size (%lu == %lu)\n",
		      chunk_size, buffer_size);
		
	}
	snd_pcm_sw_params_alloca(&swparams);	
	
	snd_pcm_sw_params_current(pcm, swparams);
	if (avail_min < 0)
		n = chunk_size;
	else
		n = (double) rate * avail_min / 1000000;
		//n =4096;
	//err = snd_pcm_sw_params_set_avail_min(pcm, swparams, n);
	//	printf("snd_pcm_sw_params_set_avail_min=%d==\n",n);
	//snd_pcm_sw_params_set_start_threshold(pcm, swparams, 1);

	if (snd_pcm_sw_params(pcm, swparams) < 0) {
		printf("unable to install sw params:\n");
		//snd_pcm_sw_params_dump(swparams, log);
		
	}

	//if (setup_chmap())
		//prg_exit(EXIT_FAILURE);


	bits_per_sample = snd_pcm_format_physical_width(hwparams.format);
	snd_pcm_format_width(hwparams.format);
	bits_per_frame = bits_per_sample * hwparams.channels;
	 
	// fprintf(stderr, "real chunk_size = %i, frags = %i, total = %i\n", chunk_size, setup.buf.block.frags, setup.buf.block.frags * chunk_size);

	printf("bits_per_frame %d \n",bits_per_frame);

	//snd_pcm_nonblock(pcm,0);


	//buffer_frames = buffer_size;	/* for position test */
}
static ssize_t pcm_write(u_char *data, size_t count)
{
	ssize_t r;
	ssize_t result = 0;

	if (count < chunk_size) {
		snd_pcm_format_set_silence(hwparams.format, data + count * bits_per_frame / 8, (chunk_size - count) * hwparams.channels);
		count = chunk_size;
	}
	//data = remap_data(data, count);
	while (count > 0 &&allow) {
		
		r = //writei_func
			snd_pcm_writei(pcm, data, count);
		
	//	if (test_position)
		//	do_test_position();
		#if 0
		if (r == -EAGAIN || (r >= 0 && (size_t)r < count)) {
			//if (!test_nowait)
			//	snd_pcm_wait(pcm, 100);
		} else if (r == -EPIPE) {
			xrun();
		} else if (r == -ESTRPIPE) {
			//suspend();
		} else if (r < 0) {
			printf("write error: \n");
			//prg_exit(EXIT_FAILURE);
			
		}
			#else
			 if (r == -EPIPE)printf("=========error============\n");
			if ( r < 0 ) {	
				if ( r == -EAGAIN ) {
					/* Apparently snd_pcm_recover() doesn't handle this case - does it assume snd_pcm_wait() above? */	
					//usleep(100);	
					snd_pcm_wait(pcm, 5);
					
					//snd_pcm_avail_update(pcm);
					continue;	
					
					}			
				r = snd_pcm_recover(pcm, r, 0);		
				if ( r < 0 ) {
					/* Hmm, not much we can do - abort */		
					//fprintf(stderr, "ALSA write failed (unrecoverable): %s\n", SDL_NAME(snd_strerror)(status));	
					//this->enabled = 0;	
					printf("write pcm error %d \n", r);
					return r;			
					}			
				continue;		
			}
			
			#endif
		if (r > 0) {
		
			result += r;
			count -= r;
			data += r * bits_per_frame / 8;
		}
	}
	return result;
}
// void ALSA_PlayAudio(_THIS)
//{
	//int status;
	//const Uint8 *sample_buf = (const Uint8 *) mixbuf;
	//const int frame_size = (((int) (0x8010 & 0xFF)) / 8) * 2;
	//printf("%s %d frame_size %d \n",__FUNCTION__,__LINE__,this->spec.samples);
//if(allow&&mixbuf)
	//pcm_write(mixbuf,this->spec.samples);
//}

unsigned char *get_soundbuf(){return sound_buf;}
void playaudio(int len)
{
	pcm_write(sound_buf,len>>2);
}


int open_alsa()
{
	char g_alsa_dev[256];
	int ret;
	int recount = 0;

	 strncpy(g_alsa_dev,  "default", sizeof(g_alsa_dev));
  	 alsa_thread_override_device(g_alsa_dev, sizeof(g_alsa_dev));
	if(is_hdmi())
	{
	 //	strncpy(g_alsa_dev,  "hdmi", sizeof(g_alsa_dev));
			sleep(10);
	}
	retry:
	 ret = snd_pcm_open(&pcm, g_alsa_dev,SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);
	printf("%s ret %d  %s \n",__FUNCTION__,ret, g_alsa_dev);
	if(ret < 0)
	{
		recount++;
		if(recount < 3)
		{
			sleep(10);
			goto retry;
		}
		return -1;
	}
	rhwparams.format = SND_PCM_FORMAT_S16_LE;
	rhwparams.rate = 44100;
///	if(is_hdmi())
//	rhwparams.rate = 48000;
	rhwparams.channels = 2;
	hwparams = rhwparams;
	//chunk_size = 1024;
	set_params();
	snd_pcm_start(pcm);
	
	
	allow = 1;
	return ret;
}
void init_buf()
{
	if(!sound_buf)
	{	sound_buf = (unsigned char*)malloc((0x8010&0xFF)/8*2*2048+44100);
	memset(sound_buf,0,(0x8010&0xFF)/8*2*2048+44100);
	
	}

}
void deinit_buf()
{
	if(sound_buf)free(sound_buf);
			sound_buf =NULL;

}
void reset_alsa()
{
	allow= 0;
	usleep(3000);
	if(pcm)
	{snd_pcm_drop(pcm);	snd_pcm_close(pcm);}
		pcm = NULL;

	open_alsa();
}
void play_silence()
{
	char *m=(char*)malloc(8192);
	if(m){memset(m,0,8192);
	pcm_write(m,8192* 8 / bits_per_frame);
	pcm_write(m,8192* 8 / bits_per_frame);
	free(m);
		}

}
void close_alsa()
{
	allow = 0;

	usleep(3000);
	
	if(pcm){snd_pcm_drop(pcm);	snd_pcm_close(pcm);}
	pcm = NULL;
	
	

}

