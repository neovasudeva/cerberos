#include <ds/queue.h>
#include <mm/kheap.h>
#include <mem/mem.h>
#include <log.h>

/* 
 * -----> head -> ... -> tail ----->
 *   ^                             ^
 *   |                             |
 *  back                         front
 */

queue_t* queue_create(size_t data_size) {
    queue_t* q = CAST(kmalloc(sizeof(queue_t)), queue_t*);
    q->head = NULL;
    q->tail = NULL;
    q->data_size = data_size;
    q->size = 0;

    return q;
}

void queue_destroy(queue_t* q) {
    qnode_t* temp = q->head;
    for (size_t i = 0; i < q->size; i++) {
       q->head = q->head->next;
       kfree(temp->data);
       kfree(temp);
       temp = q->head;
    }

    kfree(q);
}

void* queue_front(const queue_t* q) {
    if (q->tail == NULL) {
        if (q->size != 0)
            error("[queue_front] access attempt on non-empty queue with no tail. Size of queue: %lu\n", q->size);

        return NULL;
    }

    return q->tail->data;
}

void* queue_back(const queue_t* q) {
    if (q->head == NULL) {
        if (q->size != 0)
            error("[queue_back] access attempt on non-empty queue with no head. Size of queue: %lu\n", q->size);

        return NULL;
    }

    return q->head->data;
}

void queue_dequeue(queue_t* q) {
    if (q->tail == NULL) {
        error("[queue_dequeue] dequeue attempt on queue with no tail. Size of queue: %lu\n", q->size);
        return;
    }

    qnode_t* prev = q->tail->prev;

    if (prev != NULL)
        prev->next = NULL;

    kfree(q->tail);
    q->tail = prev;
    q->size--;
}

void queue_enqueue(queue_t* q, void* data) {
    qnode_t* node = CAST(kmalloc(sizeof(qnode_t)), qnode_t*);
    node->prev = NULL;
    node->data = kmalloc(q->data_size);
    memcpy(node->data, data, q->data_size);
    q->size++;

    if (q->head == NULL) {
        node->next = NULL;
        q->head = node;
        q->tail = node;
        return;
    }

    node->next = q->head;
    q->head->prev = node;
    q->head = node;
}

qnode_t* queue_head(queue_t* q) {
    return q->head;
}