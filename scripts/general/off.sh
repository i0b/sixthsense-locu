#!/bin/bash

PORT="/dev/ttyACM0"
SPEED="57600"

stty -F $PORT cs8 -cstopb -parenb $SPEED

echo "sm 0 off six/0.1" > $PORT
echo "sm 1 off six/0.1" > $PORT
echo "sm 2 off six/0.1" > $PORT
echo "sm 3 off six/0.1" > $PORT
echo "sm 4 off six/0.1" > $PORT
echo "sm 5 off six/0.1" > $PORT
echo "sm 6 off six/0.1" > $PORT

#cat < $PORT
