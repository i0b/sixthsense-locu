#!/bin/bash

if [ $# -lt 1 ]; then
  echo "usage $0 ORDER { continue | PARTII-START-QUESTION }"
  exit 1
fi

clear

echo "Welcome to the sixthsense survey"
echo "--------------------------------------------------------"
read -p "please enter your participant number: " PARTICIPANT_NUMBER

source config

#---------------------------------------------------
#----------------    PART I    ---------------------
#---------------------------------------------------

if [ -z "$2" ]; then
  echo -e "\nPART I"
  echo "Welcome to PART I of our survey. We will test  different"
  echo "actuators on you.  On this first test,  you only need to"
  echo "tell us,if and when you can feel an actuator and whether"
  echo "it ever feels  unpleasent.  For that  the actuators will"
  echo "increase the level of stimulation over time. In case you"
  echo "have any questions, pease do  not  hesitate to ask them."

  cd partI
  startLog "partI"

  while read -u 5 actuator; do
    executeScript "$actuator"
  done 5<"./order.set/$1"

  cd ..
  echo -e "\n\nGreat, you finished PART I.\n\n"
fi

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

echo -e "--------------------------------------------------------\n"

read -p "what was your maximum electro stimulation value? " maxElectro
echo -e "okay, we will not go over this limit!\n"

echo "--------------------------------------------------------"

cd partII
startLog "partII"


if [ -n "$2" ]; then
  if [ -e $SEQUENCE_FILE ] && [ "$2" == "continue" ]; then
    startValue="$(cat $SEQUENCE_FILE)"
  else
    startValue="$2"
  fi
fi

rm -f $SEQUENCE_FILE

while read -u 5 actuator; do
  executeScript "$actuator" "$maxElectro" "$startValue"
done 5<"./order.set/$1"

echo "--------------------------------------------------------"

echo -e "\nThank you very much for participating in our study!\n"

stopLog
