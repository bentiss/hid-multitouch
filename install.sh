#!/bin/bash

UDEV_RULE=/etc/udev/rules.d/41-hid-multitouch.rules
BASH_LOADING_SCRIPT=/etc/udev/load_hid_multitouch.sh

if [[ `id -u` != 0 ]]
then
  echo "Must be run as root"
  exit 1
fi

grep MODULE_ALIAS hid_mt_compat.mod.c | \
	sed 's/MODULE_ALIAS("hid:b.*v0000\(.*\)p0000\(.*\)");/DRIVER=="usbhid", ENV{MODALIAS}=="usb:v\1p\2d*", RUN+="\/bin\/sh \/etc\/udev\/load_hid_multitouch.sh usbhid %k"/' | \
	grep -v MODULE_ALIAS | \
	sort \
		> ${UDEV_RULE}

cat > ${BASH_LOADING_SCRIPT} <<EOF
#!/bin/bash

DRIVER=\$1
DEVICE=\$2

HID_DRV_PATH=/sys/bus/hid/drivers
USB_DRV_PATH=/sys/bus/usb/drivers

HID_MULTITOUCH=hid-mt-compat
USBHID=usbhid-compat

/sbin/modprobe \${HID_MULTITOUCH}
/sbin/modprobe \${USBHID}

echo \${DEVICE} > \${USB_DRV_PATH}/\${DRIVER}/unbind
echo \${DEVICE} > \${USB_DRV_PATH}/\${USBHID}/bind
EOF
