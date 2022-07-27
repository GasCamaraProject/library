#include "Queue.h"
#include <stdio.h>
#include <stdlib.h>

//created by Tammar Brand
//on 11.0.2022
//implement queue syncronic functions
Node * createNode(void * data, int size){
    Node* node=(Node*)malloc(sizeof(Node));
    //node->data=malloc(size);
    node->data=data;
    //memcpy(node->data,data,size);
    //  exit(1);
    return node;
}

Queue* createQueue(int capacity){
    Queue *q=(Queue*)malloc(sizeof(Queue));
    initQueue(q,capacity);
    return q;
}
void initQueue(Queue* q,int capacity){
    q->capacity=capacity;
    q->size=0;
    q->head=NULL;
    q->tail=NULL;
    sem_init(&(q->sem),0,1);
    return;
}
void freeNode(Node* node){
    if(node==NULL)
        return;
    free(node->data);
    node->data=0;
    free(node);
    node=0;
}
void freeQueue(Queue* q){
    if(q==NULL)
        return;
    sem_destroy(&q->sem);
    while(!isEmpty(q)){
        freeNode(dequeue(q));
    }
    free(q);
}
int getCapacity(Queue* q){
    return q->capacity;
}
int getSize(Queue* q){
    return q->size;
}
void enqueu(Queue* q,Node* data){

    sem_wait(&q->sem);//lock sem

    if(q->size==q->capacity){
        return;
    }
    if(q->head==NULL){
        q->head=q->tail=data;
    }
    else{
        q->tail->next=data;
        q->tail=data;
    }
    q->size++;

    sem_post(&q->sem);//free sem

}
Node* dequeue(Queue* q){
    Node* temp;
    sem_wait(&q->sem);//lock sem
    if(!isEmpty((q)))
    {
        temp=q->head;
        q->head=q->head->next;
        q->size--;
        if(isEmpty(q))
            q->head=q->tail=NULL;
        sem_post(&q->sem);//free sem
        return temp;
    }
    sem_post(&q->sem);//free sem
    return NULL;
}
int isEmpty(Queue* q){
    return q->size==0;
}
int isFull(Queue *q){
    sem_wait(&q->sem);//lock sem
    int isFull=q->size==q->capacity;
    sem_post(&q->sem);//free sem
    return isFull;
}
