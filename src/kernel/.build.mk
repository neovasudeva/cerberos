KERNEL_C_SRC+= \
	$(wildcard src/kernel/boot/*.c) \
	$(wildcard src/kernel/cpu/*.c) \
	$(wildcard src/kernel/device/*.c) 

KERNEL_S_SRC+= \
	$(wildcard src/kernel/boot/*.S) \
	$(wildcard src/kernel/cpu/*.S) \
	$(wildcard src/kernel/device/*.S)

KERNEL_OBJ= \
	$(patsubst src/%.c, build/%.c.o, $(KERNEL_C_SRC)) \
	$(patsubst src/%.S, build/%.S.o, $(KERNEL_S_SRC))

KERNEL_DEPS=$(KERNEL_C_SRC:.c=.d)
KERNEL_BIN=build/kernel.elf

-include $(KERNEL_DEPS)
build/kernel/%.c.o: src/kernel/%.c
	$(MKPDIR)
	$(CC) $(KCFLAGS) -c $^ -o $@ 

build/kernel/%.S.o: src/kernel/%.S
	$(MKDIR)
	$(ASM) $^ $(KASMFLAGS) -o $@ 

$(KERNEL_BIN): $(KERNEL_OBJ)
	$(MKPDIR)
	$(LD) $(KERNEL_OBJ) $(KLDFLAGS) -o $@