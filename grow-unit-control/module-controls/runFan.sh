#!/bin/bash

THISDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
MODULE_CLIENTS_DIR=$THISDIR/../module-clients

function usage {
	SCRIPTNAME=$( basename ${0} )
	echo "Usage: ${SCRIPTNAME} <box number> <run time>"
	exit 1
}

function exception {
  echo "$1" | $THISDIR/logToFile.sh error.log
  exit 1
}


BOX=$1
RUN_TIME=$2

if [ -z "$BOX" ] || [ -z "$RUN_TIME" ]; then
	usage
fi


DEV_VALVES=/tmp/dev/gb${BOX}_water_valves
LOGFILENAME=air_fan.log
LOCK_FD_WATER_VALVES=$((220 + ${BOX}))
LOCK_FILE_WATER_VALVES=/tmp/water_valves_gb${BOX}.lockfile

exec {LOCK_FD_WATER_VALVES}>$LOCK_FILE_WATER_VALVES || exit 1
flock -x "$LOCK_FD_WATER_VALVES" || { echo "ERROR: flock() failed." >&2; exit 1; }

$MODULE_CLIENTS_DIR/setFan.py $DEV_VALVES 100 $RUN_TIME || exception "Unable to run fan pump or Box 3."
echo "Fan started on box $BOX. Running for: $RUN_TIME s" | $THISDIR/logToFile.sh $LOGFILENAME
