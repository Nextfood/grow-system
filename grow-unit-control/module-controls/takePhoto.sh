#!/bin/bash

DEVICE="$1"
PREFIX="$2"
ROTATE="$3"
DIR="$4"

function usage {
	SCRIPTNAME=$( basename ${0} )
	echo "Usage: ${SCRIPTNAME} <device> <file prefix> <rotation degrees> <file dir>"
	exit 1
}

function exception {
  echo "$1" | $THISDIR/logToFile.sh error.log
  exit 1
}


if [ -z "$DEVICE" ] || [ -z "$PREFIX" ] || [ -z "$ROTATE" ] || [ -z "$DIR" ]; then
	usage
fi

NOW=`date +%Y%m%d%H%M`
FSWEBCAM_OPT="--banner-colour #80000000 --line-colour #FF000000 --shadow  --resolution 640x480 --frames 1 --jpeg -1 --rotate $ROTATE"
lsusb | grep GEMBIRD | tr -d : | awk '{print $2, $4;}' | sudo xargs -L1 /usr/local/bin/resetusb
sudo chmod a+rwx /dev/video*
fswebcam -d $DEVICE $FSWEBCAM_OPT $DIR/${PREFIX}_${NOW}.jpg
