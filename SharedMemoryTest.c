// Sample from http://stackoverflow.com/questions/1268252/python-possible-to-share-in-memory-data-between-2-separate-processes/28376308#28376308

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main (int argc, const char **argv) {
	int shared_memory_id;

	key_t key = 123456;
	char *shared_memory;

	// Set up shared memory, 11 is the size
	if ((shared_memory_id = shmget(key, 11, IPC_CREAT | 0666)) < 0) {
		printf("Error getting shared memory id");
		exit(1);
	}

	// Attach shared memory
	if ((shared_memory = shmat(shared_memory_id, NULL, 0)) == (char *) -1) {
		printf("Error attaching shared memory id");
		exit(1);
	}

	//Copy "hello world" to shared memory
	memcpy(shared_memory, "Hello World", sizeof("Hello World"));

	// Sleep for 20 sec so we can run the reader
	sleep(20);

	// Detach and remove shared memory
	shmdt(shared_memory_id);
	shmctl(shared_memory_id, IPC_RMID, NULL);
}
