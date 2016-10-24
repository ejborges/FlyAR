import serial
ser = serial.Serial("/dev/ttyAMA0")
ser.baudrate = 115200
data = ser.read(...)
print data
