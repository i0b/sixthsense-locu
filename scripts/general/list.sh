#!/bin/bash

echo "calls list() every $UPDATEINTERVAL second(s)"

PORT="/dev/ttyACM0"
SPEED="57600"
UPDATEINTERVAL="1"

stty -F $PORT cs8 -cstopb -parenb $SPEED

while true; do
    echo "list six/0.1" > $PORT
    sleep $UPDATEINTERVAL
done
