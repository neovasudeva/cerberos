LIB_C_SRC+= \
	$(wildcard src/lib/ds/*.c) \
	$(wildcard src/lib/mem/*.c) 

LIB_DEPS=$(LIB_C_SRC:.c=.d)

-include $(LIB_DEPS)
build/lib/%.c.o: src/lib/%.c
	$(MKPDIR)
	$(CC) $(KCFLAGS) -c $^ -o $@ 