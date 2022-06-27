include src/kernel/.build.mk
include src/lib/.build.mk
include src/log/.build.mk

KBIN=out/kernel.elf

KOBJ= \
	$(patsubst src/%.c, out/%.c.o, $(KERNEL_C_SRC)) \
	$(patsubst src/%.S, out/%.S.o, $(KERNEL_S_SRC)) \
	$(patsubst src/%.c, out/%.c.o, $(LIB_C_SRC)) \
	$(patsubst src/%.c, out/%.c.o, $(LOG_C_SRC)) 

$(KBIN): $(KOBJ)
	$(MKPDIR)
	$(LD) $(KOBJ) $(KLDFLAGS) -o $@