#include "queue.h"

int main(int argc, char const *argv[])
{
    int head,tial,value=100;
    Queue_TypeStatus Qst;
    Queue  *queue1,*queue2;
    // printf("hi\r\n");
    queue1=Queue_Create();
    queue2=Queue_Create();
    if (queue1==NULL||queue2==NULL)
        return -1;
    printf("created queues\r\n");
    /*insert data in queue*/
    for(int k=0;k<QUEUE_SIZE_OF_BUFFER;k++){
        
        head=queue1->head;
        
        Qst=queue1->insert(queue1,value);
        if (Qst!=QUEUE_OK && Qst!=QUEUE_ERROR_FULL)
            return -2;

        printf("queue1 --> increasing %d at position %d, size=%d\r\n",
                value,head,queue1->getSize(queue1));
        printf("checking => queue1->buffer[%d]=%d\r\n",head,queue1->buffer[head]);
        value--;        
        
        head=queue2->head;
        Qst=queue2->insert(queue2,value);
        if (Qst!=QUEUE_OK && Qst!=QUEUE_ERROR_FULL)
            return-3;

        printf("queue2 --> increasing %d at position %d, size=%d \r\n",
                value,head,queue2->getSize(queue2));
        printf("checking => queue2->buffer[%d]=%d\r\n",head,queue2->buffer[head]);
        value--;
        
    }
    printf("queue1 [5]-> %d\r\n",queue1->buffer[5]);
    printf("queue2 [7]-> %d\r\n",queue2->buffer[7]);
    printf("is queue1 full? %d\r\n", queue1->isFull(queue1));
    printf("is queue2 full? %d\r\n", queue2->isFull(queue2));

    for (int k=0;k<QUEUE_SIZE_OF_BUFFER;k++){
        tial=queue1->tail;
        Qst=queue1->remove(queue1,&value);
        if(Qst!=QUEUE_OK && Qst!=QUEUE_ERROR_EMPTY)
            return -2;
        printf("queue1--> removing %d at position %d, size=> %d \r\n",
                value,tial,queue1->getSize(queue1));
        printf("checking => queue1->buffer[%d]=%d\r\n",tial,queue1->buffer[tial]);

        
        tial=queue2->tail;
        Qst=queue2->remove(queue2,&value);
        if(Qst!=QUEUE_OK && Qst!=QUEUE_ERROR_EMPTY)
            return -3;
        printf("queue2--> removing %d at position %d, size=> %d\r\n",
                value,tial,queue2->getSize(queue2));
        printf("checking => queue2->buffer[%d]=%d\r\n",tial,queue2->buffer[tial]);
    }

    printf("is queue1 empty? %d\r\n",queue1->isEmpty(queue1));
    printf("is queue2 empty? %d\r\n",queue2->isEmpty(queue2));

    return 0;
}
