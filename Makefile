PROG=strix-claw

CC=gcc
CFLAGS=-std=gnu99 -W -Wall -Werror -lusb-1.0

SOURCES := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o,$(SOURCES))
DEPS := $(patsubst %.o,%.dep,$(OBJS))

.PHONY: all clean
all: $(PROG)

clean:
	-rm -f $(PROG) $(OBJS) $(DEPS)

$(PROG): $(OBJS) $(DEPS)
	$(CC) $(OBJS) $(CFLAGS) -o $@

%.dep: %.c Makefile
	$(CC) $(CFLAGS) -MMD -MP -MF $@ -c $<

%.o: %.c %.dep
	$(CC) $(CFLAGS) -c $< -o $@

-include $(wildcard *.dep)
