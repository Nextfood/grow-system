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

mkdir -p $DIR

NOW=`date +%Y%m%d%H%M`
raspistill -o $DIR/${PREFIX}_${NOW}.jpg
