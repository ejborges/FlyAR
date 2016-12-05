import logging
logging.basicConfig(filename='/var/tmpflyar/flyar.log', filemode='w', level=logging.INFO)
from sense_hat import SenseHat
from sysv_ipc import SharedMemory, IPC_CREX
from time import time
from smclear import padString
import gc


sense = SenseHat()

# Set up the necessary shared memory locations
rollMem = SharedMemory(None, IPC_CREX)
pitchMem = SharedMemory(None, IPC_CREX)
yawMem = SharedMemory(None, IPC_CREX)
currentTime = time()

# Store the shared memory locations to a file
with open('/var/tmpflyar/raw_shared_memory_keys.flyar', 'wb') as f:
    sharedMemoryKeys = '{},{},{}\n'.format(rollMem.key, pitchMem.key, yawMem.key)
    logging.info(str(currentTime) + ':[SENSE-READER] Storing the following shared memory keys to /var/tmpflyar/raw_shared_memory_keys.flyar: {}'.format(sharedMemoryKeys))
    f.write(str.encode(sharedMemoryKeys))

while True:
    # Process the data and store it in shared memory
    orien = sense.get_orientation_degrees()
    rollMem.write(padString("{0:.4f}".format(orien['roll'])).encode())
    pitchMem.write(padString("{0:.4f}".format(orien['pitch'])).encode())
    yawMem.write(padString("{0:.4f}".format(orien['yaw'])).encode())

    gc.collect()

    # Let's do it again!

# endwhile

# We should never get here unless it breaks out of an infinite loop somehow...
