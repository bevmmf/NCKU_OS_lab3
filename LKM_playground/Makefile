target_module := hello_world_km
obj-m += $(target_module).o

KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(CURDIR)

.PHONY : all clean load unload
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
load:
	sudo insmod $(target_module).ko

unload:
	sudo rmmod $(target_module)




