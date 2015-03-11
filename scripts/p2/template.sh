#!/bin/bash

PARTICIPANT_NUMBER="$1"
PORT="$2"

TTY="$(tty)"

PARAMETER="255"

INTENSITY1="0"
INTERVAL1="0"
INTENSITY2="0"
INTERVAL2="0"

STATISTIC_LOG="statistic."$PARTICIPANT_NUMBER".log"

function loadIntensity {
  declare -a 'values=('"$1"')'
  #eval values=($1)
  INTENSITY1=${values[0]}
  INTERVAL1=${values[1]}
  INTENSITY2=${values[2]}
  INTERVAL2=${values[3]}
}

function applyValues {
  echo "si $ACTUATOR $INTENSITY1 six/0.1" > $PORT
  sleep "$INTERVAL1"
  echo "si $ACTUATOR $INTENSITY2 six/0.1" > $PORT
  sleep "$INTERVAL2"
}

function runBatchFile {
  while read line; do
    loadIntensity "$line"

    applyValues

    if [ "$1" == "showValues" ]; then
      echo "Interval changed from" $INTENSITY1 "to" $INTENSITY2
    fi

    if [ "$1" == "test" ]; then
      echo "please enter the intensity levels you felt"
      read -p "first value: " firstValue  < $TTY
      read -p "second value: " secondValue < $TTY

      echo "actual  values:" $INTENSITY1 "to" $INTENSITY2 >> $STATISTIC_LOG
      echo "guessed values:" $firstValue "to" $secondValue >> $STATISTIC_LOG
    fi

    read -n1 -p "press a key to continue" start < $TTY
  done <$2
}

function runStatistic {
  echo "sm $ACTUATOR $ACTUATOR_MODE six/0.1" > $PORT
  echo "sp $ACTUATOR $PARAMETER six/0.1" > $PORT

  date >> $STATISTIC_LOG

  echo "calibration phase: you will feel different values of the actuator and be given the set number"
  read -n1 -p "press a key to begin" start
  for CALIBRATE_FLE in execution/$ACTYATOR_TYPE/calibrate*.txt; do
    runBatchFile "showValues" $CALIBRATE_FLE
  done

  echo "test phase: please first try to feel start and end value, then enter what value you have felt"
  read -n1 -p "press a key to begin" start
  for TEST_FLIE in execution/$ACTYATOR_TYPE/test*.txt; do
    runBatchFile "test" $TEST_FLIE
  done

  echo "sm $ACTUATOR OFF six/0.1" > $PORT
}
