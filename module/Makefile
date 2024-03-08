obj-m	:= numbaseball.o
numbaseball-objs	:= driver.o fpga.o playball.o

KDIR	:= /work/achroimx_kernel
PWD		:= $(shell pwd)

MOD 	:= $(subst .o,.ko,$(obj-m))

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules ARCH=arm

push: default
	adb push -p $(MOD) /data/local/tmp

clean:
	rm -rf *.o
	rm -rf *.ko
	rm -rf .tmp_versions
	rm -rf .*cmd
	rm -rf *.symvers
	rm -rf *.order
	rm -rf *.mod.c