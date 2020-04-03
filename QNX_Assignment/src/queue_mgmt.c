/* File: queue_mgmt.c 
 * Implements the data structure functions used for this assignment */

#include <queue_mgmt.h>

/* Function to create a node and fill it with packet data */
node* CreateNode( int szPack, void *data){
	node *temp = NULL;
	
	/* First allocate memory for node */
	temp = (node*) malloc(sizeof(node));
	temp->szPack = szPack;
	
	temp->data = (void*)malloc(szPack); /* Allocate memory for packet */
	memcpy(temp->data, (const void *)data, szPack); /* Fill data from hardware */
	temp->next = NULL;

    return temp;
}

/* Function to enqueue a node */
void enqueue( node *temp ){
	/* Mutex lock here */
	
	/* If HEAD is NULL, we are the only node */
	if ( NULL == head ) {
		head = temp;
		tail = temp;
	}
	else {
		/* Add to the end of the queue */
		tail->next = temp;
		tail = temp;
	}
	/* Mutex unlock here */
}

int dequeue( void * bufp) {
	node *temp = NULL;
	/* Mutex lock here */
	
	/* Check for empty queue*/
	if ( NULL == head ) {
		bufp = NULL;
		return 0;
	}
	else {
		temp = head;
		memcpy(bufp, (const void*) temp->data, temp->szPack);
		head = head ->next;
		
		/* Free memory */
		free(temp->data);
		free(temp);
	}
	/* Mutex unlock here */
}