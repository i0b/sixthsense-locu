#!/bin/bash

source temperature.sh

ACTYATOR_TYPE="temperature cold"
ACTUATOR_MODE="temp"
ACTUATOR="3"
ACTUATOR_MAX="15"
#GREP_TIMEOUT="2"
PARAMETER="0"

LIST_TIMEOUT="0.5"

runStatistic
