
# vim: ft=make noexpandtab

#VDP=1
#WASM_SDL_VER=1
#WING=1
C_FILES := src/main.c src/mbean.c
RETROFLAT_DOS_MEM_LARGE=1
#GLOBAL_DEFINES := -DRETROSND_TRACE_LVL
GLOBAL_DEFINES := -DRETROFLAT_NO_RETROTILE -DMAUG_NO_MHTML -DRETROFLAT_NO_RETROGXC -DMAUG_NO_MPLUG -DMAUG_NO_MJSON -DRETROFLAT_NO_STRING
#GLOBAL_DEFINES := -DRETROFLAT_NDS_WASD
#GLOBAL_DEFINES += -DMAUG_WCHAR -DUNICODE
#GLOBAL_DEFINES += -DRETROFONT_TRACE_LVL=1

include maug/Makefile.inc

# Target-specific options.
.PHONY: clean

all: mbean.sdl mbean.ale mbeanb.exe mbeannt.exe mbean.html mbeanw.exe mbeanb.exe mbean64.exe

$(eval $(call MVFS,unscii-8.hex))

# Nintendo DS

$(eval $(call TGTNDSLIBN,mbean,mbean.bmp))

# Unix Allegro

$(eval $(call TGTUNIXALE,mbean))

# Unix SDL

#$(eval $(call TGTSDLICO,mbean))

$(eval $(call TGTUNIXSDL,mbean))

# WASM

$(eval $(call TGTWASMSDL,mbean))

# DOS

$(eval $(call TGTDOSALE,mbean))

$(eval $(call TGTDOSBIOS,mbean))

# Win386

$(eval $(call TGTWINICO,mbean))

$(eval $(call TGTWIN386,mbean))

$(eval $(call TGTWIN16,mbean))

# WinNT

$(eval $(call TGTWINNT,mbean))

$(eval $(call TGTWINSDL,mbean))

$(eval $(call TGT_GCC64_WIN_WIN32,mbean))

$(eval $(call TGT_CECL_WINCE_SH3,mbean))

$(eval $(call TGT_CECL_WINCE_MIPS,mbean))

# OS/2

$(eval $(call TGTOS2SDL,mbean))

# Mac

$(eval $(call TGTMAC6GCC,mbean))

# Clean

clean:
	rm -rf $(CLEAN_TARGETS)

