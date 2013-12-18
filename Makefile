MODULE_NAME := hid_mt_compat

LINUXINCLUDE := -I$(PWD)/include $(LINUXINCLUDE)

$(MODULE_NAME)-y	:= hid-multitouch.o

obj-m			+= $(MODULE_NAME).o

HID_MODULE_NAME := hid_compat

$(HID_MODULE_NAME)-y	:= hid-core.o
$(HID_MODULE_NAME)-y	+= hid-input.o
$(HID_MODULE_NAME)-$(CONFIG_HIDRAW)	+= hidraw.o
$(HID_MODULE_NAME)-y	+= compat-mt.o
$(HID_MODULE_NAME)-y	+= compat-input.o

ifdef CONFIG_DEBUG_FS
	$(HID_MODULE_NAME)-objs		+= hid-debug.o
endif

obj-m		+= $(HID_MODULE_NAME).o
obj-m		+= uhid_compat.o
obj-m		+= hid-generic-compat.o

obj-m			+= usbhid/

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

install: $(MODULE_NAME).ko $(MODULE_NAME).mod.c
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules_install
	/bin/bash install.sh

uninstall:
	/bin/bash restore.sh

clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean

