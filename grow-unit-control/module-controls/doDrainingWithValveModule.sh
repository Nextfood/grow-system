#!/bin/bash

THISDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
MODULE_CLIENTS_DIR=$THISDIR/../module-clients
DB_LOGGER=$THISDIR/../../nextfood-metrics/db-connection/log_to_mongodb.py
SYS=$1
BOX=$2
DRAIN_TIME=$3

PWM_CH="0,0,0,100,0,0"


function exception {
  $DB_LOGGER gs${SYS} errors "{\"error_msg\": \"$1\"}"
  exit 1
}
function usage {
	SCRIPTNAME=$( basename ${0} )
	echo "Usage: ${SCRIPTNAME} <system number> <box number> <drain time>"
	exit 1
}

function logToDb {
	$DB_LOGGER gs${SYS} gb${BOX} "{\"action\": \"draining\", \"value\": $DRAIN_TIME}"
}





if [ -z "$SYS" ] || [ -z "$BOX" ] || [ -z "$DRAIN_TIME" ]; then
	usage
fi


DEV_VALVES=/tmp/dev/gb${BOX}_water_valves
LOCK_FD_WATER_VALVES=$((250 + ${BOX}))
LOCK_FILE_WATER_VALVES=/tmp/water_valves_gb${BOX}.lockfile


exec {LOCK_FD_WATER_VALVES}>$LOCK_FILE_WATER_VALVES || exit 1
flock -x "$LOCK_FD_WATER_VALVES" || { echo "ERROR: flock() failed." >&2; exit 1; }


$MODULE_CLIENTS_DIR/setPwm.py $DEV_VALVES $DRAIN_TIME $PWM_CH || exception "Unable to run drain pump or Box $BOX."

logToDb
