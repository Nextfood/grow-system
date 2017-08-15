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
	$DB_LOGGER gs${SYS} system "{\"action\": \"water_pressure\", \"value\": $VAL}"
}


if [ -z "$SYS" ]; then
	usage
fi


DEV_PRESSURE_SENSOR=/tmp/dev/gs${SYS}_pressure_sensor
LOGFILENAME=water_pressure.log
LOCK_FD_PRESSURE_SENSOR=$((211 + ${SYS}))
LOCK_FILE_PRESSURE_SENSOR=/tmp/gs${SYS}_water_pressure.lockfile

exec {LOCK_FD_PRESSURE_SENSOR}>$LOCK_FILE_PRESSURE_SENSOR || exit 1
flock -x "$LOCK_FD_PRESSURE_SENSOR" || { echo "ERROR: flock() failed." >&2; exit 1; }


VALSTR=`$MODULE_CLIENTS_DIR/getPressure.py $DEV_PRESSURE_SENSOR` \
|| exception "Unable to measure water pressure."

VAL=`echo $VALSTR | cut -d' ' -f1`

logToDb

echo $VAL
