#!/bin/bash

if [[ `id -u` != 0 ]]
then
  echo "Must be run as root"
  exit 1
fi

WORKING_DIR=$(pwd)

INSTALL_PATH=/lib/modules/`uname -r`/kernel/drivers/hid

cd $INSTALL_PATH
for i in *.ko.orig usbhid/*.ko.orig
do
  if [[ -e $i ]]
  then
    mv $i ${i%.orig}
  fi
done


echo "depmod -a"
depmod -a

if [[ -e "/usr/bin/dracut" ]]
then
  echo "dracut -f"
  dracut -f
else
  echo "update-initramfs -u"
  update-initramfs -u
fi

