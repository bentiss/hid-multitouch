MODULE_NAME := hid-multitouch

hid_mt_compat-y		:= $(MODULE_NAME).o

obj-m			+= hid_mt_compat.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

install: $(MODULE_NAME).ko
	/bin/bash install.sh

uninstall:
	/bin/bash restore.sh

clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean

