/* File: queue_mgmt.h 
 * Defines the data structure to be used for assignment */
 
#ifndef __QUEUE_MGMT_H__
#define __QUEUE_MGMT_H__

#include <pthread.h>
#include <errno.h>

/*======Data types======*/
typedef struct {
	int  szPack;
	void *data;
	node *next;
}node;

/* Using a linear queue for this task */
node *head = NULL;
node *tail = NULL;

/*======Function prototypes======*/
node* CreateNode( int szPack, void *data );
void  enqueue( node *temp );
int dequeue( void *bufp );
#endif /* __QUEUE_MGMT_H__ */
