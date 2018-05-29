HDIR=../headers
ODIR=obj
SDIR=.

CC=gcc
CFLAGS = -O2 -Wall -std=c11 -g -I$(HDIR)

DEPS=$(HDIR)/$(wildcard *.h)
SOURCES=$(wildcard *.c)
SOURCES_OBJ=$(foreach o, $(patsubst %.c,%.o,$(SOURCES)), $(ODIR)/$o)


print-% : ; @echo $* = $($*)

$(ODIR)/%.o : %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

program: $(SOURCES_OBJ)
	$(CC) $(CFLAGS) $(wildcard $(ODIR)/*.o) -o program





clean:
	rm obj/*.o
	rm program
