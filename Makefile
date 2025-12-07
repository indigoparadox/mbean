
# vim: ft=make noexpandtab

#VDP=1
#WASM_SDL_VER=1
#WING=1
FORCE_MVFS := 1
#RETROFLAT_SOUND=1
C_FILES := src/main.c src/mbean.c
RETROFLAT_DOS_MEM_LARGE=1
#RETROFLAT_FORCE_LOG_MFILE := 1
#SDL_VER_UNIX := 1

GLOBAL_DEFINES += -DMBEAN_COMMIT_HASH="\"$(shell git rev-parse HEAD)\""

GLOBAL_DEFINES += -DRETROFLAT_NO_RETROTILE -DMAUG_NO_MHTML -DRETROFLAT_NO_RETROGXC -DMAUG_NO_MPLUG -DMAUG_NO_MJSON -DRETROFLAT_NO_STRING

#GLOBAL_DEFINES += -DRETROSND_TRACE_LVL=1
#GLOBAL_DEFINES += -DRETROFLAT_NDS_WASD
#GLOBAL_DEFINES += -DMAUG_WCHAR -DUNICODE
#GLOBAL_DEFINES += -DRETROFONT_TRACE_LVL=1
#GLOBAL_DEFINES += -DMBEAN_SOUND

include maug/Makefile.inc

# Target-specific options.
.PHONY: clean

all: mbean.$(MAUG_UNIX).sdl mbean.$(MAUG_UNIX).ale mbeanb.exe mbeannt.exe mbean.html mbeanw.exe mbeanb.exe mbean64.exe mbean.m68k.dsk mbean.$(MAUG_UNIX).xlib

$(eval $(call MVFS,unscii_8.hex unscii_x.hex))

# PalmOS

$(eval $(call TGTPALM,mbean))

# Nintendo DS

$(eval $(call TGTNDSLIBN,mbean,mbean.bmp))

# Nintendo Wii

$(eval $(call TGTWIILIBO,mbean))

# Nintendo Switch

$(eval $(call TGTNXSDL,mbean))

# Unix Allegro

$(eval $(call TGTUNIXALE,mbean))

$(eval $(call TGTTGZ,mbeanale,mbean.$(MAUG_UNIX).ale README.md))

# Unix SDL

$(eval $(call TGTUNIXSDL,mbean))

$(eval $(call TGTTGZ,mbeansdl,mbean.$(MAUG_UNIX).sdl README.md))

# Unix (xlib)

$(eval $(call TGTUNIXXLIB,mbean))

$(eval $(call TGTTGZ,mbeanxlib,mbean.$(MAUG_UNIX).xlib README.md))

# WASM

$(eval $(call TGTWASMSDL,mbean))

# DOS

$(eval $(call TGTDOSALE,mbean))

$(eval $(call TGTDOSBIOS,mbean))

$(eval $(call TGTZIP,mbeanb,mbeanb.exe README.md))

# Win386

$(eval $(call TGTWINICO,mbean))

$(eval $(call TGTWIN386,mbean))

# Win

$(eval $(call TGTWIN16,mbean))

$(eval $(call TGTISO,mbeanw3,mbeanw3.exe README.md))

$(eval $(call TGTZIP,mbeanw3,mbeanw3.exe README.md))

# WinNT

$(eval $(call TGTWINNT,mbean))

$(eval $(call TGTISO,mbeannt,mbeannt.exe README.md))

$(eval $(call TGTZIP,mbeannt,mbeannt.exe README.md))

$(eval $(call TGTWINSDL,mbean))

$(eval $(call TGT_GCC64_WIN_WIN32,mbean))

$(eval $(call TGT_CECL_WINCE_SH3,mbean))

$(eval $(call TGT_CECL_WINCE_MIPS,mbean))

# OS/2

$(eval $(call TGTOS2SDL,mbean))

# Mac

$(eval $(call TGTMAC68K,mbean,,mbean.bmp,mBEN))

$(eval $(call TGTMAC68K7,mbean,,mbean.bmp,mBEN))

# Clean

clean:
	rm -rf $(CLEAN_TARGETS)

