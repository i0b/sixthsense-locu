#!/bin/bash

source template.sh

ACTYATOR_TYPE="electric"
ACTUATOR_MODE="elec"
ACTUATOR="6"
SLEEP_TIME="0.3"

UP="0"
DOWN="1"
LEFT="2"
RIGHT="3"

function applyValues {
  intensity="0"

  while [ $intensity -lt $INTENSITY1 ]; do
    echo "si $ACTUATOR $UP six/0.1" > $PORT
    sleep "$SLEEP_TIME"
    let intensity+=1
  done

  sleep "$INTERVAL1"

  difference=$(($INTENSITY2-$INTENSITY1))

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
}

runStatistic
