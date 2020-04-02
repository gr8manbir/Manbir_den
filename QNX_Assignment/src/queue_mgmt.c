/* File: queue_mgmt.c 
 * Implements the data structure functions used for this assignment */

/* Function to create a node and fill it with packet data */
node* CreateNode( int szPack, void *data){
	node *temp = NULL;
	
	/* First allocate memory for node */
	temp = (node*) malloc(sizeof(node));
	temp->szPack = szPack;
	
	temp->data = (void*)malloc(szPack); /* Allocate memory for packet */
	memcpy(temp->data, (const void *)data, szPack); /* Fill data from hardware */

    return temp;
}

/* Function to enqueue a node */
void enqueue( node *temp){
	
	/* MUTEX LOCK HERE */
	/* MUTEX UNLOCK HERE */
}