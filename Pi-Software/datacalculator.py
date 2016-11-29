# Takes the values from the data file and provides the latest orientation and position
import logging
logging.basicConfig(filename='/var/tmpflyar/flyar.log', level=logging.INFO)
from time import time
from accelerationcalculator import getFilteredAcceleration
from sysv_ipc import SharedMemory, IPC_CREX
from math import degrees
from smclear import padString

previousTime = None

xAccelMem = None
yAccelMem = None
zAccelMem = None
rollMem = None
pitchMem = None
yawMem = None
rollDegMemFrom = None
pitchDegMemFrom = None
yawDegMemFrom = None

# Get the shared memory keys from the config file living in /var/tmpflyar
with open('/var/tmpflyar/raw_shared_memory_keys.flyar', 'r') as f:
    for line in f:
        values = line.strip().split(',')
        xAccelMem = SharedMemory(int(values[0]))
        yAccelMem = SharedMemory(int(values[1]))
        zAccelMem = SharedMemory(int(values[2]))
        rollMem = SharedMemory(int(values[3]))
        pitchMem = SharedMemory(int(values[4]))
        yawMem = SharedMemory(int(values[5]))
        rollDegMemFrom = SharedMemory(int(values[6]))
        pitchDegMemFrom = SharedMemory(int(values[7]))
        yawDegMemFrom = SharedMemory(int(values[8]))

# Get shared memory locations for the actual data we'll need
xPosMem = SharedMemory(None, IPC_CREX)
yPosMem = SharedMemory(None, IPC_CREX)
zPosMem = SharedMemory(None, IPC_CREX)
rollDegMemTo = SharedMemory(None, IPC_CREX)
pitchDegMemTo = SharedMemory(None, IPC_CREX)
yawDegMemTo = SharedMemory(None, IPC_CREX)

# Write these locations to file
with open('/var/tmpflyar/formatted_shared_memory_keys.flyar', 'wb') as f:
    f.write(str.encode('{},{},{},{},{},{}\n'.format(xPosMem.key, yPosMem.key, zPosMem.key, rollDegMemTo.key, pitchDegMemTo.key, yawDegMemTo.key)))

xPosition = 0
yPosition = 0
zPosition = 0
calibrationNumber = 0
NUMBER_OF_CALIBRATION_POINTS = 5000
CALIBRATED_ACCEL_X = 0
CALIBRATED_ACCEL_Y = 0
CALIBRATED_ACCEL_Z = 0

previousRollRad = 0
previousRollDeg = 0
previousPitchRad = 0
previousPitchDeg = 0
previousYawRad = 0
previousYawDeg = 0
while True:
    # Get the latest data from shared memory
    xAccel = float(xAccelMem.read())
    yAccel = float(yAccelMem.read())
    zAccel = float(zAccelMem.read())
    rollRad = float(rollMem.read())
    pitchRad = float(pitchMem.read())
    yawRad = float(yawMem.read())
    rollDeg = float(rollDegMemFrom.read())
    pitchDeg = float(pitchDegMemFrom.read())
    yawDeg = float(yawDegMemFrom.read())

    # If we haven't exceeded the calibration period, keep a running average to use
    if calibrationNumber < NUMBER_OF_CALIBRATION_POINTS:
        calibrationNumber += 1

        CALIBRATED_ACCEL_X = (CALIBRATED_ACCEL_X + xAccel) / calibrationNumber
        CALIBRATED_ACCEL_Y = (CALIBRATED_ACCEL_Y + yAccel) / calibrationNumber
        CALIBRATED_ACCEL_Z = (CALIBRATED_ACCEL_Z + zAccel) / calibrationNumber

        if calibrationNumber == NUMBER_OF_CALIBRATION_POINTS:
            
            loggingTime = str(time())
            logging.info(loggingTime + ":[DATA-CALCULATOR] Calibration values")
            logging.info(loggingTime + ":[DATA-CALCULATOR]     X Accel: {}".format(CALIBRATED_ACCEL_X))
            logging.info(loggingTime + ":[DATA-CALCULATOR]     Y Accel: {}".format(CALIBRATED_ACCEL_Y))
            logging.info(loggingTime + ":[DATA-CALCULATOR]     Z Accel: {}".format(CALIBRATED_ACCEL_Z))

        continue

    # If previous time is None, then skip this read so we have a time gap to use
    if previousTime == None:
        previousRollRad = rollRad
        previousRollDeg = rollDeg
        previousPitchRad = pitchRad
        previousPitchDeg = pitchDeg
        previousYawRad = yawRad
        previousYawDeg = yawDeg
        previousTime = time()
        continue
    
    # Remove the calibrated values (offsets) from the readings
    xAccel -= CALIBRATED_ACCEL_X
    yAccel -= CALIBRATED_ACCEL_Y
    zAccel -= CALIBRATED_ACCEL_Z

    # Only change roll, pitch, and yaw if the values changed enough (more than their drift values)
    newRollDeg = previousRollDeg
    newRollRad = previousRollRad
    newPitchDeg = previousPitchDeg
    newPitchRad = previousPitchRad
    newYawDeg = previousYawDeg
    newYawRad = previousYawRad
    if abs(rollDeg - previousRollDeg) > .24933539:
        newRollDeg = rollDeg
        newRollRad = rollRad

    if abs(pitchDeg - previousPitchDeg) > .31902688:
        newPitchDeg = pitchDeg
        newPitchRad = pitchRad

    if abs(yawDeg - previousYawDeg) > .365389732:
        newYawDeg = yawDeg
        newYawRad = yawRad
    
    filteredAcceleration = getFilteredAcceleration(xAccel, yAccel, zAccel, newRollRad, newPitchRad, newYawRad)
    
    # Calculate the distance traveled by calculating velocity and then position
    currentTime = time()
    timeDelta = currentTime - previousTime
    positionChangeX = filteredAcceleration[0] * timeDelta * timeDelta
    positionChangeY = filteredAcceleration[1] * timeDelta * timeDelta
    positionChangeZ = filteredAcceleration[2] * timeDelta * timeDelta
    
    # Update the data
    previousTime = currentTime
    xPosition += positionChangeX
    yPosition += positionChangeY
    zPosition += positionChangeZ

    # Store the data in the calculated shared memory
    xPosMem.write(padString(str(xPosition)).encode())
    yPosMem.write(padString(str(yPosition)).encode())
    zPosMem.write(padString(str(zPosition)).encode())
    rollDegMemTo.write(padString(str(newRollDeg)).encode())
    pitchDegMemTo.write(padString(str(newPitchDeg)).encode())
    yawDegMemTo.write(padString(str(newYawDeg)).encode())

    previousRollDeg = rollDeg
    previousRollRad = rollRad
    previousPitchDeg = pitchDeg
    previousPitchRad = pitchRad
    previousYawDeg = yawDeg
    previousYawRad = yawRad

    # Do it again!
