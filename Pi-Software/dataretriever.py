from sysv_ipc import SharedMemory

class FlyARData(object):
    def __init__(self):
        self._haveSharedMemKeys = False
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
                    self._haveSharedMemKeys = True
                    values = line.strip().split(',')
                    self._rollMem = SharedMemory(int(values[0]))
                    self._pitchMem = SharedMemory(int(values[1]))
                    self._yawMem = SharedMemory(int(values[2]))

        # Get the latest data from shared memory
        self._roll = float(self._rollMem.read())
        self._pitch = float(self._pitchMem.read())
        self._yaw = float(self._yawMem.read())

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
