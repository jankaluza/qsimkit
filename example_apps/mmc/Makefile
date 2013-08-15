CC=msp430-gcc
CFLAGS=-Os -Wall -g -I. -mmcu=msp430f2012

OBJS=main.c mmc.c hal_SPI.c


all: $(OBJS)
	$(CC) $(CFLAGS) -o main.elf $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -fr main.elf *.o
