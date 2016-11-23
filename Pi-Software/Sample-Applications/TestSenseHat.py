from sense_hat import SenseHat
import math
import os

sense = SenseHat()
sense.clear()
sense.set_rotation(180) # It was appearing upside down for me ;)
#sense.show_message("Hello world!") # This is a blocking statement, FYI

offset = sense.get_accelerometer_raw()
OFFSET_X = offset['x']
OFFSET_Y = offset['y']
OFFSET_Z = offset['z']

offset = sense.get_orientation()
OFFSET_PITCH = offset['pitch']
OFFSET_ROLL = offset['roll']
OFFSET_YAW = offset['yaw']


positionX = 0
positionY = 0
positionZ = 0
while (True):
    humidity = sense.get_humidity()
    print("Pressure: {} mbar".format(sense.get_pressure()))
    print("")
    print("ORIENTATION")
    print("===========")
    print("Orientation in rad/s:")
    orientation = sense.get_orientation()
    orientationRad = sense.get_orientation_radians()
    print("==> Pitch (X): {}".format(orientation['pitch'] - OFFSET_PITCH))
    print("==>  Roll (Z): {}".format(orientation['roll'] - OFFSET_ROLL))
    print("==>   Yaw (Y): {}".format(orientation['yaw'] - OFFSET_YAW))
    print("")
    print("ACCELEROMETER DATA DELTA: RAW")
    print("=======================")
    accel = sense.get_accelerometer_raw()
    print("==> X: {}".format(accel['x'] - OFFSET_X))
    print("==> Y: {}".format(accel['y'] - OFFSET_Y))
    print("==> Z: {}".format(accel['z'] - OFFSET_Z))
    print("")
    print("POSITION")
    print("========")
    if accel['x'] - OFFSET_X > 0.014152:
        accel['x'] = accel['x'] * math.cos(orientationRad['roll'])
        positionX += (accel['x'] - OFFSET_X) * 9.81
    if accel['y'] - OFFSET_Y > 0.006832:
        accel['y'] = accel['y'] * math.sin(orientationRad['pitch'])
        positionY += (accel['y'] - OFFSET_Y) * 9.81
    print("==> X: {}".format(positionX))
    print("==> Y: {}".format(positionY))

    os.system('clear')
