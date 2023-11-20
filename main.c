#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "avltree.h"
#include "hashmap.h"
//have a variable that keeps track of the largest gap between memory
//so that you can check in O(1) time if there the job can fit
//i think we can use a stack for this but im still thinking about how to do it
//its hard to know how to get the largest gap between memory without actually going
//through every time a job is freed to calculate the gaps but im pretty sure its 
//possible to do it without


void* largeBlock;
int largeBlockSize;
struct hashMap *startMap;
struct hashMap *endMap;
struct treeNode *free_root;

void mem_init(size_t size){
    //initialize memory block
    free_root=NULL;
    largeBlock = (void*)malloc(size);
    largeBlockSize=size;
    free_root = insertTree(free_root, largeBlockSize, largeBlock);
    if(largeBlock == NULL){
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }


    //initialize startpoint hashmap
    startMap = (struct hashMap*)malloc(sizeof(struct hashMap));
	initializeHashMap(startMap);

    //initialize endpoint hashmap
    endMap = (struct hashMap*)malloc(sizeof(struct hashMap));
	initializeHashMap(endMap);

    
}
//asdasd

void my_free(void *ptr){
    int tmp = (int)(ptr-largeBlock);
    int* thisStartPoint = &tmp;
    printf("value of this startpoint: %i\n", *thisStartPoint);
    //printf("%i\n", *tmp);
    struct treeNode* memToFree = (struct treeNode*)search(startMap, thisStartPoint);
    if(!memToFree){
        printf("this should be impossible\n");
        return;
    }
    //this is startpoint of the node we want to free
    int tmp2 = (intptr_t)(memToFree->start) + memToFree->size + 1;
    int* nextStartPoint = &tmp2;
    //might need to assign to void ptr first
    struct treeNode* nodeInFront = search(startMap, nextStartPoint);
    //nodeInFront is never NULL since the hashmap always has nodes wheher the space is being used or not
    if(!nodeInFront){
        printf("node in front is null");
        return;
    }
    //this will either be just the size of the node we wanna free or the size of the node we wanna free + node in front
    int sizeOfNode;
    //if nodeInFront is free memory
    if(!(nodeInFront->inUse)){
        sizeOfNode = memToFree->size + nodeInFront->size;
        //remove the node in front from all structures because its gonna combine with the one we are pointing to
        //delete from hashmap first because the tree is gonna free the memory
        delete(startMap, nextStartPoint);
        //might be size - 1 idk
        int* nextEndPoint = *nextStartPoint + &nodeInFront->size;
        delete(endMap, nextEndPoint);
        free_root = deleteNode(free_root, nodeInFront->size, nodeInFront->start);    
    }
    else{
        //if nodeInFront is being used then just add the node to free to the tree
        sizeOfNode = memToFree->size;
    }
    free_root = insertTree(free_root, sizeOfNode, thisStartPoint);
}
    

void* my_malloc(size_t size){
    
    if(!free_root){
        //if the tree is empty get the block of memory
        //largeBlock ptr will be start of the memory endpoint will be ptr+size
        if(size>largeBlockSize){
            perror("Not enough memory");
            exit(EXIT_FAILURE);
        }
        void* a;
        a=largeBlock;
        
        a=a+(int)size;
        free_root=insertTree(free_root,largeBlockSize-size, a);
        
        return largeBlock;
    }
    struct treeNode* ret = bestFit(free_root, size);
    if(!ret){
        perror("Not enough T memory");
        exit(EXIT_FAILURE);
        return NULL;
    }else{
        int* startPointFirstNode = ret->start;
        //yeah this is a 25 characeter variable its fine
        int endPointFirstNodeStorage = *startPointFirstNode + (int)size - 1;
        int* endPointFirstNode = &endPointFirstNodeStorage;

        int freeSpaceSize = (int)(ret->size)-size;

        int startPointNextNodeStorage = endPointFirstNodeStorage+1;
        int* startPointNextNode = &startPointNextNodeStorage;
        //freespacesize is the size of the next node because the next node is the free one right hahaha 
        int endPointNextNodeStorage = startPointNextNodeStorage + freeSpaceSize;
        int* endPointNextNode = &endPointNextNodeStorage;
        //delete old free space node and insert newly sized free space into tree
        free_root = deleteNode(free_root, ret->size,startPointFirstNode);
        free_root = insertTree(free_root, freeSpaceSize, startPointNextNode);
        printf("hi\n");
        struct treeNode* freeNode = findNode(free_root, (ret->size)-size,startPointNextNode);
        printf("hi\n");

        //in use node
        printf("hi\n");
        struct treeNode* inUseNode = newNode(size, startPointFirstNode, true);
        printf("hi\n");
        printf("hi\n");
        //add both to startpoint map
        insertMap(startMap, (int*)startPointFirstNode, inUseNode);
        insertMap(startMap, (int*)startPointNextNode, freeNode);

        //add both to endpoint map
        insertMap(endMap, (int*)endPointFirstNode, inUseNode);
        insertMap(endMap, (int*)endPointNextNode, freeNode);
        return startPointFirstNode;
    }
}
void preOrder(struct treeNode *root)
{
    if(root != NULL)
    {
        printf("%d ", root->size);
        preOrder(root->left);
        preOrder(root->right);
    }
    
}

int main(){
    mem_init(100);
    //preOrder(free_root);
    //void* p = my_malloc(25);
    //preOrder(free_root);
    //printf("\n");
    //void* n=my_malloc(25);
    //printf("%p\n", p);
    
    //printf("%p\n", p+25);
    //printf("%p\n", n);
    //preOrder(free_root);
    //printf("\n");
    printf("Malloc a node of size 30\n");
    void* k=my_malloc(30);
    printf("preorder:\n");
    preOrder(free_root);
   // printf("%p\n", k);
    //printf("%p\n", p+70);
    //preOrder(free_root);
    printf("freeing:\n");
    my_free(k);
    printf("\n");
}
