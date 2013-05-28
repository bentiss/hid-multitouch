MODULE_NAME := hid_mt_compat

$(MODULE_NAME)-y	:= hid-multitouch.o
$(MODULE_NAME)-y	+= compat-hid.o
$(MODULE_NAME)-y	+= compat-mt.o
$(MODULE_NAME)-y	+= compat-input.o

obj-m			+= hid_mt_compat.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

install: $(MODULE_NAME).ko $(MODULE_NAME).mod.c
	/bin/bash install.sh

uninstall:
	/bin/bash restore.sh

clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean

