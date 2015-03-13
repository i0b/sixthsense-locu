#!/bin/bash

source template

ACTYATOR_TYPE="electric"
ACTUATOR_MODE="elec"
ACTUATOR="6"
ACTUATOR_MAX="25"
SLEEP_TIME="0.3"

UP="0"
DOWN="1"
LEFT="2"
RIGHT="3"

function intensityStep {
  let "INTENSITY+=1"
}

function intensityKeypress {
  enough="no"
  readChar="1"
  while [ "$INTENSITY" -lt "$ACTUATOR_MAX" ]; do
    intensityStep

    for i in $(seq 1 $INTENSITY); do
      echo "si $ACTUATOR $UP six/0.1" > $PORT
      sleep $SLEEP_TIME
    done

    #read -n1 -t $TIMEOUT -p "$INTENSITY >> " enough
    read -n1 -t $TIMEOUT enough
    readChar=$?

    for i in $(seq 1 $INTENSITY); do
      echo "si $ACTUATOR $DOWN six/0.1" > $PORT
      sleep $SLEEP_TIME
    done

    if [ "$readChar" -eq "0" ]; then
      break
    fi

    sleep $TIMEOUT
  done

}

#init the actuator - set to right value blinking cursor, ready to press $UP
echo "sm $ACTUATOR $ACTUATOR_MODE six/0.1" > $PORT
echo "si $ACTUATOR $UP six/0.1" > $PORT
sleep $SLEEP_TIME
echo "si $ACTUATOR $RIGHT six/0.1" > $PORT
sleep $SLEEP_TIME

runStatistic
