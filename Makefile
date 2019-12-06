CC = gcc
CFLAGS = -std=c89 -pedantic -Wall
LDFLAGS = -L/usr/lib64/nvidia -L/usr/X11R6/lib -lGL -lGLU -lglut -lm

.PHONY: all
all: crt

crt: crt.o util.o
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f *.o crt
