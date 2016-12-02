import serial
from time import sleep

ser = serial.Serial('/dev/ttyACM1', 115200)
s = [0,1]

for x in range(0,1000):
    if x % 2 ==0:
        print('0')
        ser.write(b'0')
    else:
        print('1')
        ser.write(b'1')

    sleep(.5)
    print(ser.readline())
    print(ser.readline())


#while True:
#    read_serial = ser.readline()
#    s[0] = str(int(ser.readline(),16))
#    print(s[0])
#    print(read_serial)
