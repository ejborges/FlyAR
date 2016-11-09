#!/bin/bash
# Launch a splash screen giving the user 10 seconds to cancel the startup sequence
echo "FlyAR Starting Up. Press Ctrl-C within 10 seconds to abort."

sleep 10

# Start the sensor reading application in the background
python3 SerialSample.py &

# Start FlyAR.py - don't run this in the background ;)
python3 FlyAR.py
