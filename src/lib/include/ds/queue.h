#pragma once

#include <sys/sys.h>

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

/* for debugging */
qnode_t* queue_head(queue_t* q);

/* queue api */
#define QUEUE(data_type)                queue_create(sizeof(data_type))
#define QUEUE_FREE(q)                   queue_destroy(q)
#define QUEUE_FRONT(q, data_type)       (queue_front(q) == NULL ? CAST(0, data_type) : *(CAST(queue_front(q), data_type*)))
#define QUEUE_BACK(q, data_type)        (queue_front(q) == NULL ? CAST(0, data_type) : *(CAST(queue_back(q), data_type*)))
#define QUEUE_DEQ(q)                    queue_dequeue(q)
#define QUEUE_ENQ(q, data)              { typeof(data) temp = data; queue_enqueue(q, &temp); }
