#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include "disp.h"
unsigned int framebuff[480*227];
#if 0
#include "sunxi_display2.h"


int fdDisplay;
int screen_id;
int nScreenWidth,nScreenHeight;

static int SetLayerParam()
{
    disp_layer_config2 config;
    unsigned long     args[4];
    int ret = 0;
    //* close the layer first, otherwise, in case when last frame is kept showing,
    //* the picture showed will not valid because parameters changed.
    memset(&config.info, 0, sizeof(disp_layer_info2));
  
  
            config.info.fb.format = DISP_FORMAT_ARGB_8888;
            config.info.fb.size[0].width     = 200;
            config.info.fb.size[0].height    = 100;
            config.info.fb.size[1].width     = 200/2;
            config.info.fb.size[1].height    = 100/2;
            config.info.fb.size[2].width     = 200/2;
            config.info.fb.size[2].height    = 100/2;
      

    //* initialize the layerInfo.
    config.info.mode            = LAYER_MODE_BUFFER;
    config.info.zorder          = 1;
    config.info.alpha_mode      = 1;
    config.info.alpha_mode      = 0xff;

    //* image size.
    config.info.fb.crop.x = 0;
    config.info.fb.crop.y = 0;
    config.info.fb.crop.width   = (unsigned long long)200 << 32;
    config.info.fb.crop.height  = (unsigned long long)100 << 32;
    config.info.fb.color_space  = DISP_BT601;//(lc->nHeight < 720) ? DISP_BT601 : DISP_BT709;

    //* source window.
   
        config.info.screen_win.x        = 0;
        config.info.screen_win.y        = 0;
        config.info.screen_win.width    = nScreenWidth;
        config.info.screen_win.height   = nScreenHeight;

   
/*

    int i;
    for(i = 0; i < GPU_BUFFER_NUM; i++)
    {
        if(lc->mBufferInfo[i].pMetaDataVirAddr == pPicture->pMetaData)
        {
            config.info.fb.metadata_fd = lc->mBufferInfo[i].nMetaDataMapFd;
            config.info.fb.metadata_size = 4096;
            if (lc->bAfbcModeFlag)
            {
                config.info.fb.metadata_flag = 1<<4;
                config.info.fb.fbd_en = 1;
            }
            if (lc->bHdrVideoFlag)
            {
                config.info.fb.metadata_flag = config.info.fb.metadata_flag|(1<<1);
            }
            break;
        }
    }
*/
    config.info.fb.fd = (int)framebuff;

    config.info.alpha_mode          = 1;
    config.info.alpha_value         = 0xff;

    config.channel = 0;
    config.enable = 1;
    config.layer_id = 0;
    //* set layerInfo to the driver.
    args[0] = (unsigned long)0;
    args[1] = (unsigned long)(&config);
    args[2] = (unsigned long)1;

    ret = ioctl(fdDisplay, DISP_LAYER_SET_CONFIG2, (void*)args);
    if(0 != ret)
        printf("fail to set layer info! \n");

    return 0;
}
#endif

int main(int argc , const char** argv)
{		int i=0;
#if 0
	unsigned int args[4];
	//	disp_layer_info layerInfo;
		disp_output_type output_type;


	fdDisplay = open("/dev/disp", O_RDWR);
	for(screen_id=0;screen_id<3;screen_id++)
		{
			args[0] = screen_id;
			output_type = (disp_output_type)ioctl(fdDisplay, DISP_GET_OUTPUT_TYPE, (void*)args);
			
			if((output_type != DISP_OUTPUT_TYPE_NONE) && (output_type != -1))
			{
				printf("the output type: %d \n",screen_id);
				break;
			}
		}
	args[0] = 0;
		nScreenWidth  = ioctl(fdDisplay, DISP_GET_SCN_WIDTH, (void *)args);
		nScreenHeight = ioctl(fdDisplay, DISP_GET_SCN_HEIGHT,(void *)args);
		SetLayerParam();
		printf("%d ==  %d == \n", nScreenWidth,nScreenHeight);
while(i < 255)
{
	memset(framebuff,i,200*100*4);
	usleep(10000);
	i++;
}

	close(fdDisplay);
#else
unsigned int fbAddr[3];
	struct win_info info;

disp_init();

		fbAddr[0] = (unsigned int )framebuff;
									

	//logd("addr[0]:%x addr[1]:%x addr[2]:%x\n",fbAddr[0],fbAddr[1],fbAddr[2]);
	//logd("1:%d 2:%d 3:%d 4:%d 5:%d 6:%d\n",lc->nDisplayWidth,lc->nDisplayHeight,lc->nLeftOff,lc->nTopOff,
		//lc->nScreenWidth,lc->nScreenHeight);
	//logd("linestride:%d\n",pPicture->nLineStride);
	info.width = 400;
	info.height = 200;
	info.src_win.outLeftOffset = 0;
	info.src_win.outTopOffset = 0;
	info.src_win.outRightOffset = 400;
	info.src_win.outBottomOffset = 200;

	info.scn_win.outLeftOffset = 0;
	info.scn_win.outTopOffset = 0;
	info.scn_win.outRightOffset = 480;
	info.scn_win.outBottomOffset = 272;
	//logd("set param pid:%d\n",getpid());
	disp_video_chanel_open();
	if(0 != disp_video_layer_param_set(fbAddr[0], DISP_ARGB8888_INTERLEAVED, info, DISP_UI_ALPHA))
	{
		printf("set video layer param fail!\n");
	}

while(i < 255)
{
	memset(framebuff,i,200*100*4);
	usleep(10000);
	i++;
}
disp_video_chanel_close();

disp_uninit();

#endif
	return 0;
}
