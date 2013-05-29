#!/bin/bash

MODULE_NAME=hid_mt_compat
UDEV_RULE=/etc/udev/rules.d/41-hid-multitouch.rules
BASH_LOADING_SCRIPT=/etc/udev/load_hid_multitouch.sh

if [[ `id -u` != 0 ]]
then
  echo "Must be run as root"
  exit 1
fi

WORKING_DIR=$(pwd)
TARGET=${MODULE_NAME}.ko

INSTALL_PATH=/lib/modules/`uname -r`/kernel/drivers/hid
COMPILED_TARGET=${WORKING_DIR}/${TARGET}
INSTALLED_TARGET=${INSTALL_PATH}/${TARGET}

# check if the modules are compiled
if [[ ! -e ${COMPILED_TARGET} ]]
then
  echo "please run make before install."
  echo "Aborting"
  exit 1
fi

# backup shipped modules
if [[ ! -e ${INSTALLED_TARGET}.orig && -e ${INSTALLED_TARGET} ]]
then
  if ! diff -q ${COMPILED_TARGET} ${INSTALLED_TARGET} > /dev/null ; then
    echo "backing up" ${INSTALLED_TARGET}
    mv ${INSTALLED_TARGET} ${INSTALLED_TARGET}.orig
  fi
fi

echo ${COMPILED_TARGET} "->" ${INSTALLED_TARGET}
cp ${COMPILED_TARGET} ${INSTALLED_TARGET}

echo "depmod -a"
depmod -a

rmmod ${MODULE_NAME} 2> /dev/null
modprobe ${MODULE_NAME}

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

/sbin/modprobe  \${HID_MULTITOUCH}

echo \${DEVICE} > \${USB_DRV_PATH}/\${DRIVER}/unbind
echo \${DEVICE} > \${USB_DRV_PATH}/\${USBHID}/bind
EOF
