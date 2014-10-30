MODULE_NAME := hid_multitouch

LINUXINCLUDE := -I$(SUBDIRS)/include $(LINUXINCLUDE)

obj-m	:= hid-multitouch.o

KDIR := /lib/modules/$(shell uname -r)/build
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

install: $(MODULE_NAME).ko $(MODULE_NAME).mod.c
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules_install

uninstall:
	/bin/bash restore.sh

clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean

