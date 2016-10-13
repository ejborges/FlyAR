from sense_hat import SenseHat
import os

sense = SenseHat()
sense.clear()
#sense.set_rotation(180) # It was appearing upside down for me ;)
#sense.show_message("Hello world!") # This is a blocking statement, FYI

while (True):
    humidity = sense.get_humidity()
    print("Humidity: {}".format(humidity))
    print("Temperature (via humidity): {} c".format(sense.get_temperature()))
    print("Temperature (via pressure): {} c".format(sense.get_temperature_from_pressure()))
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
    print("Compass: {}".format(sense.get_compass()))
    os.system('clear')
