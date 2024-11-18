obj-m += vuln.o

KDIR := ./6.8.0-47-generic/build

PWD := $(shell pwd)

all: 
	sudo make -C $(KDIR) M=$(PWD) modules

clean:
	sudo make -C $(KDIR) M=$(PWD) clean
