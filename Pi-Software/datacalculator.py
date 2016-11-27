# Takes the values from the data file and provides the latest orientation and position
from time import time
from accelerationcalculator import getFilteredAcceleration
from sysv_ipc import SharedMemory, IPC_CREX
from math import degrees

previousTime = None

xAccelMem = None
yAccelMem = None
zAccelMem = None
rollMem = None
pitchMem = None
yawMem = None

# Get the shared memory keys from the config file living in /var/tmpflyar
with open('/var/tmpflyar/raw_shared_memory.keys.flyar', 'r') as f:
    for line in f:
        values = line.strip().split(',')
        xAccelMem = SharedMemory(int(values[0]))
        yAccelMem = SharedMemory(int(values[1]))
        zAccelMem = SharedMemory(int(values[2]))
        rollMem = SharedMemory(int(values[3]))
        pitchMem = SharedMemory(int(values[4]))
        yawMem = SharedMemory(int(values[5]))

# Get shared memory locations for the actual data we'll need
xPosMem = SharedMemory(None, IPC_CREX)
yPosMem = SharedMemory(None, IPC_CREX)
zPosMem = SharedMemory(None, IPC_CREX)
rollDegMem = SharedMemory(None, IPC_CREX)
pitchDegMem = SharedMemory(None, IPC_CREX)
yawDegMem = SharedMemory(None, IPC_CREX)

xPosition = 0
yPosition = 0
zPosition = 0
while True:
    # Get the latest data from shared memory
    xAccel = float(xAccelMem.read())
    yAccel = float(yAccelMem.read())
    zAccel = float(zAccelMem.read())
    roll = float(rollMem.read())
    pitch = float(pitchMem.read())
    yaw = float(yawMem.read())
    
    filteredAcceleration = getFilteredAcceleration(xAccel, yAccel, zAccel, roll, pitch, yaw)
    
    # Calculate the distance traveled by calculating velocity and then position
    currentTime = time()
    timeDelta = currentTime - self._time
    positionChangeX = filteredAcceleration[0] * timeDelta * timeDelta
    positionChangeY = filteredAcceleration[1] * timeDelta * timeDelta
    positionChangeZ = filteredAcceleration[2] * timeDelta * timeDelta
    
    # Update the data
    previousTime = currentTime
    xPosition += positionChangeX
    yPosition += positionChangeY
    zPosition += positionChangeZ
    rollDeg = degrees(roll)
    pitchDeg = degrees(pitch)
    yawDeg = degrees(yaw)

    # Store the data in the calculated shared memory
    xPosMem.write(str(xPosition).encode())
    yPosMem.write(str(yPosition).encode())
    zPosMem.write(str(zPosition).encode())
    rollDegMem.write(str(rollDeg).encode())
    pitchDegMem.write(str(pitchDeg).encode())
    yawDegMem.write(str(yawDeg).encode())

    # Do it again!


class FlyARData(object):
    def __init__(self):
        self._xPosition = 0
        self._yPosition = 0
        self._zPosition = 0
        self._roll = 0
        self._pitch = 0
        self._yaw = 0

    @property
    def xPosition(self):
        return self._xPosition

    @xPosition.setter
    def xPosition(self, value):
        self._xPosition = value

    @property
    def yPosition(self):
        return self._yPosition

    @yPosition.setter
    def yPosition(self, value):
        self._yPosition = value

    @property
    def zPosition(self):
        return self._zPosition

    @zPosition.setter
    def zPosition(self, value):
        self._zPosition = value

    @property
    def roll(self):
        return self._roll

    @roll.setter
    def roll(self, value):
        self._roll = value

    @property
    def pitch(self):
        return self._pitch

    @pitch.setter
    def pitch(self, value):
        self._pitch = value

    @property
    def yaw(self):
        return self._yaw

    @yaw.setter
    def yaw(self, value):
        self._yaw = value
