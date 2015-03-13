#!/bin/bash

source template

ACTYATOR_TYPE="electric"
ACTUATOR_MODE="elec"
ACTUATOR="6"
SLEEP_TIME="0.3"

UP="0"
DOWN="1"
LEFT="2"
RIGHT="3"

function applyValues {
  echo "sm $ACTUATOR $ACTUATOR_MODE six/0.1" > $PORT

  intensity="0"

  if [ "$1" == "showValues" ]; then
    echo "Intensity: $INTENSITY1" > $TTY
  fi
              
  while [ $intensity -lt $INTENSITY1 ]; do
    echo "si $ACTUATOR $UP six/0.1" > $PORT
    sleep "$SLEEP_TIME"
    let intensity+=1
  done

  sleep "$INTERVAL1"

  difference=$(($INTENSITY2-$INTENSITY1))

  if [ "$1" == "showValues" ]; then
    echo "Intensity: $INTENSITY2" > $TTY
  fi
              
  if [ "$difference" -gt "0" ]; then
    while [ $intensity -lt $INTENSITY2 ]; do
      echo "si $ACTUATOR $UP six/0.1" > $PORT
      sleep "$SLEEP_TIME"
      let intensity+=1
    done
  else
    while [ $intensity -gt $INTENSITY2 ]; do
      echo "si $ACTUATOR $DOWN six/0.1" > $PORT
      sleep "$SLEEP_TIME"
      let intensity-=1
    done
  fi

  sleep "$INTERVAL2"

  while [ "$intensity" -gt "0" ]; do
    echo "si $ACTUATOR $DOWN six/0.1" > $PORT
    sleep "$SLEEP_TIME"
    let intensity-=1
  done

  echo "sm $ACTUATOR OFF six/0.1" > $PORT
}

runStatistic
