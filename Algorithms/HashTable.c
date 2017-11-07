//Basic implementation of a hash table with linear probing - Coding practice
//Date: 7-Nov-2017
#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 256 //Size of HashTable 

//Single Hash table entry
struct DataItem
{
   int key;
   int data;
};

//Array of pointers for HashTable
struct DataItem *HashArray[MAX_SIZE];
struct DataItem *DummyItem;

struct DataItem *item;

//Hashing function
int HashIndex(int key)
{
    return (key%MAX_SIZE);
}

struct DataItem * search(int key)
{
    int hashIndex = HashIndex(key); // Get the index from key
 
    //Since we use linear probing, we need to search from index....size
    while(HashArray[hashIndex] != NULL )
    {
        if(HashArray[hashIndex]->key == key )
            return HashArray[hashIndex]; //We found it

	hashIndex++;
        hashIndex %= MAX_SIZE;        
    }
    
}

void insert(int key, int data )
{
    int hashIndex = HashIndex(key); // Get index where to add

    item = (struct DataItem *) malloc(sizeof(struct DataItem));

    item->data = data;
    item->key = key;

    // Since location may already be occupied, need linear probing to find empty/deleted noded
    while(HashArray[hashIndex] != NULL && HashArray[hashIndex]->key !=-1 )
    {
         hashIndex++;
         hashIndex %= MAX_SIZE;
    } 

    //We found the hashIndex, add data
    HashArray[hashIndex] = item;
}


struct DataItem* delete(struct DataItem *item)
{
    int hashIndex = HashIndex(item->key);
    
    //Search for item
    while( HashArray[hashIndex] != NULL )
    {
        if (HashArray[hashIndex]->key == item->key)
        {
	    struct DataItem *temp = HashArray[hashIndex];
            HashArray[hashIndex] = DummyItem;
            return temp;
        }
        ++hashIndex;
	hashIndex %= MAX_SIZE;        
    }

    return NULL;
}

void display()
{
    for(int i = 0; i<MAX_SIZE; i++)
    {
        if(HashArray[i] != NULL)
        {
            printf("\n%d-%d\n", HashArray[i]->key, HashArray[i]->data );
        }
        else printf("~~\n");
    }
}

int main()
{
    DummyItem  = (struct DataItem*) malloc(sizeof(struct DataItem));
    DummyItem->key = -1;
    DummyItem->data = -1;

   insert(1, 20);
   insert(2, 70);
   insert(42, 80);
   insert(4, 25);
   insert(12, 44);
   insert(14, 32);
   insert(17, 11);
   insert(13, 78);
   insert(37, 97);

   display();
   item = search(37);

   if(item != NULL) {
      printf("Element found: %d\n", item->data);
   } else {
      printf("Element not found\n");
   }

   delete(item);
   item = search(37);

   if(item != NULL) {
      printf("Element found: %d\n", item->data);
   } else {
      printf("Element not found\n");
   }
}
