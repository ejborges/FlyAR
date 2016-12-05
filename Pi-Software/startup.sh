#!/bin/bash

# Trap function so we can exit the bash shell if the user hits Ctrl+C
trap ctrl_c INT

function ctrl_c() {
	clear
	printf "Abort detected!\n"
	exit
}

# Launch a splash screen giving the user 5 seconds to cancel the startup sequence. This also gives the Arduino time to get ready
clear
printf "  ______ _                _____  \n"
printf " |  ____| |         /\   |  __ \ \n"
printf " | |__  | |_   _   /  \  | |__) |\n"
printf " |  __| | | | | | / /\ \ |  _  / \n"
printf " | |    | | |_| |/ ____ \| | \ \ \n"
printf " |_|    |_|\__, /_/    \_\_|  \_\\ \n"
printf "            __/ |                \n"
printf "           |___/                 \n"
printf "FlyAR Starting Up. Press Ctrl-C within 5 seconds to abort.\n"

sleep 5

# Start the sensor reading application in the background
printf "Starting the sensor reader...\n"
cd ~/FlyAR/Pi-Software
sudo python3 arduinoreader.py &
sleep 3
# Start FlyAR.py - don't run this in the background ;)
printf "Starting the main FlyAR application...\n"
sleep 3
clear
sudo python3 flyar.py
