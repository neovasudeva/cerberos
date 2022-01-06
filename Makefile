.DEFAULT_GOAL=all

# TODO: add compiler and linker to PATH
CC=~/opt/cross/bin/x86_64-elf-gcc
LD=~/opt/cross/bin/x86_64-elf-ld

CFLAGS?= \
	-std=gnu11 \
	-Wall \
	-Wextra \
	-Werror \
	-O2 \
	-pipe \

KCFLAGS= \
	$(CFLAGS) \
	-Ideps/stivale		 \
	-Ideps/limine 		 \
	-ffreestanding       \
	-fno-stack-protector \
	-fno-pic             \
	-mabi=sysv           \
	-mno-80387           \
	-mno-mmx             \
	-mno-3dnow           \
	-mno-sse             \
	-mno-sse2            \
	-mno-red-zone        \
	-mcmodel=kernel      \
	-MMD

KLDFLAGS= \
	-static \
	-nostdlib \
	-Tsrc/arch/linker.ld \
	-z max-page-size=0x1000 \
	--no-dynamic-linker 

MKPDIR=mkdir -p $(@D)

include src/arch/.build.mk
include src/sysroot/.build.mk

QEMUFLAGS= \
	-m 512 \
	-smp 1 \
	-drive format=raw,media=cdrom,file=$(SYSROOT_ISO) \

.PHONY: all
all: $(SYSROOT_ISO)

run: $(SYSROOT_ISO)
	qemu-system-x86_64 $(QEMUFLAGS) 

.PHONY: clean
clean:
	rm -rf build
