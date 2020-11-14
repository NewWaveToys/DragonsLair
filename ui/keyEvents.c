
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#include "rthreads/rthreads.h"
#include <linux/input.h>
#include <linux/netlink.h>
#include <sys/socket.h>
#include <sys/prctl.h>


#define CHAR_BIT 8


#define test_bit(nr, addr) \
   (((1UL << ((nr) % (sizeof(long) * CHAR_BIT))) & ((addr)[(nr) / (sizeof(long) * CHAR_BIT)])) != 0)
#define NBITS(x) ((((x) - 1) / (sizeof(long) * CHAR_BIT)) + 1)


#define NUM_AXES 32

#define MAX_INPUTNUM 6
static int kbd_fd[MAX_INPUTNUM];
static int kbdevent_id[MAX_INPUTNUM];
static int need_reinit_input = 0;
int keyevent_isrun = 0;
struct timeval event_timeout;
struct abs_pad
{
	uint8_t axes_bind[ABS_MAX];
	struct input_absinfo absinfo[NUM_AXES];
	int fd;
};
static struct abs_pad g_abspad[MAX_INPUTNUM];
static int keyboard_open(int ch, const char *device)
{
	int fd;
	fd = open(device, O_RDONLY | O_NOCTTY);

    if (fd < 0) {
        return -1;
    }
	
    return fd;
}
static void setasbinfo(int fd, struct abs_pad *pad)
{
	int i;
	int axes=0;
	unsigned long absbit[NBITS(ABS_MAX)] = {0};
	 
	 if ( (ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(absbit)), absbit) < 0))
		   return ;
	for (i = 0; i < ABS_MISC&& axes < 32 ; i++)
		if (test_bit(i, absbit))
      {
         struct input_absinfo *abs = &pad->absinfo[axes];

         if (ioctl(fd, EVIOCGABS(i), abs) < 0)
            continue;
		 if (abs->maximum > abs->minimum)
         {
           
			pad->axes_bind[i] = axes++;
         }
		}
		
}
static void keyboard_reinit(void)
{
    int i = 0;
    char keyboard_device[30];

    for (i = 0; i < MAX_INPUTNUM; i++) {
		close(kbd_fd[i]);
        kbd_fd[i] = -1;
    }
    for (i = 0; i < MAX_INPUTNUM; i++) {
        sprintf(keyboard_device, "/dev/input/event%d", i);
        kbd_fd[i] = keyboard_open(i, keyboard_device);
		setasbinfo(kbd_fd[i],&g_abspad[i]);
		//printf("%s keyboard_device = %s, fd = %d\n", __func__, keyboard_device, kbd_fd[i]);
    }
}

static void keyboard_init(void)
{
	int i = 0;
	char keyboard_device[30];
	for (i = 0; i < MAX_INPUTNUM; i++) {
		sprintf(keyboard_device, "/dev/input/event%d", i);
		kbd_fd[i] = keyboard_open(i, keyboard_device);
		setasbinfo(kbd_fd[i],&g_abspad[i]);
		//printf("%s keyboard_device = %s, fd = %d\n", __func__, keyboard_device, kbd_fd[i]);
	}
}

static int keyboard_wait_event(int which, int maxfd, fd_set *in, fd_set *out, fd_set *except, struct timeval *timeout)
{
    fd_set rfds;
    int	fd,ret;
	int i;

    if (!in) {
        in = &rfds;
        FD_ZERO(in);
    }

	for (i = 0; i < MAX_INPUTNUM; i++) {
		if (kbd_fd[i] >= 0) {
			fd = kbd_fd[i];
			FD_SET(fd, in);
			if (fd > maxfd)
				maxfd = fd;
		}
	}
	
	ret = select(maxfd + 1, in, out, except, timeout) ;
	if (ret > 0) {		
		for (i = 0; i < MAX_INPUTNUM; i++) {
			kbdevent_id[i] = 0;
			fd = kbd_fd[i];
			if (fd >= 0 && FD_ISSET(fd, in)) {
				FD_CLR (fd, in);
				kbdevent_id[i] = 1;
			}
			//printf("kbdevent_id[%d] = %d\n",i,kbdevent_id[i]);
		}
	} else {
		return -1;
	}

	return 0;

}


