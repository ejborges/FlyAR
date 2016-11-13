from sense_hat import SenseHat
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


while (True):
    humidity = sense.get_humidity()
    print("Pressure: {} mbar".format(sense.get_pressure()))
    print("")
    print("ORIENTATION")
    print("===========")
    print("Orientation in rad/s:")
    orientation = sense.get_orientation()
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

    os.system('clear')
