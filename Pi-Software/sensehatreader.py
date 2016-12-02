#from serial import Serial
import logging
logging.basicConfig(filename='/var/tmpflyar/flyar.log', filemode='w', level=logging.INFO)
from sense_hat import SenseHat
from sysv_ipc import SharedMemory, IPC_CREX
from time import time
from smclear import padString
import gc

#ser = Serial("/dev/ttyAMA0")
#ser.baudrate = 115200

sense = SenseHat()

# Set up the necessary shared memory locations
xAccelMem = SharedMemory(None, IPC_CREX)
yAccelMem = SharedMemory(None, IPC_CREX)
zAccelMem = SharedMemory(None, IPC_CREX)
rollMem = SharedMemory(None, IPC_CREX)
pitchMem = SharedMemory(None, IPC_CREX)
yawMem = SharedMemory(None, IPC_CREX)
rollDegMem = SharedMemory(None, IPC_CREX)
pitchDegMem = SharedMemory(None, IPC_CREX)
yawDegMem = SharedMemory(None, IPC_CREX)
currentTime = time()

# Store the shared memory locations to a file
with open('/var/tmpflyar/raw_shared_memory_keys.flyar', 'wb') as f:
    sharedMemoryKeys = '{},{},{},{},{},{},{},{},{}\n'.format(xAccelMem.key, yAccelMem.key, zAccelMem.key, rollMem.key, pitchMem.key, yawMem.key, rollDegMem.key, pitchDegMem.key, yawDegMem.key)
    logging.info(str(currentTime) + ':[SERIAL-READER] Storing the following shared memory keys to /var/tmpflyar/raw_shared_memory_keys.flyar: {}'.format(sharedMemoryKeys))
    f.write(str.encode(sharedMemoryKeys))

while True:
    # Send a '1' byte to the Arduino to indicate that we want data
#    ser.write(b"1")

    # Now that the Arduino should now be sending bakc data, read it in
#    NUMBER_OF_BYTES_TO_READ = 1   # This needs to be changed!
#    data = ser.read(NUMBER_OF_BYTES_TO_READ)

    # Send a '0' byte to the Arduino to indicate that we got the data
#    ser.write(b"0")

    # Process the data and store it in shared memory
    # sample block of adding to shared memory
    accel = sense.get_accelerometer_raw()
    orien = sense.get_orientation_radians()
    orienDeg = sense.get_orientation_degrees()
    xAccelMem.write(padString("{0:.4f}".format(accel['x'])).encode())
    yAccelMem.write(padString("{0:.4f}".format(accel['y'])).encode())
    zAccelMem.write(padString("{0:.4f}".format(accel['z'])).encode())
    rollMem.write(padString("{0:.4f}".format(orien['roll'])).encode())
    pitchMem.write(padString("{0:.4f}".format(orien['pitch'])).encode())
    yawMem.write(padString("{0:.4f}".format(orien['yaw'])).encode())
    rollDegMem.write(padString("{0:.4f}".format(orienDeg['roll'])).encode())
    pitchDegMem.write(padString("{0:.4f}".format(orienDeg['pitch'])).encode())
    yawDegMem.write(padString("{0:.4f}".format(orienDeg['yaw'])).encode())

    gc.collect()

    # Let's do it again!

# endwhile

# We should never get here unless it breaks out of an infinite loop somehow...
