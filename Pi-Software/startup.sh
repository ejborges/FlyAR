#!/bin/bash
# Launch a splash screen giving the user 10 seconds to cancel the startup sequence
clear
printf "  ______ _                _____  \n"
printf " |  ____| |         /\   |  __ \ \n"
printf " | |__  | |_   _   /  \  | |__) |\n"
printf " |  __| | | | | | / /\ \ |  _  / \n"
printf " | |    | | |_| |/ ____ \| | \ \ \n"
printf " |_|    |_|\__, /_/    \_\_|  \_\\n"
printf "            __/ |                \n"
printf "           |___/                 \n"
printf "FlyAR Starting Up. Press Ctrl-C within 10 seconds to abort."

sleep 10

# Start the sensor reading application in the background
python3 SerialSample.py &

# Start FlyAR.py - don't run this in the background ;)
python3 FlyAR.py
