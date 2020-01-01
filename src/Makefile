
# vim: ft=make noexpandtab

OBJECTS := mbean.o

BINDIR_LINUX = ../bin/linux
BINDIR_MSP430G2553 = ../bin/msp430g2553

OBJDIR_TESTS = ../obj/test_mbean

MD=mkdir -v -p

CFLAGS := -Wall -Werror -I../../mfat/src

$(BINDIR_LINUX)/libmbean.a: OBJDIR := ../obj/linux

$(BINDIR_MSP430G2553)/libmbean.a: OBJDIR := ../obj/msp430g2553
$(BINDIR_MSP430G2553)/libmbean.a: AR := msp430-ar
$(BINDIR_MSP430G2553)/libmbean.a: CC := msp430-gcc
$(BINDIR_MSP430G2553)/libmbean.a: CFLAGS += \
	-mmcu=msp430g2553 \
	-g \
	-pg \
	-Os \
	-DUSE_DISK_RO \
	-fno-builtin \
	-ffreestanding \
	-nostdlib

test_mbean: LDFLAGS += $(shell pkg-config --libs check) -L../bin/linux/ -lmbean
test_mbean: CFLAGS += -DCHECK -g -Wall -Werror
test_mbean: OBJDIR := $(OBJDIR_TESTS)

mbean: LDFLAGS += -L../bin/linux/ -lmbean
mbean: CFLAGS += -g -Wall -Werror
mbean: OBJDIR := ../obj/mbean

all: $(BINDIR_LINUX)/libmbean.a $(BINDIR_MSP430G2553)/libmbean.a mbean

test_mbean: $(BINDIR_LINUX)/libmbean.a $(OBJDIR_TESTS)/check.o $(OBJDIR_TESTS)/check_mbean.o
	$(CC) -o $@ $^ $(LDFLAGS)

mbean: $(BINDIR_LINUX)/libmbean.a ../obj/linux/mbean.o ../obj/linux/main.o
	$(CC) -o $@ $^ $(LDFLAGS)

$(BINDIR_LINUX)/libmbean.a: $(addprefix ../obj/linux/,$(OBJECTS))
	$(MD) $(BINDIR_LINUX)
	$(AR) rcs $@ $^

$(BINDIR_MSP430G2553)/libmbean.a: $(addprefix ../obj/msp430g2553/,$(OBJECTS))
	$(MD) $(BINDIR_MSP430G2553)
	$(AR) rcs $@ $^

../obj/linux/%.o: $(notdir %.c)
	$(MD) $(OBJDIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJDIR_TESTS)/%.o: ../tests/$(notdir %.c)
	$(MD) $(OBJDIR)
	$(CC) -c -o $@ $< $(CFLAGS)

../obj/msp430g2553/%.o: $(notdir %.c)
	$(MD) $(OBJDIR)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -rf ../obj; \
	rm -f test_mbean; \
	rm -rf ../bin

