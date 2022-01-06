KERNEL_SRC+=$(wildcard src/arch/*.c)
KERNEL_OBJ=$(patsubst src/%.c, build/%.o, $(KERNEL_SRC))
KERNEL_DEPS=$(KERNEL_SRC:.c=.d)
KERNEL_BIN=build/kernel.elf

-include $(KERNEL_DEPS)
build/arch/%.o: src/arch/%.c
	$(MKPDIR)
	$(CC) $(KCFLAGS) -c $^ -o $@ 

$(KERNEL_BIN): $(KERNEL_OBJ)
	$(MKPDIR)
	$(LD) $(KERNEL_OBJ) $(KLDFLAGS) -o $@