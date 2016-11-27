from sysv_ipc import SharedMemory

class FlyARData(object):
    def __init__(self):
        self._haveSharedMemKeys = False
        self._xPosMem = None
        self._xPosition = 0
        self._yPosMem = None
        self._yPosition = 0
        self._zPosMem = None
        self._zPosition = 0
        self._rollMem = None
        self._roll = 0
        self._pitchMem = None
        self._pitch = 0
        self._yawMem = None
        self._yaw = 0

    def update(self):
        '''
        Updates the object with the latest from shared memory. Then call the 
        respective properties to get updated values
        '''
        if not self._haveSharedMemKeys:
            # Get the shared memory keys from the file
            with open('/var/tmpflyar/formatted_shared_memory_keys.flyar', 'r') as f:
                for line in f:
                    self._haveSharedMemoryKeys = True
                    values = line.strip().split(',')
                    self._xPosMem = SharedMemory(int(values[0]))
                    self._yPosMem = SharedMemory(int(values[1]))
                    self._zPosMem = SharedMemory(int(values[2]))
                    self._rollMem = SharedMemory(int(values[3]))
                    self._pitchMem = SharedMemory(int(values[4]))
                    self._yawMem = SharedMemory(int(values[5]))

        # Get the latest data from shared memory
        self._xPosition = float(self._xPosMem.read())
        self._yPosition = float(self._yPosMem.read())
        self._zPosition = float(self._zPosMem.read())
        self._roll = float(self._rollMem.read())
        self._pitch = float(self._pitchMem.read())
        self._yaw = float(self._yawMem.read())

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
