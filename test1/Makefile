
CFLAGS = -I/usr/local/include
LDFLAGS = -L/usr/local/lib -lyaml

test_ora: test_ora.o config.o
	$(CC) -o $@ $^ $(LDFLAGS)
