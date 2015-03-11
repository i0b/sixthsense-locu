#!/bin/bash

echo "use vim keys to press buttons on the electro module"

PORT="/dev/ttyACM0"
SPEED="57600"
CHANNEL="6"

stty -F $PORT cs8 -cstopb -parenb $SPEED

UP="0"
DOWN="1"
LEFT="2"
RIGHT="3"

echo "sm $CHANNEL elec six/0.1" > $PORT

while read -sN1 key; do
  case "$key" in
    'k') echo "si $CHANNEL $UP six/0.1" > $PORT;;
    'j') echo "si $CHANNEL $DOWN six/0.1" > $PORT;;
    'h') echo "si $CHANNEL $LEFT six/0.1" > $PORT;;
    'l') echo "si $CHANNEL $RIGHT six/0.1" > $PORT;;
  esac
done
