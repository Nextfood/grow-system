#!/bin/bash

THISDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
MODULE_CLIENTS_DIR=$THISDIR/../module-clients
DB_LOGGER=$THISDIR/../../nextfood-metrics/db-connection/log_to_mongodb.py
SYS=$1
BOX=$2
COLOR=$3


function exception {
  $DB_LOGGER gs${SYS} errors "{\"error_msg\": \"$1\"}"
  exit 1
}

function usage {
	SCRIPTNAME=$( basename ${0} )
	echo "Usage: ${SCRIPTNAME} <system number> <box number> <GROW|OFF>"
	exit 1
}

function logToDb {
	$DB_LOGGER gs${SYS} gb${BOX} "{\"action\": \"lighting\", \"value\": $STATE_DB}"
}


if [ -z "$SYS" ] || [ -z "$BOX" ] || [ -z "$COLOR" ]; then
	usage
fi

DEV_LED_LAMP=/tmp/dev/gb${BOX}_led_lamp
LOGFILENAME=lights_gb${BOX}.log
LOCK_FD_LED_LAMP=$((280 + ${BOX}))
LOCK_FILE_LED_LAMP=/tmp/led_lamp_gb${BOX}.lockfile

STATE="OFF"
STATE_DB=0

if [ "${COLOR,,}" = "grow" ]; then
	STATE="ON"
	STATE_DB=1
elif [ "${COLOR,,}" = "off" ]; then
        STATE="OFF"
        STATE_DB=0
fi	

exec {LOCK_FD_LED_LAMP}>$LOCK_FILE_LED_LAMP || exit 1
flock -x "$LOCK_FD_LED_LAMP" || { echo "ERROR: flock() failed." >&2; exit 1; }

$MODULE_CLIENTS_DIR/setRelay.py $DEV_LED_LAMP $STATE || exception "Unable to run water valves for Box $BOX."

logToDb
