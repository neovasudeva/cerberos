KERNEL_C_SRC+= \
	$(wildcard src/kernel/acpi/*.c) \
	$(wildcard src/kernel/boot/*.c) \
	$(wildcard src/kernel/cpu/*.c) \
	$(wildcard src/kernel/dev/*.c) \
	$(wildcard src/kernel/mm/*.c) \
	$(wildcard src/kernel/intr/*.c) \

KERNEL_S_SRC+= \
	$(wildcard src/kernel/acpi/*.S) \
	$(wildcard src/kernel/boot/*.S) \
	$(wildcard src/kernel/cpu/*.S) \
	$(wildcard src/kernel/dev/*.S) \
	$(wildcard src/kernel/mm/*.S) \
	$(wildcard src/kernel/intr/*.S) 

KERNEL_DEPS=$(KERNEL_C_SRC:.c=.d)

-include $(KERNEL_DEPS)
build/kernel/%.c.o: src/kernel/%.c
	$(MKPDIR)
	$(CC) $(KCFLAGS) -c $^ -o $@ 

build/kernel/%.S.o: src/kernel/%.S
	$(MKDIR)
	$(ASM) $^ $(KASMFLAGS) -o $@ 
