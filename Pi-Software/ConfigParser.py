# Reads in the config file from the boot partition to determine what shapes need to be displayed
from FlyARShape import FlyARShape

def read_config():
    filePath = '/boot/FlyAR/config.txt'
    with open(filePath, 'r') as ins:
        for line in ins:
            # Parse the line, and create a FlyARShape for the main FlyAR application
            pieces = line.strip().split(',')

            shapeType = pieces[0]
            red = int(pieces[1])
            green = int(pieces[2])
            blue = int(pieces[3])
            color = [red, green, blue]
            x = float(pieces[4])
            y = float(pieces[5])
            z = float(pieces[6])
            position = [x, y, z]
            xRadius = float(pieces[7])
            yRadius = float(pieces[8])
            radii = [xRadius, yRadius]

            # Store the shape
            shape = FlyARShape()
            shape.shapeType = pieces[0]
            shape.color = color
            shape.position = position
            shape.radii = radii
            
            return shape
