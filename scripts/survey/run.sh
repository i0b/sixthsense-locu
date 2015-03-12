#!/bin/bash

clear

echo "Welcome to the sixthsense survey"
echo "--------------------------------------------------------"
read -p "please enter your participant number: " PARTICIPANT_NUMBER
source config

#stty -F $PORT cs8 -cstopb -parenb $SPEED
#stty -F $PORT cs8 -cstopb -parenb cread clocal $SPEED
stty -F $PORT cs8 cread clocal $SPEED

#---------------------------------------------------
#----------------    PART I    ---------------------
#---------------------------------------------------

echo -e "\nPART I"
echo "--------------------------------------------------------"
echo "In the following you will feel actuators. They will rise"
echo "in intensity.  Please press any key as soon as you start"
echo "to feel the actuator or  as  soon  as  you  consider the" 
echo "feeling of it as unpleasant in everyday, continuous use."
echo "--------------------------------------------------------"

cd partI

#picocom -b $SPEED $PORT >> command."$PARTICIPANT_NUMBER".log&
cat $PORT >> $COMMAND_LOG&
LOG_PID=$!

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

kill $LOG_PID

cd ..

#---------------------------------------------------
#----------------    PART II    --------------------
#---------------------------------------------------

echo -e "PART II\n"
cd partII

cat $PORT >> $COMMAND_LOG&
LOG_PID=$!

echo "running temperature hot"
./temperature.hot.sh "$PARTICIPANT_NUMBER"

echo "running vibration"
./vibration.sh "$PARTICIPANT_NUMBER"

echo "running temperature cold"
./temperature.cold.sh "$PARTICIPANT_NUMBER"

echo "running pressure"
./pressure.sh "$PARTICIPANT_NUMBER"

echo "running electric"
./electric.sh "$PARTICIPANT_NUMBER"

kill $LOG_PID
