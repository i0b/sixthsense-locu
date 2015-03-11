#!/bin/bash

source template.sh

ACTYATOR_TYPE="electric"
ACTUATOR_MODE="elec"
ACTUATOR="6"
ACTUATOR_MAX="25"

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
  while [ "$readChar" -ne "0" ] && [ "$INTENSITY" -lt "$ACTUATOR_MAX" ]; do
    intensityStep
    echo "si $ACTUATOR $UP six/0.1" > $PORT
    read -n1 -t $TIMEOUT -p "$INTENSITY >> " enough
    readChar=$?
    # push down $INTENSITY - times to turn it off
    #echo "si $ACTUATOR $DOWN six/0.1" > $PORT
    #sleep $TIMEOUT
  done

}

echo "sm $ACTUATOR $ACTUATOR_MODE six/0.1" > $PORT
echo "si $ACTUATOR $UP six/0.1" > $PORT
echo "si $ACTUATOR $RIGHT six/0.1" > $PORT
sleep 2

runStatistic
