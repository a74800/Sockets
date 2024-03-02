CC = gcc
CFLAGS = -Wall -std=gnu99
LDFLAGS = -lm -lpthread

SRC1 = server.c
SRC2 = client.c
SRC3 = server2.c
SRC4 = server2_no_zombies.c

TARGETS = server client server2 server2_no_zombies

all: $(TARGETS)
	@echo "Done!"
.PHONY: all

exemplo1: $(SRC1)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

exemplo2: $(SRC2)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

exemplo3: $(SRC3)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

exemplo4: $(SRC4)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


clean:
	/bin/rm -f $(TARGETS)