#/bin/bash

THISDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
MODULE_CLIENTS_DIR=$THISDIR/../module-clients


if [ -z "$NEXTFOOD_LOG_DIR" ]; then
    LOGDIR=~
else
    LOGDIR=$NEXTFOOD_LOG_DIR
fi

LOGFILE=$1

mkdir -p ${LOGDIR}

DATENOW=`date --rfc-3339=seconds`

xargs -I {} echo $DATENOW: {} >> ${LOGDIR}/${LOGFILE}
