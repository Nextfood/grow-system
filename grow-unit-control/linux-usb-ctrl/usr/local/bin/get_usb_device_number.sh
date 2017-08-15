#!/bin/bash

function exception {
    echo "$1"
    exit 1
}


function usage {
    SCRIPTNAME=$( basename ${0} )
    echo "Usage: ${SCRIPTNAME} <device>"
    exit 1
}



DEVICE=$1

if [ -z "$DEVICE" ]; then
    usage
fi


SYS_DEVICE_DIR=/sys/bus/usb/devices

for entry in "$SYS_DEVICE_DIR"/*/"$DEVICE"
do
  MAIN_DEV_PATH=${entry%:*}
  BUSNUM=`cat $MAIN_DEV_PATH/busnum` || exception "Unable to extract BUSNUM from device $DEVICE"
  DEVNUM=`cat $MAIN_DEV_PATH/devnum` || exception "Unable to extract DEVNUM from device $DEVICE"
  printf -v BUSNUM "%03d" $BUSNUM
  printf -v DEVNUM "%03d" $DEVNUM
  echo "$BUSNUM $DEVNUM"
done
