#ifndef _HASHMAP_H_
#define _HASHMAP_H_
typedef struct Node Node;
struct mapNode 
{ 

	// key is string NOT STRING
	int* key;

	// value is also string NOT STRING
	void* value;
	struct mapNode* next;
};
struct hashMap {

	// Current number of elements in hashMap
	// and capacity of hashMap
	int numOfElements, capacity;

	// hold base address array of linked list
	struct mapNode** arr;
};
void setNode(struct mapNode* node, int* key, void* value);
void initializeHashMap(struct hashMap* mp);
int hashFunction(struct hashMap* mp, int* key);
void insertMap(struct hashMap* mp, int* key, void* value);
void delete (struct hashMap* mp, int* key);
void* search(struct hashMap* mp, int* key);

#endif