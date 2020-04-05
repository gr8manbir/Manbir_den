/* File: queue_mgmt.h 
 * Defines the data structure to be used for assignment */
 
#ifndef __QUEUE_MGMT_H__
#define __QUEUE_MGMT_H__

#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

/*======Data types======*/
typedef struct node{
	int  szPack;
	void *data;
	struct node *next;
}node;

typedef enum {
	E_SUCCESS,
	E_MEM_ERR,
	E_EMPTY_QUEUE,
}E_RET_CODE;

/*======Function prototypes======*/
node* CreateNode( int szPack, void *data );
int enqueue( int szPack, void *data );
int dequeue( void *bufp );
int queue_peek( void );
#endif /* __QUEUE_MGMT_H__ */
