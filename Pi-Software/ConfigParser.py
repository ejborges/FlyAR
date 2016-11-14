# Reads in the config file from the boot partition to determine what shapes need to be displayed
import FlyARShape

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
            shape = FlyARShape()
            shape.shapeType(pieces[0])
            shape.color(pieces[1:4])
            shape.position(pieces[4:7])
            shape.radii(pieces[7:])
            
            print("SHAPE VALUES")
            print("============")
            print("Shape Type: {}".format(shape.shapeType()))
            print("Color: {}".format(shape.color()))
            print("Position: {}".format(shape.position()))
            print("Radii: {}".format(shape.radii()))
