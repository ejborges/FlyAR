#!/bin/bash
# Launch a splash screen giving the user 10 seconds to cancel the startup sequence
clear
printf "                   ______     ___   ___ \n"
printf "                  / __/ /_ __/ _ | / _ \\n"
printf "                 / _// / // / __ |/ , _/\n"
printf "                /_/ /_/\_, /_/ |_/_/|_| \n"
printf "                      /___/             \n"
printf "FlyAR Starting Up. Press Ctrl-C within 10 seconds to abort."

sleep 10

# Start the sensor reading application in the background
python3 SerialSample.py &

# Start FlyAR.py - don't run this in the background ;)
python3 FlyAR.py
