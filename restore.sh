#!/bin/bash

MODULE_NAME=hid_mt_compat

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

if [[ -e ${INSTALLED_TARGET}.orig ]]
then
  echo "Restoring original module"
  mv ${INSTALLED_TARGET}.orig ${INSTALLED_TARGET}
else
  if [[ -e ${COMPILED_TARGET} && -e ${INSTALLED_TARGET} ]]
  then
    if diff -q ${COMPILED_TARGET} ${INSTALLED_TARGET} > /dev/null ; then
      # the file are equals, it is safe to remove it
      echo "Removing installed module" ${INSTALLED_TARGET}
      rm ${INSTALLED_TARGET}
    else
      echo "Keeping installed module" ${INSTALLED_TARGET}
      echo "  --> You may need to remove it manually if you know what you are doing."
    fi
  else
    echo "Keeping installed module" ${INSTALLED_TARGET}
    echo "  --> You may need to remove it manually if you know what you are doing."
  fi
fi

echo "depmod -a"
depmod -a
