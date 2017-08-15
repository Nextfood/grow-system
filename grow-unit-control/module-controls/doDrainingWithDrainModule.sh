#!/bin/bash

THISDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
MODULE_CLIENTS_DIR=$THISDIR/../module-clients
DB_LOGGER=$THISDIR/../../nextfood-metrics/db-connection/log_to_mongodb.py
SYS=$1
BOX=$2
DRAIN_TIME=$3


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


DEV_DRAIN_PUMPS=/tmp/dev/gs${SYS}_drain_pumps
LOCK_FD_DRAIN_PUMPS=$((260 + ${BOX}))
LOCK_FILE_DRAIN_PUMPS=/tmp/DRAIN_PUMPS_gb${BOX}.lockfile


exec {LOCK_FD_DRAIN_PUMPS}>$LOCK_FILE_DRAIN_PUMPS || exit 1
flock -x "$LOCK_FD_DRAIN_PUMPS" || { echo "ERROR: flock() failed." >&2; exit 1; }

PWM_CH="0,0,0,0,0,0"
if [ "$BOX" -eq "1" ]; then
	PWM_CH="100,0,0,0,0,0"
elif [ "$BOX" -eq "2" ]; then
	PWM_CH="0,100,0,0,0,0"
elif [ "$BOX" -eq "3" ]; then
	PWM_CH="0,0,100,0,0,0"
elif [ "$BOX" -eq "4" ]; then
	PWM_CH="0,0,0,100,0,0"
elif [ "$BOX" -eq "5" ]; then
	PWM_CH="0,0,0,0,100,0"
elif [ "$BOX" -eq "6" ]; then
	PWM_CH="0,0,0,0,0,100"
fi


$MODULE_CLIENTS_DIR/setPwm.py $DEV_DRAIN_PUMPS $DRAIN_TIME $PWM_CH || exception "Unable to run drain pump or Box $BOX."

logToDb
