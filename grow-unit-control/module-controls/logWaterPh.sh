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
	$DB_LOGGER gs${SYS} system "{\"action\": \"water_ph\", \"value\": $PH_VALUE}"
}

if [ -z "$SYS" ]; then
	usage
fi

DEV_TEMP=/tmp/dev/gs${SYS}_water_ph_sensor
LOCK_FD_WATER_PH=$((231 + ${SYS}))
LOCK_FILE_WATER_PH=/tmp/gs${SYS}_water_ph.lockfile

exec {LOCK_FD_WATER_PH}>$LOCK_FILE_WATER_PH || exit 1
flock -x "$LOCK_FD_WATER_PH" || { echo "ERROR: flock() failed." >&2; exit 1; }


PH_VALUE=`$MODULE_CLIENTS_DIR/getWaterPh.py $DEV_TEMP` || exception "Unable to log water PH level for SYS $SYS."

logToDb

echo $PH_VALUE
