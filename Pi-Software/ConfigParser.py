# Reads in the config file from the boot partition to determine what shapes need to be displayed
from FlyARShape import FlyARShape

def read_config():
    filePath = '/boot/FlyAR/config.txt'

    shapes = []
    with open(filePath, 'r') as ins:
        for line in ins:
            # Parse the line, and create a FlyARShape for the main FlyAR application
            pieces = line.strip().split(',')

            shapeType = int(pieces[0])
            red = int(pieces[1])
            green = int(pieces[2])
            blue = int(pieces[3])
            color = [red, green, blue]
            x = float(pieces[4])
            y = float(pieces[6])
            z = float(pieces[5])
            position = [x, y, z]
            xRadius = float(pieces[7])
            yRadius = float(pieces[8])
            radii = [xRadius, yRadius]

            # Store the shape
            shape = FlyARShape()
            shape.shapeType = shapeType
            shape.color = color
            shape.position = position
            shape.radii = radii

            shapes.append(shape)

    return shapes