static int event_reads(int ch, unsigned int *buf, int *modifiers)
{
    int    cc;            /* characters read */
    struct input_event buftmp;

    *modifiers = 0;      /* no modifiers yet */

    *buf = 0;

    cc = read(kbd_fd[ch], &buftmp, sizeof(buftmp));
    if (cc < 0) {
        return -1;
    }

    buf[0] = buftmp.code;
    buf[1] = buftmp.value;
    buf[2] = buftmp.type;
   	//printf("keyboard: value=%d,code=%d,*buf=0x%08x type %d\n",buftmp.value, buftmp.code, *buf, buftmp.type);
    return 0;
}
static  short udev_compute_axis(const struct input_absinfo *info, int value)
{
   int range = info->maximum - info->minimum;
   int axis  = (value - info->minimum) * 0xffffll / range - 0x7fffll;

   if (axis > 0x7fff)
      return 0x7fff;
   else if (axis < -0x7fff)
      return -0x7fff;
   return axis;
}
extern int onkey(unsigned short code, unsigned int value);
int up_down=0;
int left_right=0;
static void parse(uint16_t code, int value, uint16_t type,struct abs_pad *pad )
{
	if(EV_ABS==type)
	{
		
		 if (code < ABS_MISC)
		 {
              switch (code)
              {
                 case ABS_HAT0X:
                 case ABS_HAT0Y:
                 case ABS_HAT1X:
                 case ABS_HAT1Y:
                 case ABS_HAT2X:
                 case ABS_HAT2Y:
                 case ABS_HAT3X:
                 case ABS_HAT3Y:
                    code                           -= ABS_HAT0X;
                    printf("%s %d code %d \n", __FUNCTION__,value,code);
                    break;
                 default:
                    {
						
                       unsigned axis   = pad->axes_bind[code];
					   short rec = udev_compute_axis(&pad->absinfo[axis], value);
					   if(axis == 1)
					   	{
					   	//y
					   		if(-128 == rec)
					   		{
					   			if(up_down == 1)
					   			{
					   				type = EV_KEY;
									code = 103;
									value =0;
									up_down = 0;
					   			}
								else if(up_down == -1)
								{
									type = EV_KEY;
									code = 108;
									value =0;
									up_down = 0;
								}
					   		}

							if(0x7fff>= rec&&rec>300)
							{
								if(up_down == 0)
								{
									type = EV_KEY;
									code = 108;
									value =1;
									up_down = -1;
								}
							}							
							else if(-200 > rec&&-32767<=rec)
							{
								if(up_down == 0)
								{
									type = EV_KEY;
									code = 103;
									value =1;
									up_down = 1;
								}
							}
					   	}
					    if(axis == 0)
					   	{
					   	//x
					   		if(-128 == rec)
					   		{
					   			if(left_right == 1)
					   			{
					   				type = EV_KEY;
									code = 106;
									value =0;
									left_right = 0;
					   			}
								else if(left_right == -1)
								{
									type = EV_KEY;
									code = 105;
									value =0;
									left_right = 0;
								}
					   		}

							if(32767>= rec&&rec>300)
							{
								if(left_right == 0)
								{
									type = EV_KEY;
									code = 106;
									value =1;
									left_right = 1;
								}
							}							
							else if(-200 > rec&&-32767<=rec)
							{
								if(left_right == 0)
								{
									type = EV_KEY;
									code = 105;
									value =1;
									left_right = -1;
								}
							}
					   	}
                     // pad->axes[axis] = 
                   //  printf("%s %d code %d =%x axis %d %d \n", __FUNCTION__,value,code,rec,axis,rec);
					//  	udev_compute_axis(
                    //         &pad->absinfo[axis], value);
                       break;
                    }
              }
		 	}
	}
	if(EV_KEY ==type)
	{
		onkey(code, value);
		
	}
}

