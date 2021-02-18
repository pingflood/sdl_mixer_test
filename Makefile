BUILDTIME=$(shell date +'\"%Y-%m-%d %H:%M\"')

CC ?= $(CROSS_COMPILE)gcc
CXX ?= $(CROSS_COMPILE)g++
STRIP ?= $(CROSS_COMPILE)strip

SYSROOT     ?= $(shell $(CC) --print-sysroot)
SDL_CFLAGS  ?= $(shell $(SYSROOT)/usr/bin/sdl-config --cflags)
SDL_LIBS    ?= $(shell $(SYSROOT)/usr/bin/sdl-config --libs)

CFLAGS = -ggdb -g3 $(SDL_CFLAGS) -mhard-float -mips32 -mno-mips16
CFLAGS += -std=c++11 -fdata-sections -ffunction-sections -fno-exceptions -fno-math-errno -fno-threadsafe-statics
CFLAGS += -lSDL_image -lSDL_ttf -std=c++11 -fdata-sections -ffunction-sections -fno-exceptions -fno-math-errno -fno-threadsafe-statics
CFLAGS += -Isrc

LDFLAGS = $(SDL_LIBS) -lfreetype -lSDL_image -lSDL_ttf -lSDL_mixer -lSDL_gfx -lSDL -lpthread -lmpg123
LDFLAGS +=-Wl,--as-needed -Wl,--gc-sections -s

all:
	$(CXX) $(CFLAGS) $(LDFLAGS) src/play.cpp -o play/play.dge

linux:
	gcc src/play.cpp -g -o play/play.dge -DDEBUG -lSDL_image -lSDL_mixer -lSDL -lSDL_ttf -Isrc -I/usr/include/SDL

opk: all
	@mksquashfs \
	play/default.retrofw.desktop \
	play/sf2.mp3 \
	play/pinball_mania_2.mod \
	play/ryu.wav \
	play/zangief.ogg \
	play/bt-theme.mid \
	play/sonicboom.mp3 \
	play/shoryuken.ogg \
	play/hadouken.wav \
	play/play.dge \
	play/play.png \
	play/play.opk \
	-all-root -noappend -no-exports -no-xattrs
