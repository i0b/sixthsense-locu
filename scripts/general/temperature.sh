#!/bin/bash

if [ $# -ne 2 ]; then
  echo "usage $0 [ON/OFF - 0-15] [HOT/COLD - 0-15]"
  exit 1
fi


PORT="/dev/ttyACM0"
SPEED="57600"

stty -F $PORT cs8 -cstopb -parenb $SPEED

echo "sm 3 temp six/0.1" > $PORT
echo "si 3 $1 six/0.1"  > $PORT
echo "sp 3 $2 six/0.1" > $PORT

#cat < $PORT
