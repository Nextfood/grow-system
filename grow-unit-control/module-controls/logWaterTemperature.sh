#!/bin/bash

THISDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
MODULE_CLIENTS_DIR=$THISDIR/../module-clients
DB_LOGGER=$THISDIR/../../nextfood-metrics/db-connection/log_to_mongodb.py
SYS=$1

function exception { 
  $DB_LOGGER gs${SYS} errors "{\"error_msg\": \"$1\"}"
  exit 1
}

function usage {
	SCRIPTNAME=$( basename ${0} )
	echo "Usage: ${SCRIPTNAME} <grow system number>"
	exit 1
}

function logToDb {
	$DB_LOGGER gs${SYS} system "{\"action\": \"water_temperature\", \"value\": $TEMP}"
	$DB_LOGGER gs${SYS} system "{\"action\": \"ambient_temperature\", \"value\": $INT_TEMP}"
}

if [ -z "$SYS" ]; then
	usage
fi

DEV_TEMP=/tmp/dev/gs${SYS}_water_temp
LOGFILENAME=temperature_water.log
LOCK_FD_TEMP_HUM=$((221 + ${SYS}))
LOCK_FILE_TEMP_HUM=/tmp/gs${SYS}_temp_water.lockfile

exec {LOCK_FD_TEMP_HUM}>$LOCK_FILE_TEMP_HUM || exit 1
flock -x "$LOCK_FD_TEMP_HUM" || { echo "ERROR: flock() failed." >&2; exit 1; }


VALSTR=`$MODULE_CLIENTS_DIR/getTemp.py $DEV_TEMP` || exception "Unable to log water temperature for SYS $SYS."

TEMP=`echo $VALSTR | cut -d' ' -f1`
INT_TEMP=`echo $VALSTR | cut -d' ' -f5`

logToDb

echo $VALSTR
