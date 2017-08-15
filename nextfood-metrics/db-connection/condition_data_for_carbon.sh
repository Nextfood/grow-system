#!/bin/bash

while IFS= read -r ARG; do



	DATE=`echo $ARG | cut -d ' ' -f1`
	DATE_EPOCH=`date -d$DATE '+%s'`
	GROW_SYSTEM=`echo $ARG | cut -d ' ' -f2 | tr -d '()' | cut -d '/' -f1`
	GROW_BOX=`echo $ARG | cut -d ' ' -f2 | tr -d '()' | cut -d '/' -f2`
	MODULE=`echo $ARG | cut -d ' ' -f3 | tr -d '[]'`
	VALUE=`echo $ARG | cut -d ' ' -f4`

	if [ -z "$VALUE" ]; then
		VALUE=1
	fi

	echo "$GROW_SYSTEM.$GROW_BOX.$MODULE $VALUE $DATE_EPOCH" 

done