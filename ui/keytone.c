//File   : lplay.c
//Author : Loon <sepnic@gmail.com>
 
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#include <locale.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <alsa/asoundlib.h>
#include <assert.h>
#include "wav_parser.h"
#include "sndwav_common.h"

static SNDPCMContainer_t playback;

ssize_t SNDWAV_Read(int fd, SNDPCMContainer_t *sndpcm, WAVContainer_t *container)
{
	ssize_t ret;
 
	ret = read(fd, sndpcm->data_buf, container->chunk.length );

	return ret;
}

void keytone_play(void)
{
	int load,ret;
	off64_t written = 0;
	off64_t c;
	off64_t count = playback.wav_data_size;
	
	//printf("===playback.wav_data_size=%d playback.chunk_bytes=%d\n",playback.wav_data_size,playback.chunk_bytes);
 
	load = 0;
	if (playback.handle) {
		while (written < count) {
			/* Must read [chunk_bytes] bytes data enough. */
			do {
				c = count - written;
				if (c > playback.chunk_bytes)
					c = playback.chunk_bytes;
				c -= load;
	 
				if (c == 0)
					break;
				load += c;
			} while ((size_t)load < playback.chunk_bytes);
	 
			/* Transfer to size frame */
			load = load * 8 / playback.bits_per_frame;
			ret = SNDWAV_WritePcm(&playback, load, written);
			//printf("==ret==%d\n",ret);
			if (ret != load)
				break;
			
			ret = ret * playback.bits_per_frame / 8;
			written += ret;
			load = 0;
		}
		/* next play ready*/
		snd_pcm_prepare(playback.handle);
	}
}

int keytone_init(char *filename)
{
	int fd;
	char devicename[] = "default";
	WAVContainer_t wav;
	
    //if(get_hdmi_status())
		//sprintf(devicename, "hdmi");
	
	memset(&playback, 0x0, sizeof(playback));

	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Error open [%s]\n", filename);
		goto Err;
	}
	
	if (WAV_ReadHeader(fd, &wav) < 0) {
		fprintf(stderr, "Error WAV_Parse [%s]\n", filename);
		goto Err;
	}
 
	if (snd_output_stdio_attach(&playback.log, stderr, 0) < 0) {
		fprintf(stderr, "Error snd_output_stdio_attach\n");
		goto Err;
	}
 
	if (snd_pcm_open(&playback.handle, devicename, SND_PCM_STREAM_PLAYBACK, 0) < 0) {
		fprintf(stderr, "Error snd_pcm_open [ %s]\n", devicename);
		goto Err;
	}
 
	if (SNDWAV_SetParams(&playback, &wav) < 0) {
		fprintf(stderr, "Error set_snd_pcm_params\n");
		goto Err;
	}
	//snd_pcm_dump(playback.handle, playback.log);
	
	SNDWAV_Read(fd, &playback, &wav);
	
	close(fd);
 
	//snd_pcm_drain(playback.handle);
	return 0;
 
Err:
	close(fd);
	if (playback.data_buf) free(playback.data_buf);
	//if (playback.log) snd_output_close(playback.log);
	if (playback.handle) snd_pcm_close(playback.handle);
	memset(&playback, 0x0, sizeof(playback));
	
	return -1;
}

int keytone_exit(void)
{
	//printf("keytone_exit\n");
	if (playback.data_buf) free(playback.data_buf);
	if (playback.handle) snd_pcm_close(playback.handle);
	memset(&playback, 0x0, sizeof(playback));
	
	return 0;
}