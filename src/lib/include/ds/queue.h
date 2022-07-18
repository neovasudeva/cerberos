#pragma once

#include <sys/sys.h>

/* 
 * Kraken's queue
 * 
 * The queue implementation below is similar to C++'s. A queue stores data in a linked list 
 * allocated on the heap. The queue is not as cache-efficient as the vector, but avoids 
 * having to continually resize like the vector.
 * 
 * Data retrieved from the queue and inserted into the queue are merely COPIES. It is
 * currently not possible to get the data stored within the queue itself. 
 * 
 * When freeing a queue, only the internal linked list nodes and the queue's metadata are freed. Hence,
 * if the queue contains pointers to other heap allocated memory, it will need to be freed manually
 * by the user before calling QUEUE_FREE.
 * 
 * If the queue is empty, QUEUE_FRONT and QUEUE_BACK will return 0 casted to whatever data type
 * the queue holds.
 * 
 */

typedef struct __qnode_t {
    struct __qnode_t* next;
    struct __qnode_t* prev;
    void* data;
} qnode_t;

typedef struct {
    qnode_t* head;
    qnode_t* tail;
    size_t data_size;
    size_t size;
} queue_t;

queue_t* queue_create(size_t data_size);
void queue_destroy(queue_t* q);
void* queue_front(const queue_t* q);
void* queue_back(const queue_t* q);
void queue_dequeue(queue_t* q);
void queue_enqueue(queue_t* q, void* data);

/* queue api */
#define QUEUE(data_type)                queue_create(sizeof(data_type))
#define QUEUE_FREE(q)                   queue_destroy(q)
#define QUEUE_FRONT(q, data_type)       (queue_front(q) == NULL ? CAST(0, data_type) : *(CAST(queue_front(q), data_type*)))
#define QUEUE_BACK(q, data_type)        (queue_front(q) == NULL ? CAST(0, data_type) : *(CAST(queue_back(q), data_type*)))
#define QUEUE_DEQ(q)                    queue_dequeue(q)
#define QUEUE_ENQ(q, data)              { typeof(data) temp = data; queue_enqueue(q, &temp); }
