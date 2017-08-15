#!/bin/bash

SYS_DEVICE_DIR=/sys/bus/usb/devices

rm /tmp/dev/*

for entry in "$SYS_DEVICE_DIR"/usb*
do
    HUBDEV=${entry##*/}
    echo $HUBDEV > /sys/bus/usb/drivers/usb/unbind
    echo $HUBDEV > /sys/bus/usb/drivers/usb/bind
done
