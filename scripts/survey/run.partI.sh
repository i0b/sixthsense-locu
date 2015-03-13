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
#----------------    PART I    ---------------------
#---------------------------------------------------

echo -e "\nPART I"
echo "Welcome to PART I of our survey. We will test  different"
echo "actuators on you.  On this first test,  you only need to"
echo "tell us,if and when you can feel an actuator and whether"
echo "it ever feels  unpleasent.  For that  the actuators will"
echo "increase the level of stimulation over time. In case you"
echo "have any questions, pease do  not  hesitate to ask them."

cd partI
startLog "partI"

echo -e "\nactuator: temperature hot"
./temperature.hot.sh "$PARTICIPANT_NUMBER"
echo "--------------------------------------------------------"

echo -e "\nactuator: vibration"
./vibration.sh "$PARTICIPANT_NUMBER"
echo "--------------------------------------------------------"

echo -e "\nactuator: electric"
./electric.sh "$PARTICIPANT_NUMBER"
echo "--------------------------------------------------------"

echo -e "\nactuator: temperature cold"
./temperature.cold.sh "$PARTICIPANT_NUMBER"
echo "--------------------------------------------------------"

echo -e "\nactuator: pressure"
./pressure.sh "$PARTICIPANT_NUMBER"
echo "--------------------------------------------------------"

stopLog
cd ..
echo -e "\n\nGreat, you finished PART I.\n\n"

