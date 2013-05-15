#!/bin/bash

MODULE_NAME=hid-multitouch

if [[ `id -u` != 0 ]]
then
  echo "Must be run as root"
  exit 1
fi

WORKING_DIR=$(pwd)
TARGET=${MODULE_NAME}.ko

INSTALL_PATH=/lib/modules/`uname -r`/kernel/drivers/hid

# check if the modules are compiled
if [[ ! -e $WORKING_DIR/${TARGET} ]]
then
  echo "please run make before install."
  echo "Aborting"
  exit 1
fi

# backup shipped modules
cd $INSTALL_PATH
if [[ ! -e ${TARGET}.orig ]]
then
  mv ${TARGET} ${TARGET}.orig
fi


cd $WORKING_DIR
echo ${TARGET} "->" ${INSTALL_PATH}/${TARGET}
cp ${TARGET} ${INSTALL_PATH}/${TARGET}

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

