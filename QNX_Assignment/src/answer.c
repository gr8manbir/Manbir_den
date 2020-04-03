

#include <queue_mgmt.h>

//TODO Define global data structures to be used

/**
 * This thread is responsible for pulling data off of the shared data 
 * area and processing it using the process_data() API.
 */
void *reader_thread(void *arg) {
	//TODO: Define set-up required
	
	while(1) {
		//TODO: Define data extraction (queue) and processing 
	}
	
	return NULL;
}


/**
 * This thread is responsible for pulling data from a device using
 * the get_external_data() API and placing it into a shared area
 * for later processing by one of the reader threads.
 */
void *writer_thread(void *arg) {
	//TODO: Define set-up required
	
	while(1) {
		//TODO: Define data extraction (device) and storage 
	}
	
	return NULL;
}


#define M 10
#define N 20
int main(int argc, char **argv) {
	int i;
	int errnum = 0;
	/* Synchronization variables(pthread) */
	pthread_attr_t attr;
	
	/* Init the pthread attributes */
	if(pthread_attr_init(&attr) < 0 ) {
		errnum = errno;
		printf("Failed thread attribute initialization: %d\n", errnum);
		exit(0);
	}
	
	/* Set attr DETACHED STATE ( and stack size )?? */
	if ( pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) < 0) {
		errnum = errno;
        printf("Failed to detach thread: %d\n", errnum);
		exit(0);
	}
	for(i = 0; i < N; i++) { 
		pthread_create(NULL, NULL, reader_thread, NULL);
	}

	for(i = 0; i < M; i++) { 
		pthread_create(NULL, NULL, writer_thread, NULL);
	}
	
	while(1) {
		/* We do not want main to exit as it will kill the detached threads.
		 * Also we do not want this thread to be scheduled any time soon as it
		 * does nothing.
		 * TODO: Think of some exit criteria.
		 */
		sleep(10);
	}
	return 0;	
}