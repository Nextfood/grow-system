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
	$DB_LOGGER gs${SYS} gb${BOX} "{\"action\": \"temperature\", \"value\": $TEMP}"
	$DB_LOGGER gs${SYS} gb${BOX} "{\"action\": \"humidity\", \"value\": $HUM}"
}

if [ -z "$SYS" ] || [ -z "$BOX" ]; then
	usage
fi

DEV_TEMP_HUM=/tmp/dev/gb${BOX}_temp_humitity 
LOGFILENAME=temperature_humidity.log
LOCK_FD_TEMP_HUM=$((270 + ${BOX}))
LOCK_FILE_TEMP_HUM=/tmp/temp_hum_gb${BOX}.lockfile

exec {LOCK_FD_TEMP_HUM}>$LOCK_FILE_TEMP_HUM || exit 1
flock -x "$LOCK_FD_TEMP_HUM" || { echo "ERROR: flock() failed." >&2; exit 1; }


VALSTR=`$MODULE_CLIENTS_DIR/getTempHum.py $DEV_TEMP_HUM` || exception "Unable to log temperature humidity for box $BOX."

TEMP=`echo $VALSTR | cut -d' ' -f1`
HUM=`echo $VALSTR | cut -d' ' -f4`

logToDb

echo $VALSTR
