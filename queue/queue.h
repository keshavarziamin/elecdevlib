#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef QUEUE_H_
#define QUEUE_H_

#define QUEUE_SIZE_OF_BUFFER 10

typedef enum{
    QUEUE_OK =0,
    QUEUE_NOK = -1,
    QUEUE_ERROR_POINTER=-2,
    QUEUE_ERROR_FULL=-3,
    QUEUE_ERROR_EMPTY=-4
}Queue_TypeStatus;

typedef struct __queue Queue;
struct __queue
{
    int buffer[QUEUE_SIZE_OF_BUFFER];
    int head;
    int size;
    int tail;

    Queue_TypeStatus (*isFull)(Queue* const me);
    Queue_TypeStatus (*isEmpty)(Queue* const me);
    int (*getSize)(Queue* const me);
    Queue_TypeStatus (*remove)(Queue* const me,int *value);
    Queue_TypeStatus (*insert)(Queue* const me,int value);

};

/*constructors and destructores*/
void Queue_Init(Queue* const me, 
                Queue_TypeStatus(*isFull_function)(Queue* const me),
                Queue_TypeStatus (*isEmpty_function)(Queue* const me),
                int              (*getSize_function)(Queue* const me),
                Queue_TypeStatus (*remove_function)(Queue* const me,int *value),
                Queue_TypeStatus (*insert_function)(Queue* const me,int value));

void Queue_Cleanup(Queue* const me);

Queue_TypeStatus Queue_isFull(Queue* const me);
Queue_TypeStatus Queue_isEmpty(Queue* const me);
int              Queue_getSize(Queue* const me);
Queue_TypeStatus Queue_remove(Queue* const me,int *value);
Queue_TypeStatus Queue_insert(Queue* const me,int value);


Queue* Queue_Create(void);
void Queue_Destory(Queue * const me);

#endif /*QUEUE_H_*/