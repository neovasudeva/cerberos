#include <ds/vector.h>

#include <mm/kheap.h>
#include <mem/mem.h>
#include <log.h>

/* returns the address of an element in vector */
#define get_elem_addr(vec, idx) CAST(CAST(vec->buf, uint8_t*) + ((idx) * vec->data_size), void*) 

inline vector_t* vector_create(size_t data_size, size_t capacity) {
    vector_t* vec = CAST(kmalloc(sizeof(vector_t)), vector_t*);
    vec->capacity = capacity;
    vec->size = 0;
    vec->data_size = data_size;
    vec->buf = kmalloc(data_size * vec->capacity);
    return vec;
}

inline void vector_destroy(vector_t* vec) {
    kfree(vec->buf);
    kfree(vec);
}

inline void* vector_get(const vector_t* vec, size_t idx) {
    if (idx >= vec->size) {
        error("[vector_get] idx %lu is out of range. Length of vector: %lu\n", idx, vec->size);
        return NULL;
    }

    return get_elem_addr(vec, idx);
}

inline void vector_set(const vector_t* vec, size_t idx, const void* data) {
    if (idx >= vec->size) {
        error("[vector_set] idx %lu is out of range. Length of vector: %lu\n", idx, vec->size);
        return;
    }

    void* elem = get_elem_addr(vec, idx);
    memcpy(elem, data, vec->data_size);
}

inline void vector_add(vector_t* vec, const void* data) {
    // capacity has been reached, double it
    if (vec->size == vec->capacity) {
        vec->capacity *= 2;
        vec->buf = krealloc(vec->buf, vec->capacity * vec->data_size);
    }

    void* elem = get_elem_addr(vec, vec->size); 
    memcpy(elem, data, vec->data_size);
    vec->size++;
}

inline void vector_remove(vector_t* vec, size_t idx) {
    if (idx >= vec->size) {
        error("[vector_set] idx %lu is out of range. Length of vector: %lu\n", idx, vec->size);
        return;
    }
    
    if (idx != vec->size - 1) {
        void* elem = get_elem_addr(vec, idx);
        void* rest = get_elem_addr(vec, idx + 1);

        memmove(elem, rest, (vec->size - idx - 1) * vec->data_size);
    }

    vec->size--;
}

inline void vector_insert(vector_t* vec, size_t idx, const void* data) {
    // NOTE: idx could be vector's size
    if (idx > vec->size) {
        error("[vector_insert] idx %lu is out of range of vector. Length of vector: %lu\n", idx, vec->size);
        return;
    }

    // capacity has been reached, double it
    if (vec->size == vec->capacity) {
        vec->capacity *= 2;
        vec->buf = krealloc(vec->buf, vec->capacity * vec->data_size);
    }

    void* elem = get_elem_addr(vec, idx);
    if (idx != vec->size) {
        void* next = get_elem_addr(vec, idx + 1);
        memmove(next, elem, (vec->size - idx) * vec->data_size);
    }

    memcpy(elem, data, vec->data_size);
    vec->size++;
}
