# Core Makefile for GP32 development using GCC
# Written 2002 by Christian Nowak <chnowak@web.de>
# Patched 2004 by DJWillis for GamePark SDK project
# compatability for newer GCC's using Patched GameParkSDK
# Version 2.0

# File Name
TARGET		=	WB32Core
# Full Application Name
APPNAME		=	WonderBoi32 (new core)
# Author
AUTHOR		=	Rattboi
# FXE Tool (b2fxe or b2fxec)
FXETOOL		=	b2fxec

# gpSDK root directory
GPSDK           =       /c/devkitARM/gamepark_sdk
#MAC#			/Users/Bradon/devkitARM/gamepark_sdk
#HOME#			/c/devkitARM/gamepark_sdk
#WORK#			/d/devkitARM/gamepark_sdk

				
# GCC Tool-chain
CC			=	arm-elf-gcc
CXX			=	arm-elf-g++
LD			=	arm-elf-gcc
AS			=	arm-elf-as
OBJCOPY			=	arm-elf-objcopy
 
.SFILES			=	wscbmp.s \
					horz_render_normal.s \
					vert_render_normal.s \
					horz_render_hstretch.s \
					horz_render_hvstretch.s \
					../cpuspeed.s
 
.CFILES			=	gpstart.c \
					gpmain.c \
					WS.c \
					WSDraw.c \
					WSInput.c \
					WSSound.c \
                    nec/nec.c \
					../OKF/global.c \
					../OKF/clock.c \
					../OKF/graphics16.c \
					../OKF/okf.c \
					../OKF/list.c \
					../OKF/folder.c \
					../file_dialog.c \
					../setcpuspeed.c \
					../menu.c \
					../misc_str_func.c \
					../zlib/unzip.c \
					../selector.c \
					../OKF/fileSelector.c \
					../filecache.c 

.CXXFILES		=	Profiler.cpp

OBJS			=	$(.SFILES:.s=.o) $(.CFILES:.c=.o) $(.CXXFILES:.cpp=.o)
 
CFLAGS			=	-marm \
				-march=armv4t \
		       		-mapcs \
		       		-O2 \
	       			-fomit-frame-pointer \
	       			-finline-functions \
		       		-fshort-enums \
		       		-ffast-math \
	       			-mstructure-size-boundary=8 \
		       		-mno-thumb-interwork \
		       		-Wno-multichar \
				-I$(GPSDK)/include \
				-I. \
                                -I../zlib \
                                -I./nec \
                                -I../gplibs

CXXFLAGS			=	-marm \
				-march=armv4t \
		       		-mapcs \
		       		-O2 \
	       			-fomit-frame-pointer \
	       			-finline-functions \
		       		-fshort-enums \
		       		-ffast-math \
	       			-mstructure-size-boundary=8 \
		       		-mno-thumb-interwork \
		       		-Wno-multichar \
				-I$(GPSDK)/include \
				-I. \
                                -I../zlib \
                                -I./nec \
                                -I../gplibs

LIBDIRS                 =       -L$(GPSDK)/lib -L../gplibs -L../zlib
GP_LIBS         =       -lgpgraphic -lgpgraphic16 -lgpmem -lgpos -lgpstdlib -lgpstdio -lgpsound -lgpg_ex01 -lgpfont -lgpfont16
LIBS			=	-lz -lm -lc
 
LDFLAGS			=	$(LDSPECS) \
				-Wl,-Map,$(MAPFILE) \
				$(LIBDIRS) \
				$(GP_LIBS) \
				$(LIBS)
 
LDSPECS			=	-specs=gp32_gpsdk.specs
 
LINK			=	$(LD) -o $(GP_OBJS) $@ $^ $(LDFLAGS) 
 
# Outputs
ELF			=	$(TARGET).elf
MAPFILE			=	$(TARGET).map
BIN			=	$(TARGET).bin
GXB			=	$(TARGET).gxb
FXE			=	$(TARGET).fxe
 
.PHONY: all clean depend

all: $(GXB)
 
fxe: $(FXE)

test: $(GXB)
				pclink -e $(GXB)

upload: $(FXE)
				pclink -u $(FXE) -p gp:\\gpmm\\

clean:
				rm -f $(OBJS) $(MAPFILE) $(BIN) $(ELF) $(FXE) $(GXB)

$(FXE): $(GXB)
				$(FXETOOL) -b $(TARGET).bmp -f -a "$(AUTHOR)" -t "$(APPNAME)" $< $@

$(ELF): $(OBJS)
				$(LINK)
 
$(BIN): $(ELF)
				$(OBJCOPY) -O binary $< $@
 
$(GXB): $(BIN)
				$(FXETOOL) -g $< $@
