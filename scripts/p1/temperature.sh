#!/bin/bash

source template.sh

function intensityStep {
  let "INTENSITY<<=1"
  let "INTENSITY+=1"
}

function logStatistic {
  echo "$ACTYATOR_TYPE $1 $INTENSITY" >> $STATISTIC_LOG
  echo "LIST six/0.1" > $PORT
  sleep $LIST_TIMEOUT
  grep "peltier value" $COMMAND_LOG >> $STATISTIC_LOG

}
