PROJECT_DIR := $(shell pwd)
CXX=$(PROJECT_DIR)/../../../buildroot/output/rockchip_rk3128_game/host/bin/arm-buildroot-linux-gnueabihf-g++
CC=$(PROJECT_DIR)/../../../buildroot/output/rockchip_rk3128_game/host/bin/arm-buildroot-linux-gnueabihf-gcc
BIN = test

DESTDIR:=../../buildroot/output/rockchip_rk3032/target
STAGING_DIR = $(PROJECT_DIR)/../../../buildroot/output/rockchip_rk3128_game/host/arm-buildroot-linux-gnueabihf/sysroot


#OBJ =     main.o\

OBJ =     start.o\
		png.o \
		jpeg.o \
		fb_display.o \
		rthreads/rthreads.o \
		processdisp.o \
		textdraw.o \
		graphics.o \
		resources.o\
		parameter.o \
		../drm_display.o \
		keyEvents.o\
		uimode.o
#		keytone.o \
		sndwav_common.o \
		wav_parser.o\
		browser.o\
		ioapi.o \
		unzip.o \
testmode.o \
		key_test_qa.o \
		
#OBJ =     testdisp.o\
	disp.o
	
			

#-DHAVE_OVERLAY  -DHAVE_WAYLAND -DHAVE_KMS  -DHAVE_THREADS HAVE_EGL -DPIC -DHAVE_ENDIAN_H
CFLAGS =  -I. -DMODULE_DISP_COMPILE_CFG=1 -DOVERSCAN_PERCENT=0 -DDEFAULT_ROTATION=ROTATION_NONE\
		-I./include -DVID_DRM=1 -DFBV_SUPPORT_PNG -DFBV_SUPPORT_JPEG\
 -I$(STAGING_DIR)/usr/include \
          -I$(STAGING_DIR)/usr/include/alsa \
          -I$(STAGING_DIR)/usr/include/libdrm \
          -I$(STAGING_DIR)/usr/include \
          -L$(STAGING_DIR)/usr/lib \
          -L$(STAGING_DIR)/usr/lib \
          -lpthread  -ldrm -ljpeg -lpng -lz -lasound \
	    
#-lminui
$(BIN): $(OBJ)
	$(CC) -o $(BIN) $(OBJ) $(CFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) 
	
%.o: %.cpp
	$(CXX) -c $< -o $@ $(CFLAGS) 

clean:
	#rm -rf $(DESTDIR)/etc/res
	rm -rf $(DESTDIR)/usr/local/share/minigui
	rm -rf $(OBJ) $(BIN)

install:
	#cp -R $(PROJECT_DIR)/res $(DESTDIR)/etc
	cp $(BIN) $(DESTDIR)/usr/bin
	mkdir -p $(DESTDIR)/usr/local/share/
	cp -R $(PROJECT_DIR)/minigui $(DESTDIR)/usr/local/share/

