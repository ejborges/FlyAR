from sysv_ipc import SharedMemory
from os import system

xPosMem = None
yPosMem = None
zPosMem = None
with open('/var/tmpflyar/formatted_shared_memory_keys.flyar', 'r') as f:
    for line in f:
        values = line.strip().split(',')
        xPosMem = SharedMemory(int(values[0]))
        yPosMem = SharedMemory(int(values[1]))
        zPosMem = SharedMemory(int(values[2]))

while True:
    print("X Position: {}".format(xPosMem.read()))
    print("Y Position: {}".format(yPosMem.read()))
    print("Z Position: {}".format(zPosMem.read()))
    system('clear')
