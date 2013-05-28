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
