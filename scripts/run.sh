#!/bin/bash

PORT="/dev/ttyACM0"
SPEED="57600"


read -p "participant no: " PARTICIPANT_NUMBER

stty -F $PORT cs8 -cstopb -parenb $SPEED

#---------------------------------------------------
#----------------    PART I    ---------------------
#---------------------------------------------------

echo -e "PART I\n"
cd p1

cat $PORT >> command."$PARTICIPANT_NUMBER".log&
LOG_PID=$!
echo "running temperature hot"
./temperature.hot.sh "$PARTICIPANT_NUMBER" "$PORT"

echo "running vibration"
./vibration.sh "$PARTICIPANT_NUMBER" "$PORT"

echo "running temperature cold"
./temperature.cold.sh "$PARTICIPANT_NUMBER" "$PORT"

echo "running pressure"
./pressure.sh "$PARTICIPANT_NUMBER" "$PORT"

echo "running electric"
./electric.sh "$PARTICIPANT_NUMBER" "$PORT"

kill $LOG_PID


cd ..

#---------------------------------------------------
#----------------    PART II    --------------------
#---------------------------------------------------

echo -e "PART II\n"
cd p2
cat $PORT >> command."$PARTICIPANT_NUMBER".log&
LOG_PID=$!

echo "running vibration"
./vibration.sh "$PARTICIPANT_NUMBER" "$PORT"

echo "running pressure"
./pressure.sh "$PARTICIPANT_NUMBER" "$PORT"

kill $LOG_PID
