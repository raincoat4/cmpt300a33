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
    intptr_t a =(intptr_t)(ptr-largeBlock);
    int b=(int)a/4;
    int* thisStartPoint = &b;
    printf("%li, %li\n", ptr, largeBlock);
    printf("value of this startpoint: %i\n", *thisStartPoint);
    //printf("%i\n", *tmp);
    struct treeNode* memToFree = (struct treeNode*)search(startMap, thisStartPoint);
    if(!memToFree){
        printf("this should be impossible\n");
        return;
    }
    //this is startpoint of the node we want to free
    
    intptr_t tmp2 = (intptr_t)((memToFree->start)-(int*)largeBlock);
    intptr_t tmpb= tmp2+(memToFree->size);
    int tmpc= (int)tmpb;
    int* nextStartPoint = &tmpc;
    //might need to assign to void ptr first
    struct treeNode* nodeInFront = search(startMap, nextStartPoint);
    //nodeInFront is never NULL since the hashmap always has nodes whether the space is being used or not
    if(!nodeInFront){
        printf("node in front is null");
        return;
    }
    //this will either be just the size of the node we wanna free or the size of the node we wanna free + node in front
    int sizeOfNode=0;
    //if nodeInFront is free memory
    intptr_t tmp3 = (intptr_t)((memToFree->start)-(int*)largeBlock);
    intptr_t tmpd= tmp3--;
    int tmpe= (int)tmpd;
    int* backEndPoint = &tmpe;
    //might need to assign to void ptr first
    struct treeNode* nodeInBack = search(startMap, backEndPoint);
    printf("node in back %i\n", nodeInBack->size);
    //| job a | job b | free |
    //| job a | free  | free |
    
    printf("node in front %i\n", nodeInFront->size);
    if(!(nodeInFront->inUse)){
        sizeOfNode = sizeOfNode+memToFree->size + nodeInFront->size;
        //remove the node in front from all structures because its gonna combine with the one we are pointing to
        //delete from hashmap first because the tree is gonna free the memory
        delete(startMap, nextStartPoint);
        //might be size - 1 idk
        int* nextEndPoint = *nextStartPoint + &nodeInFront->size;
        
        delete(endMap, nextEndPoint);
        
        free_root = deleteNode(free_root, nodeInFront->size, nodeInFront->start);    
    }
    if(nodeInBack==NULL){
        printf("node in back is null");
        return;
    }
    printf("%i\n", (nodeInBack->inUse));
    if(!(nodeInBack->inUse)){
        printf(" size: %i\n", nodeInBack->size);
        sizeOfNode = sizeOfNode+memToFree->size + nodeInBack->size;
        //remove the node in back from all structures because its gonna combine with the one we are pointing to
        //delete from hashmap first because the tree is gonna free the memory
        
        delete(endMap, backEndPoint);
        //might be size - 1 idk
        int* backStartPoint = *backEndPoint + &nodeInBack->size;
        
        delete(startMap, backStartPoint);
        
        free_root = deleteNode(free_root, nodeInBack->size, nodeInBack->start);
    }
    if((nodeInBack->inUse)&&(nodeInFront->inUse)){
        //if nodeInFront&nodeInBack is being used then just add the node to free to the tree
        sizeOfNode = memToFree->size;
    }
    printf("sizeOfNode: %d\n", sizeOfNode);
    free_root = insertTree(free_root, sizeOfNode, thisStartPoint);
}
    

void* my_malloc(size_t size){
    
    
    struct treeNode* ret = bestFit(free_root, size);
    if(!ret){
        perror("Not enough T memory");
        exit(EXIT_FAILURE);
        return NULL;
    }else{
        int* startPointFirstNode = ret->start;
        //yeah this is a 25 characeter variable its fine
        printf("size %li\n", startPointFirstNode+(intptr_t)size-1);
        //int endPointFirstNodeStorage = (int)startPointFirstNode + ((int)size - 1);
        //printf("%i\n", endPointFirstNodeStorage);
        int* endPointFirstNode = startPointFirstNode + ((int)size - 1);
        //printf("%li\n", (intptr_t)(endPointFirstNodeStorage-startPointFirstNode));
        int freeSpaceSize = (int)(ret->size)-size;
        
        //int startPointNextNodeStorage = endPointFirstNodeStorage+1;
        int* startPointNextNode = startPointFirstNode + ((int)size);
        //freespacesize is the size of the next node because the next node is the free one right hahaha 
        //int endPointNextNodeStorage = startPointNextNodeStorage + freeSpaceSize;
        int* endPointNextNode = startPointNextNode+freeSpaceSize;
        //delete old free space node and insert newly sized free space into tree
        free_root = deleteNode(free_root, ret->size,startPointFirstNode);
        free_root = insertTree(free_root, freeSpaceSize, startPointNextNode);
        printf("asdasd  %i\n", freeSpaceSize);
        struct treeNode* freeNode = findNode(free_root,freeSpaceSize,startPointNextNode);
        printf("treeNode: %i\n", freeNode->size);
        //in use node

        struct treeNode* inUseNode = newNode(size, startPointFirstNode, true);
        printf("%li\n", (intptr_t)largeBlock);
        
        //add both to startpoint map
        intptr_t a =(intptr_t)((void*)startPointFirstNode-largeBlock);
        int b=(int)a/4;
        int* sPFN = &b;
        a=(intptr_t)((void*)startPointNextNode-largeBlock);
        int c=(int)a/4;
        int* sPNN =&c;
        a=(intptr_t)((void*)endPointFirstNode-largeBlock);
        int d=(int)a/4;
        int* ePFN =&d;
        a=(intptr_t)((void*)endPointNextNode-largeBlock);
        int e=(int)a/4;
        int* ePNN =&e;

        printf("%i %i %i %i\n", *sPFN, *sPNN, *ePFN,  *ePNN);
        insertMap(startMap, sPFN, inUseNode);
        insertMap(startMap, sPNN, freeNode);

        //add both to endpoint map
        insertMap(endMap, ePFN, inUseNode);
        insertMap(endMap, ePNN, freeNode);
        return startPointFirstNode;
    }
}
void preOrder(struct treeNode *root)
{
    struct treeNode* cRoot=root;
    if(root != NULL)
    {
        printf("%d ", root->size);
        while(cRoot->count){
            cRoot=cRoot->count;
            printf("%d ", cRoot->size);
        }
        preOrder(root->left);
        preOrder(root->right);
    }
    
}

int main(){
    mem_init(100);
    //preOrder(free_root);
    void* p = my_malloc(25);
    //preOrder(free_root);
    //printf("\n");
    void* n=my_malloc(25);
    //printf("%p\n", p);
    
    //printf("%p\n", p+25);
    //printf("%p\n", n);
    //preOrder(free_root);
    //printf("\n");
    printf("Malloc a node of size 30\n");
    void* k=my_malloc(30);
    printf("preorder:\n");
    preOrder(free_root);
    printf("\n%p\n", p);
    //printf("%p\n", p+70);
    //preOrder(free_root);
    printf("freeing:\n");
    //| job p | job n | job k |
    my_free(n);
    printf("preorder:\n");
    preOrder(free_root);
    printf("\n");
    //| job p | free  | job k |
    my_free(p);
    //|      free     | job k |
    //my_free(p);
    
    
    printf("\n");
    printf("preorder:\n");
    preOrder(free_root);
    printf("\n");
}
