#!/bin/bash

PARTICIPANT_NUMBER="$1"
PORT="$2"

TIMEOUT="3"

INTENSITY="0"

PARAMETER="255"

COMMAND_LOG="command."$PARTICIPANT_NUMBER".log"
STATISTIC_LOG="statistic."$PARTICIPANT_NUMBER".log"


function intensityStep {
  let "INTENSITY+=10"
}

function intensityKeypress {
  enough="no"
  readChar="1"
  while [ "$readChar" -ne "0" ] && [ "$INTENSITY" -lt "$ACTUATOR_MAX" ]; do
    intensityStep

    echo "si $ACTUATOR $INTENSITY six/0.1" > $PORT
    #read -n1 -t $TIMEOUT -p "$INTENSITY >> " enough
    read -n1 -t $TIMEOUT enough
    readChar=$?

    echo "si $ACTUATOR 0 six/0.1" > $PORT

    if [ "$readChar" -eq "0" ]; then
      break
    fi

    sleep $TIMEOUT
  done
}

function logStatistic {
  echo "$ACTYATOR_TYPE $1 $INTENSITY" >> $STATISTIC_LOG
}

#function logCommands {
#}

function runStatistic {
  #logCommands

  echo "sm $ACTUATOR $ACTUATOR_MODE six/0.1" > $PORT
  echo "sp $ACTUATOR $PARAMETER six/0.1" > $PORT

  date >> $STATISTIC_LOG
  read -n1 -p "press a key if actuator value was noticable" start

  intensityKeypress
  logStatistic "min:"

  read -n1 -p "press a key if actuator value was unpleasant" cont

  intensityKeypress
  logStatistic "max:"

  echo "sm $ACTUATOR OFF six/0.1" > $PORT
}
