#!/bin/bash

THISDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
MODULE_CLIENTS_DIR=$THISDIR/../module-clients
DB_LOGGER=$THISDIR/../../nextfood-metrics/db-connection/log_to_mongodb.py
SYS="$1"
BOX="$2"

function exception { 
  $DB_LOGGER gs${SYS} errors "{\"error_msg\": \"$1\"}"
  exit 1
}



function usage {
	SCRIPTNAME=$( basename ${0} )
	echo "Usage: ${SCRIPTNAME} <grow system number> <box number>"
	exit 1
}

function logToDb {
	$DB_LOGGER gs${SYS} gb${BOX} "{\"action\": \"root_temperature\", \"value\": $TEMP_ROOT}"
	$DB_LOGGER gs${SYS} gb${BOX} "{\"action\": \"cannopy_temperature\", \"value\": $TEMP_CAN}"
}

if [ -z "$SYS" ] || [ -z "$BOX" ]; then
	usage
fi

DEV_ROOT_TEMP=/tmp/dev/gb${BOX}_root_chamber_temp 
LOGFILENAME=root_temperature.log
LOCK_FD_ROOT_TEMP=$((290 + ${BOX}))
LOCK_FILE_ROOT_TEMP=/tmp/root_temp_gb${BOX}.lockfile

exec {LOCK_FD_ROOT_TEMP}>$LOCK_FILE_ROOT_TEMP || exit 1
flock -x "$LOCK_FD_ROOT_TEMP" || { echo "ERROR: flock() failed." >&2; exit 1; }


VALSTR=`$MODULE_CLIENTS_DIR/getTemp.py $DEV_ROOT_TEMP` || exception "Unable to log root temperature for box $BOX."

TEMP_ROOT=`echo $VALSTR | cut -d' ' -f1`
TEMP_CAN=`echo $VALSTR | cut -d' ' -f5`

logToDb

echo $VALSTR
