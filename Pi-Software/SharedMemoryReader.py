# Install sysv_ipc via pip
import sysv_ipc

# Create shared memory objct
# 123456 is the same shared memory ID specified in the C file
memory = sysv_ipc.SharedMemory(123456)

# Read from shared memory
memory_value = memory.read()

# Find the 'end' of the string and strip
i = memory_value.find(b'\0')
if i != -1:
	memory_value = memory_value[:i]

value_as_string = memory_value.decode("utf-8")

print(value_as_string)
