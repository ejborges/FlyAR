from sense_hat import SenseHat

sense = SenseHat()
sense.clear()

print("COLLECTING DATA. PRESS CTRL+C TO EXIT")
with open('pi_at_rest.csv', 'wb') as f:
    header = b'pitch,roll,yaw'
    f.write(header + b'\n')
    row = '{},{},{}'

    while (True):
        orien = sense.get_orientation()

        textToWrite = row.format(orien['pitch'], orien['roll'], orien['yaw'])
        f.write(str.encode(textToWrite))
        f.write(b'\n')
