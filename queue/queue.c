#include "queue.h"
void Queue_Init(Queue* const me, 
                Queue_TypeStatus (*isFull_function)(Queue* const me),
                Queue_TypeStatus (*isEmpty_function)(Queue* const me),
                int (*getSize_function)(Queue* const me),
                Queue_TypeStatus (*remove_function)(Queue* const me, int *value),
                Queue_TypeStatus (*insert_function)(Queue* const me,int value)
                ){

        
        /*Initialize attriburtes*/
        me->head=0;
        me->size=0;
        me->tail=0;

        me->isEmpty=isEmpty_function;
        me->isFull=isFull_function;
        me->getSize=getSize_function;
        me->insert= insert_function;
        me->remove= remove_function;
}

void Queue_cleanup(Queue* const me){
    if(me!=NULL)
        memset(me,0,sizeof(Queue));
};

Queue_TypeStatus Queue_isFull(Queue* const me){
    
    /*check pointer*/
    if (me==NULL)
        return QUEUE_ERROR_POINTER;
    return ((me->head+1)%QUEUE_SIZE_OF_BUFFER==me->tail) ? QUEUE_OK:QUEUE_NOK;
}



Queue_TypeStatus Queue_isEmpty(Queue* const me){
    if (me==NULL)
        return QUEUE_ERROR_POINTER;

    return (me->head==me->tail) ? QUEUE_OK:QUEUE_NOK;
}

int Queue_getSize(Queue* const me){
    /*check pointer*/
    if (me==NULL)
        return QUEUE_ERROR_POINTER;

    return me->size;
}
Queue_TypeStatus Queue_insert(Queue* const me, int value){
    /*check pointer*/
    if (me==NULL)
        return QUEUE_ERROR_POINTER; // return error of pointer
    /* */
    if(me->isFull(me)==QUEUE_OK)
        return QUEUE_ERROR_FULL; // return the queue is full 

    me->buffer[me->head]=value;//insert value into buffer
    me->head=(me->head+1)%QUEUE_SIZE_OF_BUFFER; //increse index of head
    me->size++;
    return QUEUE_OK; 
}

Queue_TypeStatus Queue_remove(Queue* const me, int *value){
    
    *value=0xFFFFFFFF;
    if (me==NULL)
        return QUEUE_ERROR_POINTER; // return error of pointer
    /* */
    if(me->isEmpty(me)==QUEUE_OK)
        return QUEUE_ERROR_EMPTY; // return the queue is empty 

    *value=me->buffer[me->tail]; //return value
    me->tail=(me->tail+1)%QUEUE_SIZE_OF_BUFFER; 
    me->size--;
    return QUEUE_OK;
}


Queue* Queue_Create(void){
    Queue* me=(Queue*)malloc(sizeof(Queue)); //create heap memory pointer
    
    /*check pointer of memory*/
    if(me!=NULL){
        /* create an object*/
        Queue_Init(me,Queue_isFull,Queue_isEmpty,
        Queue_getSize,Queue_remove,Queue_insert);
    }
    return me;   
} 

void Queue_Destory(Queue* const me){
    if (me!=NULL)
        Queue_cleanup(me);
    free(me);
}