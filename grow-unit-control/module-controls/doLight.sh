#!/bin/bash

THISDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
MODULE_CLIENTS_DIR=$THISDIR/../module-clients

function exception { 
  echo "$1" | $THISDIR/logToFile.sh error.log
  exit 1
}

function usage {
	SCRIPTNAME=$( basename ${0} )
	echo "Usage: ${SCRIPTNAME} <device> <color: white|grow|off>"
	exit 1
}

BOX=$1
COLOR=$2

if [ -z "$BOX" ] || [ -z "$COLOR" ]; then
	usage
fi

DEV_LED_DRIVER=/tmp/dev/gb${BOX}_led_driver
LOGFILENAME=lights_gb${BOX}.log
LOCK_FD_LED_DRIVER=$((230 + ${BOX}))
LOCK_FILE_LED_DRIVER=/tmp/LED_DRIVER_gb${BOX}.lockfile

PWM_1=0
PWM_2=0
PWM_3=0

if [ "${COLOR,,}" = "white" ]; then
	PWM_1=100
	PWM_2=0
	PWM_3=0
elif [ "${COLOR,,}" = "grow" ]; then
	PWM_1=100
	PWM_2=100
	PWM_3=100
elif [ "${COLOR,,}" = "off" ]; then
	PWM_1=0
	PWM_2=0
	PWM_3=0
fi	

exec {LOCK_FD_LED_DRIVER}>$LOCK_FILE_LED_DRIVER || exit 1
flock -x "$LOCK_FD_LED_DRIVER" || { echo "ERROR: flock() failed." >&2; exit 1; }

$MODULE_CLIENTS_DIR/setLights.py $DEV_LED_DRIVER $PWM_1 $PWM_2 $PWM_3  || exception "Unable to run lights grow."
echo "White: ${PWM_1}% | Red: ${PWM_2}% | Blue: ${PWM_3}%" | $THISDIR/logToFile.sh $LOGFILENAME
