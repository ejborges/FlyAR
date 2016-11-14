# Reads in the config file from the boot partition to determine what shapes need to be displayed

class FlyARShape():
    def __init__(self):
        self._shapeType = None
        self._red = None
        self._green = None
        self._blue = None
        self._x = None
        self._y = None
        self._z = None
        self._xRadius = None
        self._yRadius = None

    @property
    def shapeType(self):
        return self._shapeType
    
    @shapeType.setter
    def shapeType(self, value):
        self._shapeType = value

    @property
    def red(self):
        return self._red

    @red.setter
    def red(self, value):
        self._red = value

    @property
    def green(self):
        return self._green

    @green.setter
    def green(self, value):
        self._green = value

    @property
    def blue(self):
        return self._blue

    @blue.setter
    def blue(self, value):
        self._blue = value

    @property
    def x(self):
        return self._x

    @x.setter
    def x(self, value):
        self._x = value

    @property
    def y(self):
        return self._y

    @y.setter
    def y(self, value):
        self._y = value

    @property
    def z(self):
        return self._z

    @z.setter
    def z(self, value):
        self._z = value

    @property
    def xRadius(self):
        return self._xRadius

    @xRadius.setter
    def xRadius(self, value):
        self._xRadius = value

    @property
    def yRadius(self):
        return self._yRadius

    @yRadius.setter
    def yRadius(self, value):
        self._yRadius = value


def read_config():
    filePath = '/boot/FlyAR/config.txt'
    with open(filePath, 'r') as ins:
        for line in ins:
            # Parse the line, and create a FlyARShape for the main FlyAR application
            pieces = line.strip().split(',')

            shapeType = pieces[0]
            red = pieces[1]
            green = pieces[2]
            blue = pieces[3]
            x = pieces[4]
            y = pieces[5]
            z = pieces[6]
            xRadius = pieces[7]
            yRadius = pieces[8]

            # Store the shape
            print("SHAPE VALUES")
            print("============")
            print("Shape Type: {}".format(shapeType))
            print("Red Value: {}".format(red))
            print("Green Value: {}".format(green))
            print("Blue Value: {}".format(blue))
            print("X Position: {}".format(x))
            print("Y Position: {}".format(y))
            print("Z Position: {}".format(z))
            print("X Radius: {}".format(xRadius))
            print("Y Radius: {}".format(yRadius))
