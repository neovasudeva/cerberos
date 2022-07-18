#include <ds/queue.h>
#include <mm/kheap.h>
#include <mem/mem.h>
#include <log.h>

/* 
 *  ----------------------------
 * -----> head -> ... -> tail ----->
 *  ----------------------------
 *   ^                             ^
 *   |                             |
 *  back                         front
 */

/* 
 * queue_create
 * creates a new queue
 * @param data_size : size in bytes of objects to be stored in queue
 * @returns pointer to new queue
 */
queue_t* queue_create(size_t data_size) {
    queue_t* q = CAST(kmalloc(sizeof(queue_t)), queue_t*);
    q->head = NULL;
    q->tail = NULL;
    q->data_size = data_size;
    q->size = 0;

    return q;
}

/* 
 * queue_destroy
 * frees a queue
 * @param q : queue to free
 */ 
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

/* 
 * queue_front
 * gets the element at the front of the queue
 * @param q : queue to get front element from
 */
void* queue_front(const queue_t* q) {
    if (q->tail == NULL) {
        if (q->size != 0)
            error("[queue_front] access attempt on non-empty queue with no tail. Size of queue: %lu\n", q->size);

        return NULL;
    }

    return q->tail->data;
}

/* 
 * queue_back
 * gets the element at the back of the queue
 * @param q : queue to get back element from
 */
void* queue_back(const queue_t* q) {
    if (q->head == NULL) {
        if (q->size != 0)
            error("[queue_back] access attempt on non-empty queue with no head. Size of queue: %lu\n", q->size);

        return NULL;
    }

    return q->head->data;
}

/* 
 * queue_dequeue
 * dequeues an element from the queue
 * @param q : queue to dequeue from
 */
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

/* 
 * queue_enqueue
 * enqueues an element from the queue
 * @param q : queue to enqueue from
 */
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
