from serial import Serial
import logging
logging.basicConfig(filename='/var/tmpflyar/flyar.log', filemode='w', level=logging.INFO)
from sysv_ipc import SharedMemory, IPC_CREX
from time import time
from smclear import padString
import gc

# Open the serial connection. The Pi sometimes like to switch back and forth between ttyACM0 and ttyACM1, so try 0. If it fails, fall back to 1.
try:
    ser = Serial("/dev/ttyACM0", 115200, timeout=.5)
except SerialException:
    ser = Serial("/dev/ttyACM1", 115200, timeout=.5)

# Set up the necessary shared memory locations
rollMem = SharedMemory(None, IPC_CREX)
pitchMem = SharedMemory(None, IPC_CREX)
yawMem = SharedMemory(None, IPC_CREX)

# Store the shared memory locations to a file
with open('/var/tmpflyar/raw_shared_memory_keys.flyar', 'wb') as f:
    sharedMemoryKeys = '{},{},{}\n'.format(rollMem.key, pitchMem.key, yawMem.key)
    logging.info(str(currentTime) + ':[ARDUINO-READER] Storing the following shared memory keys to /var/tmpflyar/raw_shared_memory_keys.flyar: {}'.format(sharedMemoryKeys))
    f.write(str.encode(sharedMemoryKeys))

while True:
    # Send a '1' byte to the Arduino to indicate that we want data
    ser.write(b'1')

    # Now that the Arduino should now be sending back data, read it in
    data = ser.readline()


    # Process the data and store it in shared memory
    orien = data.strip().split(',')
    rollMem.write(padString("{0:.4f}".format(orien[0])).encode())
    pitchMem.write(padString("{0:.4f}".format(orien[1])).encode())
    yawMem.write(padString("{0:.4f}".format(orien[2])).encode())

    gc.collect()

    # Let's do it again!

# endwhile

# We should never get here unless it breaks out of an infinite loop somehow...
