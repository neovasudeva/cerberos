#include <ds/vector.h>

#include <mm/kheap.h>
#include <mem/mem.h>
#include <log.h>

/* returns the address of an element in vector */
#define get_elem_addr(vec, idx) CAST(CAST(vec->buf, uint8_t*) + ((idx) * vec->data_size), void*) 

/* 
 * vector_create 
 * dynamically allocates a vector with specified capacity holding 
 * objects of data_size
 * @param data_size : size of objects in bytes that the vector will hold
 * @param capacity : initial capacity of the vector
 * @returns pointer to newly created vector
 */
inline vector_t* vector_create(size_t data_size, size_t capacity) {
    vector_t* vec = CAST(kmalloc(sizeof(vector_t)), vector_t*);
    vec->capacity = capacity;
    vec->size = 0;
    vec->data_size = data_size;
    vec->buf = kmalloc(data_size * vec->capacity);
    return vec;
}

/* 
 * vector_destroy
 * frees a dynamically allocated vector
 * @param vec : pointer to vector to destroy
 */
inline void vector_destroy(vector_t* vec) {
    kfree(vec->buf);
    kfree(vec);
}

/* 
 * vector_get
 * retrieves the data from a vector
 * @param vec : vector to retrieve data from
 * @param idx : index of object to return
 * @returns a pointer to the object
 */
inline void* vector_get(const vector_t* vec, size_t idx) {
    if (idx >= vec->size) {
        error("[vector_get] idx %lu is out of range. Length of vector: %lu\n", idx, vec->size);
        return NULL;
    }

    return get_elem_addr(vec, idx);
}

/*
 * vector_set
 * sets an index in vector to a new object
 * @param vec : vector to set in
 * @param idx : index within vector to set new object
 * @param data : pointer to new object to put in vec at index idx
 */
inline void vector_set(const vector_t* vec, size_t idx, const void* data) {
    if (idx >= vec->size) {
        error("[vector_set] idx %lu is out of range. Length of vector: %lu\n", idx, vec->size);
        return;
    }

    void* elem = get_elem_addr(vec, idx);
    memcpy(elem, data, vec->data_size);
}

/* 
 * vector_add
 * adds element to the end of a vector 
 * @param vec : vector to add object in
 * @param data : pointer to object to add into vector
 */
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

/* 
 * vector_remove
 * removes element from vector
 * @param vec : vector to remove element from
 * @param idx : index of element to remove
 */
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

/* 
 * vector_insert
 * inserts element at a specific position in the vector
 * @param vec : vector to insert element in
 * @param idx ; index to insert element at
 * @param data : pointer of object to insert
 */
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
