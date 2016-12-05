from sysv_ipc import SharedMemory
from os import system

roll = None
pitch = None
yaw = None
with open('/var/tmpflyar/raw_shared_memory_keys.flyar', 'r') as f:
    for line in f:
        values = line.strip().split(',')
        roll = SharedMemory(int(values[2]))
        pitch = SharedMemory(int(values[1]))
        yaw = SharedMemory(int(values[0]))

print("Roll:       {}".format(roll.read().strip()))
print("Pitch:      {}".format(pitch.read().strip()))
print("Yaw:        {}".format(yaw.read().strip()))
