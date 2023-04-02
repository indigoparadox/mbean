
# vim: ft=make noexpandtab

C_FILES := src/main.c src/mbean.c

include maug/Makefile.inc

# Target-specific options.
.PHONY: clean

all: mbean.sdl mbean.ale mbeand.exe mbeannt.exe mbean.html mbeanw3.exe

# Nintendo DS

#$(eval $(call TGTNDSLIBN,mbean,mbean.bmp))

# Unix Allegro

$(eval $(call TGTUNIXALE,mbean))

# Unix SDL

#$(eval $(call TGTSDLICO,mbean))

$(eval $(call TGTUNIXSDL,mbean))

# WASM

$(eval $(call TGTWASMSDL,mbean))

# DOS

$(eval $(call TGTDOSALE,mbean))

# Win386

$(eval $(call TGTWINICO,mbean))

$(eval $(call TGTWIN386,mbean))

$(eval $(call TGTWIN16,mbean))

# WinNT

$(eval $(call TGTWINNT,mbean))

$(eval $(call TGTWINSDL,mbean))

# OS/2

$(eval $(call TGTOS2SDL,mbean))

# Mac

$(eval $(call TGTMAC6GCC,mbean))

# Clean

clean:
	rm -rf $(CLEAN_TARGETS)

