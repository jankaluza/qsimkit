CC=msp430-gcc
CFLAGS=-Os -Wall -g -mmcu=msp430x168

OBJS=main.c


all: $(OBJS)
	$(CC) $(CFLAGS) -o main.elf $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -fr main.elf *.o
