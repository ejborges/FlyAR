# Reads in the config file from the boot partition to determine what shapes need to be displayed

def read_config():
    filePath = '/boot/FlyAR/config.txt'
    with open(filePath, 'r') as ins:
        for line in ins:
            # Parse the line, and create a FlyARShape for the main FlyAR application
            pieces = line.strip().split(',')

            x = pieces[0]
            y = pieces[1]
            z = pieces[2]
            shapeType = pieces[3]

            # Depending on the shape type, grab shape-specific values
            if shapeType == 1:
                # Do something
            elif shapeType == 2:
                # Do something

            # Store the shape
