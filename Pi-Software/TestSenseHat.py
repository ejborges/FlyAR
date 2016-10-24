from sense_hat import SenseHat
import os

sense = SenseHat()
sense.clear()
sense.set_rotation(180) # It was appearing upside down for me ;)
#sense.show_message("Hello world!") # This is a blocking statement, FYI

while (True):
    humidity = sense.get_humidity()
    print("Pressure: {} mbar".format(sense.get_pressure()))
    print("")
    print("ORIENTATION")
    print("===========")
    print("Orientation in degrees:")
    orientation = sense.get_orientation_degrees()
    print("==> Pitch: {}".format(orientation['pitch']))
    print("==>  Roll: {}".format(orientation['roll']))
    print("==>   Yaw: {}".format(orientation['yaw']))
    print("")
    print("ACCELEROMETER DATA: RAW")
    print("=======================")
    accel = sense.get_accelerometer_raw()
    print("==> X: {}".format(accel['x']))
    print("==> Y: {}".format(accel['y']))
    print("==> Z: {}".format(accel['z']))
    os.system('clear')
