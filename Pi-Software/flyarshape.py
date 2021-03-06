# An actual shape object to be used by the 3D rendering script. Contains all necessary information
class FlyARShape(object):
    def __init__(self):
        self._shapeType = None
        self._color = None
        self._position = None
        self._radius = None

    @property
    def shapeType(self):
        return self._shapeType
    
    @shapeType.setter
    def shapeType(self, value):
        self._shapeType = value

    @property
    def color(self):
        return self._color

    @color.setter
    def color(self, value):
        self._color = value

    @property
    def position(self):
        return self._position

    @position.setter
    def position(self, value):
        self._position = value

    @property
    def radius(self):
        return self._radius

    @radius.setter
    def radius(self, value):
        self._radius = value
