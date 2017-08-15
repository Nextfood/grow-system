#!/bin/bash

THISDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
MODULE_CLIENTS_DIR=$THISDIR/../module-clients
DB_LOGGER=$THISDIR/../../nextfood-metrics/db-connection/log_to_mongodb.py
PRESSURE_MARK=0.90
PRESSURE_WINDOW_SIZE=0.12
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
	$DB_LOGGER gs${SYS} system "{\"action\": \"regulate_water\", \"value\": $RUNTIME}"
}



if [ -z "$SYS" ]; then
	usage
fi


LOGFILENAME=water_pressure_regulator.log
DEV_WATER_PUMP=/tmp/dev/gs${SYS}_water_pump
DEV_PRESSURE_SENSOR=/tmp/dev/gs${SYS}_pressure_sensor
LOCK_FD_WATER_PUMP=$((201 + ${SYS}))
LOCK_FD_PRESSURE_SENSOR=$((211 + ${SYS}))
LOCK_FILE_WATER_PUMP=/tmp/gs${SYS}_water_pump.lockfile
LOCK_FILE_PRESSURE_SENSOR=/tmp/gs${SYS}_water_pressure.lockfile

exec {LOCK_FD_WATER_PUMP}>$LOCK_FILE_WATER_PUMP || exit 1
exec {LOCK_FD_PRESSURE_SENSOR}>$LOCK_FILE_PRESSURE_SENSOR || exit 1
flock -x "$LOCK_FD_WATER_PUMP" || { echo "ERROR: flock() failed." >&2; exit 1; }
flock -x "$LOCK_FD_PRESSURE_SENSOR" || { echo "ERROR: flock() failed." >&2; exit 1; }


VALSTR=`$MODULE_CLIENTS_DIR/waterToPressureMark.py $DEV_WATER_PUMP $DEV_PRESSURE_SENSOR $PRESSURE_MARK $PRESSURE_WINDOW_SIZE` \
|| exception "Unable to run water pump for Grow System $SYS."

RUNTIME=`echo $VALSTR | cut -d' ' -f4`
PRESSURE=`echo $VALSTR | cut -d' ' -f9`

if (( $(echo "$RUNTIME > 0.0" |bc -l) )); then
  logToDb
fi

echo $VALSTR
