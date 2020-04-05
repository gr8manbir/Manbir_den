#include <queue_mgmt.h>

/* Global variables */
#define PACKETSIZE 4096

//TODO Define global data structures to be used
pthread_mutex_t pth_mutex;         /* pthread mutex */
pthread_cond_t  pth_waitcond;      /* pthread condition variable */

/**
 * This thread is responsible for pulling data off of the shared data 
 * area and processing it using the process_data() API.
 */
void *reader_thread(void *arg) {
	//TODO: Define set-up required
	char *bufp = NULL; /* Will be malloc'ed once we are signalled to conserve memory */
	int  szPack = 0; /* For size of packet at head of queue*/
	int  iRet = 0;
	int  errnum = 0;
	
	/* We have to own the mutex before wait on condition */
	if ( pthread_mutex_lock(&pth_mutex) < 0 ) {
		/* We failed to lock the mutex. Can not proceed with this thread. */
		errnum = errno;
		printf("Failed thread attribute initialization: %d\n", errnum);
		return NULL;
	}
	while(1) {
		//TODO: Define data extraction (queue) and processing 
		/* Wait on condition */
		if ( pthread_cond_wait(&pth_waitcond, &pth_mutex ) < 0 ) {
			/* conditional wait failed. */
			errnum = errno;
			printf("Failed conditional wait: %d\n", errnum);
			/* Assumption: Recovery is wait some time and try again */
			usleep( 50 * 1000UL ); /* 50 msec */
			continue;
		}
		
		/* We now own the mutex. Dequeue data and send it for processing. */
		
		/* 1. Get size of packet on queue */
		szPack = queue_peek( );
		if( 0 == szPack ) {
			/* No packet on queue - can not be zero sized packet 
			 * due to logic used in writer thread. */
			/* Assumption: Best action, sleep for a while and continue */
			pthread_mutex_unlock(&pth_mutex);
			usleep( 50 * 1000UL ); /* 50 msec */
			continue;
		}
		
		/* 2. Dynamically allocate memory for packet - slight performance penalty
		 * over static allocation. Saves memory though. */
		bufp = (char*)malloc((size_t)szPack); /* Allocate memory for packet */
		if( NULL == bufp ) {
			/* OOPS, out of memory. We have not dequeued - no packet lost.
			 * So best action to just continue - Change cond_signal to cond_broadcast */
			pthread_mutex_unlock(&pth_mutex);
			usleep( 50 * 1000UL ); /* 50 msec */
			continue; 
		}
		
		/* 3. Dequeue packet */
		iRet = dequeue((void*)bufp);
		if( (int)E_SUCCESS != iRet ) {
			/* Should never happen as this has been taken care of in peek() */
			pthread_mutex_unlock(&pth_mutex);
			free(bufp);
			usleep( 50 * 1000UL ); /* 50 msec */
			continue; 
		}
		
		/* 4. Unlock mutex */
		if ( pthread_mutex_unlock(&pth_mutex) < 0 ) {
            /* TODO: What to do here? */
		}
		
		/* Send the packet away for processing */
		process_data(bufp, szPack);
		
		/* Free memory */
		free(bufp);
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

	/* The assignment description mentions that it will be judged based
	 * on performance ( avoid polling, wasting CPU cycles ).
	 * So in the speed vs size optimization, I choose speed. Hence declaration
	 * of this 4K size buffer per thread ( 20*4K = 80K overall). If optimizing
	 * for size, this 4K buffer would be allocated to process space (not thread
	 * space) and pushed to inside of the mutex block. So only one thread would
	 * use the 4K block at a time in case of size optimization.
	 */
	char buf[PACKETSIZE] = {0};  /* 4K block */
	int  iRet = 0;
	while(1) {
		//TODO: Define data extraction (device) and storage
		
		/* Zero out memory */
		memset(buf, 0, (size_t)PACKETSIZE);
		
		iRet = get_external_data(buf, (int) PACKETSIZE);
		
		if ( 0 > iRet ) {
			/* We got an error */
			printf("Error reading external data: %d\n", iRet);
			/* Assumption: Recovery in case of error not described 
			 * Sleeping for some time and continuing */
			usleep( 100 * 1000UL ); /* 100 msec */
			continue;
		}
		else if ( 0 == iRet ) {
			/* We read nothing - Nothing to add to queue
			 * Assuming we can pause this thread a while and continue */
			 usleep( 50 * 1000UL ); /* 50 msec */
			 continue;
		}
		
		/* We got data, first lock the mutex */
		if ( pthread_mutex_lock(&pth_mutex) < 0 ) {
			/* TODO: Do something to preserve data */
			continue; /* Can not add to queue without mutex lock */
		}
		/* Add to packet queue */
		if( E_SUCCESS != enqueue(iRet, buf) ) {
			/* TODO: Do something to preserve data */
			pthread_mutex_unlock(&pth_mutex);
			continue; /* Can not signal reader threads to wakeup and process data */
		}
		
		/* Data successfully added to queue, now signal Reader threads
		 * and unlock mutex. Use broadcast instead of signal.
		 */
		if ( pthread_cond_broadcast(&pth_waitcond) < 0 ) {
			/* TODO: What to do here? */
		}
		
		if ( pthread_mutex_unlock(&pth_mutex) < 0 ) {
            /* TODO: What to do here? */
		}
	}
	
	return NULL;
}


#define M 10
#define N 20
int main(int argc, char **argv) {
	int i;
	int errnum = 0;
	pthread_attr_t attr; /* For setting detached state */
	
	/* Below two not required for QNX but this assignment was tested on
	 * linux which requires this to be NOT NULL */
	pthread_t thread_id_M[M];
	pthread_t thread_id_N[N];
	
	/* Init the pthread attributes */
	if( pthread_attr_init(&attr) < 0 ) {
		errnum = errno;
		printf("Failed thread attribute initialization: %d\n", errnum);
		exit(0);
	}
	
	/* Set attr DETACHED STATE ( and stack size )?? */
	if ( pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) < 0 ) {
		errnum = errno;
        printf("Failed to detach thread: %d\n", errnum);
		exit(0);
	}
	
	/* Initialize the mutex and condition variable */
	if ( pthread_mutex_init(&pth_mutex, NULL) < 0 ) {
        errnum = errno;
        printf("Failed to initialize mutex %d\n", errnum);
		exit(0);
	}
    if ( pthread_cond_init(&pth_waitcond, NULL) < 0 ) {
        errnum = errno;
        printf("Failed to initialize wait condition %d\n", errnum);
		exit(0);
	}
	
	for(i = 0; i < N; i++) { 
		pthread_create(&(thread_id_N[i]), &attr, &reader_thread, NULL);
	}

	for(i = 0; i < M; i++) { 
		pthread_create(&(thread_id_M[i]), &attr, &writer_thread, NULL);
	}
	
	while(1) {
		/* We do not want main to exit as it will kill the detached threads.
		 * Also we do not want the main thread to be scheduled any time soon as it
		 * does nothing.
		 * TODO: Think of some "main" process exit criteria.
		 */
		sleep(10);
	}
	return 0;	
}

/* Placeholder functions to compile source code */
int get_external_data(char *buffer, int bufferSizeInBytes) {
	(void)(buffer);
	(void)(bufferSizeInBytes);
	/* Do nothing */
	return 0;
}
void process_data(char *buffer, int bufferSizeInBytes) {
	(void)(buffer);
	(void)(bufferSizeInBytes);
	/* Do nothing */
}