obj-m+=dice.o

CFLAGS='-std=c11'

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules CFLAGS=$(CFLAGS)
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean CFLAGS=$(CFLAGS)