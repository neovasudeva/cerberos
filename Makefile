.DEFAULT_GOAL=all

# TODO: add compiler and linker to PATH
CC=~/opt/cross/bin/x86_64-elf-gcc
LD=~/opt/cross/bin/x86_64-elf-ld
ASM=nasm

CFLAGS?= \
	-std=gnu11 \
	-Wall \
	-Wextra \
	-O2 \
	-pipe \
	-g \
	-ggdb

KCFLAGS= \
	$(CFLAGS) \
	-Ideps				 \
	-Isrc/kernel/include \
	-Isrc/lib			 \
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
	-Tsrc/kernel/boot/linker.ld \
	-z max-page-size=0x1000 \
	--no-dynamic-linker 

KASMFLAGS= \
	-felf64 \
	-Isrc/kernel/include/

MKPDIR=mkdir -p $(@D)

include src/kernel/.build.mk
include src/sysroot/.build.mk

QEMUFLAGS= \
	-m 512 \
	-smp 1 \
	-drive format=raw,media=cdrom,file=$(SYSROOT_ISO) \
	-serial stdio \
	-s -S

.PHONY: all
all: $(SYSROOT_ISO)

run: $(SYSROOT_ISO)
	qemu-system-x86_64 $(QEMUFLAGS) 

.PHONY: clean
clean:
	rm -rf build
