#from serial import Serial
from sysv_ipc import SharedMemory, IPC_CREX

#ser = Serial("/dev/ttyAMA0")
#ser.baudrate = 115200

sharedMemoryXPosition = SharedMemory(None, IPC_CREX)
sharedMemoryYPosition = SharedMemory(None, IPC_CREX)
sharedMemoryZPosition = SharedMemory(None, IPC_CREX)
sharedMemoryRoll = SharedMemory(None, IPC_CREX)
sharedMemoryPitch = SharedMemory(None, IPC_CREX)
sharedMemoryYaw = SharedMemory(None, IPC_CREX)

xPositionKey = sharedMemoryXPosition.key
yPositionKey = sharedMemoryYPosition.key
zPositionKey = sharedMemoryZPosition.key
rollKey = sharedMemoryRoll.key
pitchKey = sharedMemoryPitch.key
yawKey = sharedMemoryYaw.key

# Store a temp file at the home folder that holds the keys so the actual software knows where everything is stored
with open('shared_memory_keys.flyar', 'wb') as f:
    f.write(str.encode("{},{},{},{},{},{}".format(xPositionKey, yPositionKey, zPositionKey, rollKey, pitchKey, yawKey)))


while False:
    # Send a '1' byte to the Arduino to indicate that we want data
    ser.write(b"1")

    # Now that the Arduino should now be sending bakc data, read it in
    NUMBER_OF_BYTES_TO_READ = 1   # This needs to be changed!
    data = ser.read(NUMBER_OF_BYTES_TO_READ)

    # Send a '0' byte to the Arduino to indicate that we got the data
    ser.write(b"0")

    # Process the data and store it in shared memory
    # sample block of adding to shared memory
    sharedMemoryXPosition.write("10") #all should be string values; easier to parse!

    # Let's do it again!

# endwhile

# We should never get here unless it breaks out of an infinite loop somehow...
