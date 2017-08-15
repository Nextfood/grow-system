#!/bin/bash

THISDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
MODULE_CLIENTS_DIR=$THISDIR/../module-clients
DB_LOGGER=$THISDIR/../../nextfood-metrics/db-connection/log_to_mongodb.py
SYS=$1
DRAIN_TIME=$2


function exception {
  $DB_LOGGER gs${SYS} errors "{\"error_msg\": \"$1\"}"
  exit 1
}
function usage {
	SCRIPTNAME=$( basename ${0} )
	echo "Usage: ${SCRIPTNAME} <system number> <drain time>"
	exit 1
}

function logToDb {
	$DB_LOGGER gs${SYS} "system" "{\"action\": \"draining\", \"value\": $DRAIN_TIME}"
}





if [ -z "$SYS" ] || [ -z "$DRAIN_TIME" ]; then
	usage
fi


DEV_DRAIN_PUMPS=/tmp/dev/gs${SYS}_drain_pumps
LOCK_FD_DRAIN_PUMPS=$((260))
LOCK_FILE_DRAIN_PUMPS=/tmp/DRAIN_PUMPS_gs${SYS}.lockfile


exec {LOCK_FD_DRAIN_PUMPS}>$LOCK_FILE_DRAIN_PUMPS || exit 1
flock -x "$LOCK_FD_DRAIN_PUMPS" || { echo "ERROR: flock() failed." >&2; exit 1; }


$MODULE_CLIENTS_DIR/setPwm.py $DEV_DRAIN_PUMPS $DRAIN_TIME "100,100,100,100,100,100" || exception "Unable to run drain pump or Box $BOX."

logToDb
