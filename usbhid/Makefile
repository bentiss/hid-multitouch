#
# Makefile for the USB input drivers
#

MODULE_NAME := usbhid_compat

# Multipart objects.
$(MODULE_NAME)-y	:= hid-core.o hid-quirks.o

# Optional parts of multipart objects.

ifeq ($(CONFIG_USB_HIDDEV),y)
	$(MODULE_NAME)-y	+= hiddev.o
endif
ifeq ($(CONFIG_HID_PID),y)
	$(MODULE_NAME)-y	+= hid-pidff.o
endif

obj-m		+= $(MODULE_NAME).o
