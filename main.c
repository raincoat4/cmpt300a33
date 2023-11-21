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
    
    printf("value of this startpoint: %i\n", *thisStartPoint);
    //printf("%i\n", *tmp);
    struct treeNode* memToFree = (struct treeNode*)search(startMap, thisStartPoint);
    if(!memToFree){
        printf("this should be impossible\n");
        return;
    }

    int x=b+memToFree->size-1;
    int *thisEndPoint=&x;
    printf("endpoint %i\n", *thisEndPoint);
    int sizeOfNode= memToFree->size;
    printf("initial size of node %i\n", sizeOfNode);
    //this is startpoint of the node we want to free
    
    intptr_t tmpb= (intptr_t)(memToFree->size)+b;
    int tmpc= (int)tmpb;
    int* nextStartPoint = &tmpc;
    //might need to assign to void ptr first
    //printf("tmpc %i\n", tmpc);
    struct treeNode* nodeInFront = (struct treeNode*)search(startMap, nextStartPoint);
    //nodeInFront is never NULL since the hashmap always has nodes whether the space is being used or not
    bool front=false;
    int fSize=0;
    if(nodeInFront){
        printf(" node in front size:%i\n", nodeInFront->size);
        if(!(nodeInFront->inUse)&&nodeInFront->size>0){
            front=true;
            sizeOfNode = sizeOfNode+ nodeInFront->size;
            fSize=nodeInFront->size;
            //remove the node in front from all structures because its gonna combine with the one we are pointing to
            //delete from hashmap first because the tree is gonna free the memory
            
            //might be size - 1 idk
            int* nextEndPoint = *nextStartPoint + &nodeInFront->size;
            delete(startMap, nextStartPoint);
            delete(endMap, nextEndPoint);
            
            free_root = deleteNode(free_root, nodeInFront->size, nodeInFront->start);    
        }else{
            printf("front node in use\n");
        }
    }
    //this will either be just the size of the node we wanna free or the size of the node we wanna free + node in front
    
    //if nodeInFront is free memory

    int tmpe= b-1;
    int* backEndPoint = &tmpe;
    int* backStartPoint;
    int bSize=0;
    printf("tmpe %i\n", tmpe);
    //might need to assign to void ptr first
    bool back=false;
    struct treeNode* nodeInBack=NULL;
    if(!(tmpe<=0)&&tmpe!=largeBlockSize){
        nodeInBack = (struct treeNode*)search(endMap, backEndPoint);
        printf("%i\n", nodeInBack==NULL);
        if(nodeInBack){
            printf("size: %i\n", nodeInBack->size);
            if(!(nodeInBack->inUse)){
                sizeOfNode = sizeOfNode+ nodeInBack->size;
                bSize=nodeInBack->size;
                //remove the node in back from all structures because its gonna combine with the one we are pointing to
                //delete from hashmap first because the tree is gonna free the memory
                
                
                //might be size - 1 idk
                backStartPoint = *backEndPoint + &nodeInBack->size;
                delete(endMap, backEndPoint);
                delete(startMap, backStartPoint);
                
                free_root = deleteNode(free_root, nodeInBack->size, nodeInBack->start);
                back=true;
                printf("sizeOfNode+backnode %i\n", sizeOfNode);

            }else{
                printf("node in use\n");
            }
        }
    }
    //if nodeInFront&nodeInBack is being used then just add the node to free to the tree
    delete(startMap,thisStartPoint);
    delete(endMap, thisEndPoint);
    
    struct treeNode *inserting = NULL;
    printf("sizeOfNode: %d\n", sizeOfNode);
    int insertingtmp;
    free_root = insertTree(free_root, sizeOfNode, backStartPoint);
    inserting = findNode(free_root, sizeOfNode, backStartPoint);
    preOrder(free_root);
    printf("\n");
    if(back&&front){
        insertingtmp=tmpe-bSize;
        insertMap(startMap, &insertingtmp, inserting);
        printf("\ttmp bf %i\n", insertingtmp);
        insertingtmp=tmpc+fSize;
        insertMap(endMap, &insertingtmp,inserting);
        printf("\ttmp bf %i\n", insertingtmp);
    }else if(front){
        
        inserting = findNode(free_root, tmpc, thisStartPoint);
        printf("\ttmp f %i\n", tmpc);
        insertMap(startMap, &b, inserting);
        insertingtmp=tmpc+fSize;
        insertMap(endMap, &insertingtmp,inserting);
        printf("\ttmp f %i\n", insertingtmp);
    }else if(back){
        printf("nodeinback size%i\n", bSize);
        printf("tmpe%i\n", tmpe);
        insertingtmp=tmpe-(bSize)+1;
        printf("\ttmp b %i\n", insertingtmp);
        insertMap(startMap, &insertingtmp, inserting);
        insertingtmp=b+memToFree->size-1;
        printf("\ttmp b %i\n", insertingtmp-1);
        insertMap(endMap, &insertingtmp, inserting);
    }else{
        insertingtmp=b+memToFree->size;
        insertingtmp--;
        inserting = findNode(free_root, sizeOfNode, thisStartPoint);
        printf("\ttmp %i\n", insertingtmp);
        printf("\ttmp %i\n", b);
        insertMap(startMap, &b, inserting);
        insertMap(endMap, &insertingtmp,inserting);
    }
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
        //printf("asdasd  %i\n", freeSpaceSize);
        
        struct treeNode* freeNode = findNode(free_root,freeSpaceSize,startPointNextNode);
        //printf("treeNode: %i\n", freeNode->size);
        //in use node

        struct treeNode* inUseNode = newNode(size, startPointFirstNode, true);
        //printf("%li\n", (intptr_t)largeBlock);
        
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
        //printf("%i %i %i %i\n", *sPFN, *sPNN, *ePFN,  *ePNN);
        insertMap(startMap, sPFN, inUseNode);
        insertMap(startMap, sPNN, freeNode);

        //add both to endpoint map
        insertMap(endMap, ePFN, inUseNode);
        insertMap(endMap, ePNN, freeNode);
        return startPointFirstNode;
    }
}

int main(){
    mem_init(100);
    //preOrder(free_root);
    void* a = my_malloc(20);
    //preOrder(free_root);
    //printf("\n");
    void* b=my_malloc(25);
    //printf("%p\n", p);
    
    //printf("%p\n", p+25);
    //printf("%p\n", n);
    //preOrder(free_root);
    //printf("\n");
    printf("Malloc a node of size 30\n");
    void* c=my_malloc(38);
    void*d=my_malloc(17);
    printf("preorder:\n");
    preOrder(free_root);
    printf("\n");
    //printf("%p\n", p+70);
    //preOrder(free_root);
    printf("freeing a:\n");
    //| job p | job n | job k |
    my_free(a);
    printf("preorder1:\n");
    preOrder(free_root);
    printf("\n");
    //| job p | job n | free |
    printf("freeing b:\n");
    my_free(b);
    printf("preorder2:\n");
    preOrder(free_root);
    printf("\n");
    printf("freeing c:\n");
    my_free(c);
    //|      free     | job k |
    //my_free(p);
    
    
    printf("\n");
    printf("preorder3:\n");
    preOrder(free_root);
    printf("\n");
    my_free(d);
    printf("preorder4:\n");
    preOrder(free_root);
    printf("\n");
}
