#!/bin/bash

FILENAME=$1

DATENOW=`date --rfc-3339=seconds`

echo "$DATENOW: $2 $3 $4 $5 $6" >> /home/nextfood/$FILENAME
