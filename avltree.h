#ifndef _AVLTREE_H_
#define _AVLTREE_H_
#include <stdbool.h>
typedef struct Node Node;
struct treeNode 
{ 
    int size; //size

    int startPoint;
    //int endPoint;
    struct treeNode *left; 
    struct treeNode *right; 
    struct treeNode *count;
    int* start;
    int height;
    bool inUse;

}; 
void* getEnd(struct treeNode *N);
void* getStart(struct treeNode *N);
int getSize(struct treeNode *N);
int height(struct treeNode *N);
int max(int a, int b);
struct treeNode* newNode(int size,void* start, bool beingUsed);
struct treeNode *rightRotate(struct treeNode *y);
struct treeNode *leftRotate(struct treeNode *x);
int getBalance(struct treeNode *N);
struct treeNode* insertTree(struct treeNode* node, int size,void* start);
struct treeNode * minValueNode(struct treeNode* node);
struct treeNode* deleteCountNode(struct treeNode* root, void* start);
struct treeNode* deleteNode(struct treeNode* root, int size, void* start);
struct treeNode* findNode(struct treeNode* root, int size, void* start);
struct treeNode* bestFit(struct treeNode* root, int size);
void* helperBF(struct treeNode* root, int size,struct treeNode* bFn);


#endif