#include <queue_mgmt.h>

/* Global variables */
#define PACKETSIZE 4096

//TODO Define global data structures to be used
pthread_mutex_t pth_mutex;         /* pthread mutex */
pthread_cond_t  pth_waitcond;      /* pthread condition variable */

/* Below function kills currently running program */
void killProgram()
{
	usleep( 10 * 1000UL ); /* 10 msec to complete any remaining printf */
	/* Below should kill the process without further data (integrity) loss */
	raise(SIGABRT);
}

/**
 * This thread is responsible for pulling data off of the shared data 
 * area and processing it using the process_data() API.
 */
void *reader_thread(void *arg) {
	//TODO: Define set-up required
	char *bufp  = NULL; /* Will be malloc'ed later to conserve memory */
	int  szPack = 0; /* For size of packet at head of queue*/
	int  iRet   = 0;
	
	/* We have to own the mutex before wait on condition */
	iRet = pthread_mutex_lock(&pth_mutex);
	if ( iRet < 0 ) {
		/* We failed to lock the mutex. Can not proceed with this thread.
		 * Assumption: One thread failure does not impact other threads */
		printf("ERROR: Failed mutex lock: %d\n", iRet);
		return NULL;
	}
	
	while(1) {
		//TODO: Define data extraction (queue) and processing 
		/* Wait on condition */
		iRet = pthread_cond_wait(&pth_waitcond, &pth_mutex );
		if ( iRet < 0 ) {
			/* conditional wait failed. */
			printf("Failed conditional wait: %d\n", iRet);
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
			iRet = pthread_mutex_unlock(&pth_mutex);
			if( iRet < 0 ) {
				printf("ERROR: Failed to unlock mutex(broadcast): %d\n", iRet);
				killProgram();
			}
			usleep( 50 * 1000UL ); /* 50 msec */
			continue;
		}
		
		/* 2. Dynamically allocate memory for packet - slight performance penalty
		 * over static allocation. Saves memory though. */
		bufp = (char*)malloc((size_t)szPack); /* Allocate memory for packet */
		if( NULL == bufp ) {
			/* OOPS, out of memory. We have not dequeued - no packet lost.
			 * So best action to just continue - Change cond_signal to cond_broadcast */
			iRet = pthread_mutex_unlock(&pth_mutex);
			if( iRet < 0 ) {
				printf("ERROR: Failed to unlock mutex(broadcast): %d\n", iRet);
				killProgram();
			}
			usleep( 50 * 1000UL ); /* 50 msec */
			continue; 
		}
		memset((void*)bufp, 0, (size_t)szPack); /* Zero out the memory */
		
		/* 3. Dequeue packet */
		iRet = dequeue((void*)bufp);
		if( (int)E_SUCCESS != iRet ) {
			/* Should never happen as this scenario has been taken care of in peek() */
			iRet = pthread_mutex_unlock(&pth_mutex);
			if( iRet < 0 ) {
				printf("ERROR: Failed to unlock mutex(broadcast): %d\n", iRet);
				killProgram();
			}
			free(bufp);
			usleep( 50 * 1000UL ); /* 50 msec */
			continue; 
		}
		
		/* 4. Unlock mutex */
		if ( pthread_mutex_unlock(&pth_mutex) < 0 ) {
			/* System will now lockup( mutex locked ). Finish processing of 
			 * current packet and abort */
			iRet = E_SYS_LOCKUP;
		}
		
		/* Send the packet away for processing */
		process_data(bufp, szPack);
		
		/* Free memory */
		free(bufp);
		
		if( E_SYS_LOCKUP == iRet ) {
			/* Better to let this process die. An external daemon monitor can 
			 * do a clean restart then */
			printf("ERROR: Failed to unlock mutex(broadcast): %d\n", iRet);
			killProgram();
		}
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
	 * of this 4K size buffer per thread ( 20*4K=80K overall). If optimizing
	 * for size, this 4K buffer would be allocated to process space (not thread
	 * space) and pushed to inside of the mutex block. So only one thread would
	 * use the 4K block at a time in case of size optimization.
	 */
	char buf[PACKETSIZE] = {0};  /* 4K block */
	int  iRet            = 0;
	
	while(1) {
		//TODO: Define data extraction (device) and storage
		
		/* Zero out memory */
		memset(buf, 0, (size_t)PACKETSIZE);
		
		iRet = get_external_data(buf, (int)PACKETSIZE);	
		if ( 0 > iRet ) {
			/* We got an error */
			printf("Error reading external data: %d\n", iRet);
			/* Assumption: Recovery in case of error not described 
			 * Sleeping for some time and continuing */
			usleep( 50 * 1000UL ); /* 50 msec */
			continue;
		}
		else if ( 0 == iRet ) {
			/* We read nothing - Nothing to add to queue
			 * Assumption: We need to pause this thread and continue */
			 usleep( 50 * 1000UL ); /* 50 msec */
			 continue;
		}
		
		/* We got data, first lock the mutex */
		iRet = pthread_mutex_lock(&pth_mutex);
		if ( iRet < 0 ) {
			/* Data packet loss - points to a deeper system issue */
			printf("ERROR: Failed to lock mutex: %d\n", iRet);
			/* Can not add to queue without mutex lock */
			killProgram();
		}
		/* Add to packet queue */
		if( E_SUCCESS != enqueue(iRet, buf) ) {
			/* Data packet loss - points to a deeper system issue */
			iRet = pthread_mutex_unlock(&pth_mutex);
			if( iRet < 0 ) {
				printf("ERROR: Failed to unlock mutex: %d\n", iRet);
			}
			/* Should not signal reader threads to wake, further process data*/
			killProgram();
		}
		
		/* Data successfully added to queue, now signal Reader threads
		 * and unlock mutex. Use broadcast instead of signal.
		 */
		if ( pthread_cond_broadcast(&pth_waitcond) < 0 ) {
			/* One lost broadcast will not cause system failure 
			 * as in future packet RX, all threads will wake up one
			 * by one and should process any remaining data on queue.
			 * Print log and continue to unlock mutex */
			 printf("Failed to broadcast\n");
		}
		
		iRet = pthread_mutex_unlock(&pth_mutex);
        if( iRet < 0 ) {
			/* Can not proceed as data integrity not guaranteed */
			printf("ERROR: Failed to unlock mutex(broadcast): %d\n", iRet);
			killProgram();
		}
	}
	
	return NULL;
}


#define M 10
#define N 20
int main(int argc, char **argv) {
	int i;
	int iRet   = 0;
	pthread_attr_t attr; /* For setting detached state */
	
	/* Below two not required for QNX but this assignment was tested on
	 * Linux which requires this param to be NOT NULL */
	pthread_t thread_id_M[M];
	pthread_t thread_id_N[N];
	
	/* Register signal handler */
	signal( SIGABRT, SIG_DFL );   /* SIGABRT should kill process */
	
	/* Init the pthread attributes */
	iRet = pthread_attr_init(&attr);
	if( iRet < 0 ) {
		printf("Failed thread attribute initialization: %d\n", iRet);
		exit(0);
	}
	
	/* Set attr DETACHED STATE ( and stack size )?? */
	iRet = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if ( iRet < 0 ) {
        printf("Failed to detach thread: %d\n", iRet);
		exit(0);
	}
	
	/* Initialize the mutex and condition variable */
	iRet = pthread_mutex_init(&pth_mutex, NULL);
	if ( iRet < 0 ) {
        printf("Failed to initialize mutex %d\n", iRet);
		exit(0);
	}
	iRet = pthread_cond_init(&pth_waitcond, NULL);
    if ( iRet < 0 ) {
        printf("Failed to initialize wait condition %d\n", iRet);
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
		 * Also we do not want the main thread to be scheduled any time soon as
		 * it does nothing useful.
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