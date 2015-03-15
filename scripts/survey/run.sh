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

#---------------------------------------------------
#----------------    PART II    --------------------
#---------------------------------------------------

echo "--------------------------------------------------------"
echo -e "TEIL 2\n"
echo "--------------------------------------------------------"
echo "Jetzt wollen wir testen, wie gut Sie verschiedene Stufen"
echo "an Stimulationen unterscheiden koennen.  Dazu werden wir"
echo "die Aktuatoren der Reihe nach testen. Jeder Test besteht"
ehco "aus zwei Schritten:"
echo "1) Kalibrierung: Sie werden fuehlen, wie der Aktuator von"
echo "                 einem Intensitaets-Level zu einem anderen"
echo "                 wechselt."
echo "                 Die Intensitaets-Level werden Ihnen dazu"
echo "                 jeweils auf dem Bildschirm angezeigt."
echo " "
echo "2) Test: Sie werden fuehlen, wie der Aktuator von einem"
echo "         Intensitaets-Level zu einem anderen wechselt."
echo "         Dieses Mal werden Ihnen die Werte aber nicht"
echo "         angezeigt, stattdessen muessen Sie raten und"
echo "         dazu den ersten und zweiten Wert, den Sie gefuehlt 
echo "         haben, eingeben."

echo -e "\nFalls Sie Fragen haben, koennen Sie diese nun gerne stellen."

echo "--------------------------------------------------------"

cd partII
startLog "partII"

echo -e "\nAktuator: Temperatur (warm)"
./temperature.hot.sh "$PARTICIPANT_NUMBER"
echo "--------------------------------------------------------"

echo -e "\nAktuator: Vibration"
./vibration.sh "$PARTICIPANT_NUMBER"
echo "--------------------------------------------------------"

echo -e "\nactuator: electric"
echo -e "\nAktuator: Elektrisch"
read -p "Was war ihr maximaler Elektrische-Stimulation Wert?" maxElectro
echo -e "Okay, wir werden keine Werte ueber diesem Limit testen!\n"
./electric.sh "$PARTICIPANT_NUMBER" "$maxElectro"
echo "--------------------------------------------------------"

echo -e "\nAktuator: Temperatur (kalt)"
./temperature.cold.sh "$PARTICIPANT_NUMBER"
echo "--------------------------------------------------------"

echo -e "\nAktuator: Druck"
./pressure.sh "$PARTICIPANT_NUMBER"
echo "--------------------------------------------------------"

stopLog
