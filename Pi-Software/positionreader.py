from sysv_ipc import SharedMemory
from os import system

xPosMem = None
yPosMem = None
zPosMem = None
roll = None
pitch = None
yaw = None
with open('/var/tmpflyar/formatted_shared_memory_keys.flyar', 'r') as f:
    for line in f:
        values = line.strip().split(',')
        xPosMem = SharedMemory(int(values[0]))
        yPosMem = SharedMemory(int(values[1]))
        zPosMem = SharedMemory(int(values[2]))
        roll = SharedMemory(int(values[3]))
        pitch = SharedMemory(int(values[4]))
        yaw = SharedMemory(int(values[5]))

print("X Position: {}".format(xPosMem.read().strip()))
print("TEST: {}".format(xPosMem.read()))
print("Y Position: {}".format(yPosMem.read().strip()))
print("Z Position: {}".format(zPosMem.read().strip()))
print("Roll:       {}".format(roll.read().strip()))
print("Pitch:      {}".format(pitch.read().strip()))
print("Yaw:        {}".format(yaw.read().strip()))
