#!/bin/bash

function startLog {
  PART=$1
  source config

  stty -F $PORT -brkint -icrnl -imaxbel -opost -isig -icanon -iexten -echo $SPEED

  for pid in $(ps u | grep "cat $PORT" | awk '{ print $2 }'); do
    kill $pid 2> /dev/null
  done

  cat $PORT >> $COMMAND_LOG&
  LOG_PID=$!
}

function stopLog {
  kill $LOG_PID
}

clear

echo "Welcome to the sixthsense survey"
echo "--------------------------------------------------------"
read -p "please enter your participant number: " PARTICIPANT_NUMBER
source config

#---------------------------------------------------
#----------------    PART II    --------------------
#---------------------------------------------------

echo "--------------------------------------------------------"
echo -e "PART II\n"
echo "--------------------------------------------------------"
echo "Now, we  want  to  test  how good you can  differentiate"
echo "different levels  of  stimulation. The actuators are all"
echo "going to be tested. Each test consists of two steps:"
echo "1) calibration: You will  feel  the actuator change from"
echo "                one   level  of  stimulation  to another"
echo "                while the  values  will be  presented on"
echo "                the screen at the same time.  Please try"
echo "                to remember the feeling of each level."
echo "2) test: You will feel the actuators' stimmulation level"
echo "         change from one to another,  but you  will  NOT"
echo "         see any values. Instead you need to type in the"
echo "         start and end value you think you felt."

echo -e "\nIf you have any questions, ask them right away."

echo "--------------------------------------------------------"

cd partII
startLog "partII"

echo -e "\nactuator: temperature hot"
./temperature.hot.sh "$PARTICIPANT_NUMBER"
echo "--------------------------------------------------------"

echo -e "\nactuator: vibration"
./vibration.sh "$PARTICIPANT_NUMBER"
echo "--------------------------------------------------------"

echo -e "\nactuator: electric"
read -p "what was your maximum electro stimulation value?" maxElectro
echo -e "okay, we will not go over this limit!\n"
./electric.sh "$PARTICIPANT_NUMBER" "$maxElectro"
echo "--------------------------------------------------------"

echo -e "\nactuator: temperature cold"
./temperature.cold.sh "$PARTICIPANT_NUMBER"
echo "--------------------------------------------------------"

echo -e "\nactuator: pressure"
./pressure.sh "$PARTICIPANT_NUMBER"
echo "--------------------------------------------------------"

stopLog
