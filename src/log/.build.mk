LOG_C_SRC+= \
	$(wildcard src/log/*.c) \

LOG_DEPS=$(LOG_C_SRC:.c=.d)

-include $(LOG_DEPS)
build/log/%.c.o: src/log/%.c
	$(MKPDIR)
	$(CC) $(KCFLAGS) -c $^ -o $@ 