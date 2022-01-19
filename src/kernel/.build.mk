KERNEL_C_SRC+= \
	$(wildcard src/kernel/boot/*.c) \
	$(wildcard src/kernel/cpu/*.c) \
	$(wildcard src/kernel/device/*.c) \
	$(wildcard src/kernel/mem/*.c) 

KERNEL_S_SRC+= \
	$(wildcard src/kernel/boot/*.S) \
	$(wildcard src/kernel/cpu/*.S) \
	$(wildcard src/kernel/device/*.S) 

KERNEL_DEPS=$(KERNEL_C_SRC:.c=.d)

-include $(KERNEL_DEPS)
build/kernel/%.c.o: src/kernel/%.c
	$(MKPDIR)
	$(CC) $(KCFLAGS) -c $^ -o $@ 

build/kernel/%.S.o: src/kernel/%.S
	$(MKDIR)
	$(ASM) $^ $(KASMFLAGS) -o $@ 
