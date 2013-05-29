#!/bin/bash

MODULE_NAMES="hid_compat hid_mt_compat uhid_compat usbhid_compat"
UDEV_RULE=/etc/udev/rules.d/41-hid-multitouch.rules
BASH_LOADING_SCRIPT=/etc/udev/load_hid_multitouch.sh

if [[ `id -u` != 0 ]]
then
  echo "Must be run as root"
  exit 1
fi

for MODULE_NAME in ${MODULE_NAMES}
do
  TARGET=${MODULE_NAME}.ko

  INSTALL_PATH=/lib/modules/`uname -r`/extra

  INSTALLED_TARGET=`find ${INSTALL_PATH} -name ${TARGET}`
  if [[ -e ${INSTALLED_TARGET} ]]
  then
    echo "Removing installed module" ${INSTALLED_TARGET}
    rm ${INSTALLED_TARGET}
  fi
done

if [[ -e ${UDEV_RULE} ]]
then
  echo "removing udev rule" ${UDEV_RULE}
  rm ${UDEV_RULE}
fi

if [[ -e ${BASH_LOADING_SCRIPT} ]]
then
  echo "removing udev script" ${BASH_LOADING_SCRIPT}
  rm ${BASH_LOADING_SCRIPT}
fi

echo "depmod -a"
depmod -a
