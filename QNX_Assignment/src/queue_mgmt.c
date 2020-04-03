/* File: queue_mgmt.c 
 * Implements the data structure functions used for this assignment */

#include <queue_mgmt.h>

/* Using a linear queue for this assignment */
node *head = NULL;
node *tail = NULL;

/* Function to enqueue a node */
int enqueue( int szPack, void *data){
	node *temp = NULL;
	
	/* First allocate memory for node */
	temp = (node*) malloc(sizeof(node));
	if( NULL == temp ) {
		/* OOPS, out of memory */
		return E_MEM_ERR;
	}
	temp->szPack = szPack;
	
	temp->data = (void*)malloc(szPack); /* Allocate memory for packet */
	if( NULL == temp->data ) {
		/* OOPS, out of memory */
		free(temp);
		return E_MEM_ERR;
	}
	memcpy(temp->data, (const void *)data, szPack); /* Fill data from hardware */
	temp->next = NULL;

	if ( NULL == head ) {
		head = temp;
		tail = temp;
	}
	else {
		/* Add to the end of the queue */
		tail->next = temp;
		tail = temp;
	}
	
	return E_SUCCESS;
}

int dequeue( void * bufp) {
	node *temp = NULL;
	
	/* Check for empty queue*/
	if ( NULL == head ) {
		return E_EMPTY_QUEUE;
	}
	else {
		temp = head;
		memcpy(bufp, (const void*) temp->data, temp->szPack);
		head = head ->next;
		
		/* Free memory */
		free(temp->data);
		free(temp);
	}
	
	return E_SUCCESS;
}