from sense_hat import SenseHat
from time import time
import math
from scipy import signal

# Get data

sense = SenseHat()
previousTime = -1
ACCELEROMETER_DRIFT_WHEN_STATIONARY = 0.05
samplePeriod = 1/256
while True:
    # Get sensor data and the current time
    gyro = sense.get_gyroscope_raw()    # deg/s
    acc = sense.get_accelerometer_raw() # g's (1g = 9.81 m/s^2)
    currentTime = time()

    # Calculate the magnitude of acceleration
    accMagnitude = sqrt((acc['x'] * acc['x']) + (acc['y'] * acc['y']) + (acc['z'] * acc['z']))

    # Use a high-pass filter to remove some noise
    filterCutoff = 0.001
    butterFilterResult = signal.butter(1, (2*filterCutoff)/(1/samplePeriod), 'highpass')
    accMagnitudeFiltered = signal.filtfilt(butterFilterResult['b'], butterFilterResult['a'], accMagnitude)

    # Take the absolute value of the filtered magnitude
    accMagnitudeFiltered = abs(accMagnitudeFiltered)

    # Use a low-pass filter to remove some noise
    filterCutoff = 5
    butterFilterResult = signal.butter(1, (2*filterCutoff)/(1/samplePeriod), 'lowpass')
    accMagnitudeFiltered = signal.filtfilt(butterFilterResult['b'], butterFilterResult['a'], accMagnitudeFiltered)

    # Are we actually stationary?
    stationary = accMagnitudeFiltered < ACCELEROMETER_DRIFT_WHEN_STATIONARY

    # Compute orientation
    # TODO
