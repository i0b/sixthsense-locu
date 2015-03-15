#!/bin/bash

source template

ACTUATOR_TYPE="electric"
ACTUATOR_MODE="elec"
ACTUATOR="6"
SLEEP_TIME="0.3"
MAX_STIMULATION=$2

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

function runBatchFile {
  while read -u 7 line; do
    loadIntensity "$line"

    readSequenceNumber

    if [ -z $START_SEQUENCE_NUMBER ] || [ "$START_SEQUENCE_NUMBER" -le "$SEQUENCE_NUMBER" ]; then
      applyValues $1

      echo $line | grep "[0-$MAX_STIMULATION] .* [0-$MAX_STIMULATION] .*" &> /dev/null

      if [ "$?" -eq "0" ]; then
        applyValues $1

        if [ "$1" == "test" ]; then
          echo "please enter the intensity levels you felt"
          read -p "first value: " firstValue  < $TTY
          read -p "second value: " secondValue < $TTY

          echo "$#SEQUENCE_NUMBER correct:" $INTENSITY1 "to" $INTENSITY2 >> $STATISTIC_LOG
          echo -e "#SEQUENCE_NUMBER guessed:" $firstValue "to" $secondValue "\n" >> $STATISTIC_LOG
        fi

        read -n1 -p "press a key to continue" start < $TTY
      fi
    fi

    incSequenceNumber

  done 7<$2
}

#init the actuator - set to right value blinking cursor, ready to press $UP
echo "sm $ACTUATOR $ACTUATOR_MODE six/0.1" > $PORT
echo "si $ACTUATOR $LEFT six/0.1" > $PORT
sleep $SLEEP_TIME
echo "si $ACTUATOR $UP six/0.1" > $PORT
sleep $SLEEP_TIME
echo "si $ACTUATOR $RIGHT six/0.1" > $PORT
sleep $SLEEP_TIME

runStatistic
