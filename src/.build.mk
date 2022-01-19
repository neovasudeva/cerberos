include src/kernel/.build.mk
include src/lib/.build.mk
include src/log/.build.mk

KBIN=build/kernel.elf

KOBJ= \
	$(patsubst src/%.c, build/%.c.o, $(KERNEL_C_SRC)) \
	$(patsubst src/%.S, build/%.S.o, $(KERNEL_S_SRC)) \
	$(patsubst src/%.c, build/%.c.o, $(LIB_C_SRC)) \
	$(patsubst src/%.c, build/%.c.o, $(LOG_C_SRC)) 

$(KBIN): $(KOBJ)
	$(MKPDIR)
	$(LD) $(KOBJ) $(KLDFLAGS) -o $@