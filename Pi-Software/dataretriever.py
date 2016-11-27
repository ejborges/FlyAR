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
