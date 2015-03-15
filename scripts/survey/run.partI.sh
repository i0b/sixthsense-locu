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

echo "Herzlich willkommen bei der sixthsense Studie!"
echo "--------------------------------------------------------"
read -p "Bitte geben Sie Ihre Probanden-Nummer ein: " PARTICIPANT_NUMBER
source config

#---------------------------------------------------
#----------------    PART I    ---------------------
#---------------------------------------------------

echo -e "\nTEIL 1"
echo "Willkommen beim TEIL 1 unserer Studie. Wir werden verschiedene"
echo "Akuatoren mit Ihnen testen. In diesem ersten Test muessen Sie uns"
echo "lediglich mitteilen, ob und wann Sie etwas fuehlen und ob es sich"
echo "unangenehm anfuehlt.  Wir werden dazu das Intensitaets-Level der"
echo "Aktuatoren nach und nach erhoehen."

cd partI
startLog "partI"

echo -e "\nAktuator: Temperatur (warm)"
./temperature.hot.sh "$PARTICIPANT_NUMBER"
echo "--------------------------------------------------------"

echo -e "\nAktuator: Vibration"
./vibration.sh "$PARTICIPANT_NUMBER"
echo "--------------------------------------------------------"

echo -e "\nAktuator: Elektrisch"
./electric.sh "$PARTICIPANT_NUMBER"
echo "--------------------------------------------------------"

echo -e "\nAktuator: Temperatur (kalt)"
./temperature.cold.sh "$PARTICIPANT_NUMBER"
echo "--------------------------------------------------------"

echo -e "\nAktuator: Druck"
./pressure.sh "$PARTICIPANT_NUMBER"
echo "--------------------------------------------------------"

stopLog
cd ..
echo -e "\n\nVielen Dank! Sie haben TEIL 1 beendet.\n\n"

