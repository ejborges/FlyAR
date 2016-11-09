from sense_hat import SenseHat

sense = SenseHat()
sense.clear()

print("COLLECTING DATA. PRESS CTRL+C TO EXIT")
with open('pi_at_rest.csv', 'wb') as f:
    header = b'accelX,accelY,accelZ,pressure'
    f.write(header + b'\n')
    row = '{},{},{},{}'

    while (True):
        pressure = sense.get_pressure()
        accel = sense.get_accelerometer_raw()

        textToWrite = row.format(accel['x'], accel['y'], accel['z'], pressure)
        f.write(str.encode(textToWrite))
        f.write(b'\n')