static int keyboard_update(void)
{
    int retvalue = 0;
    int i,ret;
    for (i = 0; i < MAX_INPUTNUM; i++) {
	    unsigned  int buf[3];
		int modifier;
	    if (kbdevent_id[i] == 1){
	    	retvalue = event_reads(i, buf, &modifier);
			//
		    if ((retvalue == -1) || (buf[2] == 0)) {
		        return -1;
		    } else {
		    #if 0
				if(1 == sdCardExist) {
					if(pagemode)
					{
						ret = pagemode->onkey(buf[0], buf[1]);
						if(ret == 1) {
							pagemode_init = 0;
							if(pagemode)
								pagemode->deinit();
							pagemode = &testqa_page;
							pagemode->init(NULL);
							pagemode_init = 1;
						}
						continue;
						//printf("============%u %u============\n",GetTickCount() , rungame_time);
					} else {
						if(sdCardInit == 1)
							parse_sdcard_key(buf[0], buf[1], buf[2]);
					}
				} else {
				#endif
				//printf("%s %d retvalue = %d, buf[0]=0x%x buf[1]=%d buf[2]=%d \n", __func__, i,retvalue, buf[0],buf[1],buf[2]);
		    		parse(buf[0], buf[1], buf[2],&g_abspad[i]);
					//rungame_time = GetTickCount();
				//}
			}
   		}
    }

	return 0;
}
static int sockfd;

static void input_monitor_thread(void *arg)
{
	
	int i, j, len;
	char buf[512];
	struct iovec iov;
	struct msghdr msg;
	struct sockaddr_nl sa;
	sleep(5);
	prctl(PR_SET_NAME, "input_monitor", 0, 0, 0);

	memset(&sa, 0, sizeof(sa));
	sa.nl_family = AF_NETLINK;
	sa.nl_groups = NETLINK_KOBJECT_UEVENT;
	sa.nl_pid = 0;
	memset(&msg, 0, sizeof(msg));
	iov.iov_base = (void *)buf;
	iov.iov_len = sizeof(buf);
	msg.msg_name = (void *)&sa;
	msg.msg_namelen = sizeof(sa);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	sockfd = socket(AF_NETLINK, SOCK_RAW, NETLINK_KOBJECT_UEVENT);
	if (sockfd == -1) {
		printf("socket creating failed\n");
		goto err_event_monitor;
	}

	if (bind(sockfd, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
		printf("bind error\n");
		goto err_event_monitor;
	}

	while (keyevent_isrun) {
		/*if(disablekey) {
			usleep(5000);
			continue;
		}*/
		memset(buf, 0, sizeof(buf));
		len = recvmsg(sockfd, &msg, 0);
		
		if (len < 0) {
			printf("receive errno\n");
		} else if (len < 32 || len > sizeof(buf)) {
			printf("invalid message");
		} else {			
			if (strstr(buf, "event")) {
				if (strstr(buf, "remove")) {
					need_reinit_input = 1;
					//printf("need_reinit_input %d\n",need_reinit_input);
				} else if (strstr(buf, "add")) {
					need_reinit_input = 1;
					//printf("need_reinit_input %d\n",need_reinit_input);
				}
			} else if (strstr(buf, "mmcblk")) {
				if (strstr(buf, "remove")) {
					//sdCardExist = 0;
					//printf("sdCardExist  %d\n",sdCardExist);
				} else if (strstr(buf, "add")) {
					
					//sdCardExist = 1;
					//printf("sdCardExist  %d\n",sdCardExist);
				}
			}
		}
	}

err_event_monitor:
	printf("rec =exit= \n");
	pthread_detach(pthread_self());
	pthread_exit(NULL);
}

int sdcard_check(void)
{
   	int fd;
	fd = fopen("/dev/mmcblk0", "r");
	if (fd != NULL) fclose(fd);
    return fd;
}
/*
int sdcard_file_check(void)
{
	if ((access(TESTMODE_FILE,F_OK)) != -1) {
		//testfile exists
		return 1;
	}
	else {
		//testfile don't exists
		return 0;
	}
}*/

static void EventLoop(void *arg)
{
    int event;
	event_timeout.tv_sec = 3;
	event_timeout.tv_usec = 500;
	sleep(5);
	keyboard_init();

    while (keyevent_isrun) {
		
        if (need_reinit_input) {
            need_reinit_input = 0;
            keyboard_reinit();
        }
        event = keyboard_wait_event(0, 0, NULL, NULL, NULL, &event_timeout);
        if (event < 0)
            continue;
		keyboard_update();
    }
}

sthread_t *minput_monitor_thread;
sthread_t *meventloop;

int init_keyevent()
{
	keyevent_isrun = 1;

	minput_monitor_thread = sthread_create(input_monitor_thread, NULL);
	meventloop = sthread_create(EventLoop, NULL);
return 0;
}
int deinit_loopkeyevent()
{
	keyevent_isrun = 0;

	if(meventloop) sthread_join(meventloop);
	meventloop = NULL;

}
int deinit_keyevent()
{
	//char buf[1];
	keyevent_isrun = 0;
	if(meventloop) sthread_join(meventloop);
	printf("=======0===\n");
	// sendmsg(sockfd, buf, 1, 0);
	printf("=======2===\n");
	//close(sockfd);
	exit(1);
	if(minput_monitor_thread) sthread_join(minput_monitor_thread);
	printf("=======1===\n");
	minput_monitor_thread = NULL;
	meventloop = NULL;
	return 0;
}


