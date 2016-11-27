#from serial import Serial
import logging
logging.basicConfig(filename='/var/tmpflyar/flyar.log', filemode='w', level=logging.WARN)
from sense_hat import SenseHat
from sysv_ipc import SharedMemory, IPC_CREX
from time import time

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
currentTime = time()

# Store the shared memory locations to a file
with open('/var/tmpflyar/raw_shared_memory_keys.flyar', 'wb') as f:
    sharedMemoryKeys = '{},{},{},{},{},{}\n'.format(xAccelMem.key, yAccelMem.key, zAccelMem.key, rollMem.key, pitchMem.key, yawMem.key)
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
    xAccelMem.write(str(accel['x']).encode())
    yAccelMem.write(str(accel['y']).encode())
    zAccelMem.write(str(accel['z']).encode())
    rollMem.write(str(orien['roll']).encode())
    pitchMem.write(str(orien['pitch']).encode())
    yawMem.write(str(orien['yaw']).encode())
    currentTime = time()
    logging.info(str(currentTime) + ':[SERIAL-READER] Just wrote {} to the X Shared Memory location'.format(str(accel['x'])))
    logging.info(str(currentTime) + ':[SERIAL-READER] Just wrote {} to the Z Shared Memory location'.format(str(accel['z'])))
    logging.info(str(currentTime) + ':[SERIAL-READER] Just wrote {} to the ROLL Shared Memory location'.format(str(orien['roll'])))
    logging.info(str(currentTime) + ':[SERIAL-READER] Just wrote {} to the PITCH Shared Memory location'.format(str(orien['pitch'])))
    logging.info(str(currentTime) + ':[SERIAL-READER] Just wrote {} to the YAW Shared Memory location'.format(str(orien['yaw'])))
    logging.info(str(currentTime) + ':[SERIAL-READER] Just wrote {} to the Y Shared Memory location'.format(str(accel['y'])))

    # Let's do it again!

# endwhile

# We should never get here unless it breaks out of an infinite loop somehow...
