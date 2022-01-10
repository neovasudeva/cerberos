KERNEL_C_SRC+=$(wildcard src/kernel/arch/*.c)
KERNEL_S_SRC+=$(wildcard src/kernel/arch/*.S)
KERNEL_OBJ= \
	$(patsubst src/%.c, build/%.c.o, $(KERNEL_C_SRC)) \
	$(patsubst src/%.S, build/%.S.o, $(KERNEL_S_SRC))

KERNEL_DEPS=$(KERNEL_SRC:.c=.d)
KERNEL_BIN=build/kernel.elf

-include $(KERNEL_DEPS)
build/kernel/arch/%.c.o: src/kernel/arch/%.c
	$(MKPDIR)
	$(CC) $(KCFLAGS) -c $^ -o $@ 

build/kernel/arch/%.S.o: src/kernel/arch/%.S
	$(MKDIR)
	$(ASM) $^ $(KASMFLAGS) -o $@ 

$(KERNEL_BIN): $(KERNEL_OBJ)
	$(MKPDIR)
	$(LD) $(KERNEL_OBJ) $(KLDFLAGS) -o $@