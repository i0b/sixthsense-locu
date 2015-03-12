#!/bin/bash

if [ $# -ne 2 ]; then
  echo "usage $0 [INTENSITY - 0-100] [ON/OFF - 0-255"
  exit 1
fi


PORT="/dev/ttyACM0"
SPEED="57600"

stty -F $PORT cs8 -cstopb -parenb $SPEED

echo "sm 1 vib six/0.1" > $PORT
echo "si 1 $1 six/0.1"  > $PORT
echo "sp 1 $2 six/0.1" > $PORT

#cat < $PORT