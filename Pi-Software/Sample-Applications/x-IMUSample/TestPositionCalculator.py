from sense_hat import SenseHat
from time import time
from math import sqrt
from scipy import signal
from madgwick_py.madgwickahrs import MadgwickAHRS
from quat_rotate import qv_mult
import numpy as np

# Get data

sense = SenseHat()
previousTime = -1
ACCELEROMETER_DRIFT_WHEN_STATIONARY = 0.00000000001
samplePeriod = 1/256

posX = 0
posY = 0
posZ = 0
while True:
    # Get sensor data and the current time
    gyro = sense.get_gyroscope_raw()    # deg/s
    acc = sense.get_accelerometer_raw() # g's (1g = 9.81 m/s^2)
    mag = sense.get_compass_raw()       # microteslas (uT)
    currentTime = time()

    # Calculate the magnitude of acceleration
    accMagnitude = sqrt((acc['x'] * acc['x']) + (acc['y'] * acc['y']) + (acc['z'] * acc['z']))

    # Use a high-pass filter to remove some noise
    filterCutoff = 0.001
    butterFilterB, butterFilterA = signal.butter(1, (2*filterCutoff)/(1/samplePeriod), 'highpass')
    accMagnitudeFiltered = signal.filtfilt(butterFilterB, butterFilterA, [accMagnitude, accMagnitude], padlen=1)

    # Take the absolute value of the filtered magnitude
    accMagnitudeFiltered = abs(accMagnitudeFiltered)

    # Use a low-pass filter to remove some noise
    filterCutoff = 5
    butterFilterB, butterFilterA = signal.butter(1, (2*filterCutoff)/(1/samplePeriod), 'lowpass')
    accMagnitudeFiltered = signal.filtfilt(butterFilterB, butterFilterA, [accMagnitudeFiltered, accMagnitudeFiltered], padlen=1)

    # Are we actually stationary?
    stationary = accMagnitudeFiltered < ACCELEROMETER_DRIFT_WHEN_STATIONARY
    print(accMagnitudeFiltered)

    # If we are stationary, don't bother doing anything
    if stationary.any():
        continue

    # Compute orientation
    ahrsAlgorithm = MadgwickAHRS()
    ahrsAlgorithm.update([gyro['x'], gyro['y'], gyro['z']], [acc['x'], acc['y'], acc['z']], [mag['x'], mag['y'], mag['z']])
    quaternion = ahrsAlgorithm.quaternion

    # Rotate body accelerations to earth frame
    trueAcceleration = qv_mult(quaternion.conj(), [acc['x'], acc['y'], acc['z']])

    # Convert acceleration to m/s/s
    accX = acc['x'] * 9.81
    accY = acc['y'] * 9.81
    accZ = acc['z'] * 9.81

    # Compute translational velocities
    accX -= 9.81
    accY -= 9.81
    accZ -= 9.81

    # We'll assume acceleration == velocity
    # if this is the first iteration, don't add to the position
    velX = 0
    velY = 0
    velZ = 0
    if previousTime != -1:
        # Do the magic
        timeDiff = currentTime - previousTime
        velX = accX * timeDiff
        velY = accY * timeDiff
        velZ = accZ * timeDiff

        # Add to the position
        posX += velX * timeDiff
        posY += velY * timeDiff
        posZ += velZ * timeDiff

    # Set the previous time to the current time
    previousTime = currentTime

    # Print position
    print("X: {}".format(posX))
    print("Y: {}".format(posY))
    print("Z: {}".format(posZ))

