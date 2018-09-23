obj-m := assignment6.o

KERNEL_DIR = /usr/src/linux-headers-$(shell uname -r)

all:
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(shell pwd) 
	gcc -o testcase4 testcase4.c -lpthread
	gcc -o testcase3 testcase3.c -lpthread
	gcc -o testcase2 testcase2.c
	gcc -o testcase1 testcase1.c
	gcc -o sample sampleprogram.c

clean:
	rm -rf *.o *.ko *.mod.* *.symvers *.order *~ 
