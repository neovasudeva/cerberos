#pragma once

#include <sys/sys.h>

/* 
 * Kraken's vector
 * 
 * The vector implementation below is similar to C++'s. A vector stores data in contiguous 
 * heap-allocated memory. When a vector is created, a capacity (or the initial size of the 
 * internal buffer) can be specified to avoid having to repeatedly expand the internal buffer.
 * 
 * Data retrieved from the vector and inserted into the vector are merely COPIES. It is
 * currently not possible to get the data stored within the vector itself. 
 * 
 * When freeing a vector, only the internal buffer and the vector's metadata are freed. Hence,
 * if the vector contains pointers to other heap allocated memory, it will need to be freed manually
 * by the user before calling VECTOR_FREE.
 * 
 */

#define VEC_INITIAL_CAPACITY    CAST(16, size_t)

typedef struct {
    void* buf;
    size_t capacity;
    size_t size;
    size_t data_size;
} vector_t;

vector_t* vector_create(size_t data_size, size_t capacity);
void vector_destroy(vector_t* vec);
void* vector_get(const vector_t* vec, size_t idx);
void vector_set(const vector_t* vec, size_t idx, const void* data);
void vector_add(vector_t* vec, const void* data);
void vector_remove(vector_t* vec, size_t idx);
void vector_insert(vector_t* vec, size_t idx, const void* data);

/* vector api */
#define VECTOR(data_type)                   vector_create(sizeof(data_type), VEC_INITIAL_CAPACITY)
#define VECTOR_CAP(data_type, capacity)     vector_create(sizeof(data_type), capacity)
#define VECTOR_FREE(vec)                    vector_destroy(vec)
#define VECTOR_GET(vec, idx, data_type)     (*CAST(vector_get(vec, idx), data_type*))
#define VECTOR_SET(vec, idx, data)          { typeof(data) temp = data; vector_set(vec, idx, &temp); }
#define VECTOR_ADD(vec, data)               { typeof(data) temp = data; vector_add(vec, &temp); }
#define VECTOR_REMOVE(vec, idx)             vector_remove(vec, idx)
#define VECTOR_INSERT(vec, idx, data)       { typeof(data) temp = data; vector_insert(vec, idx, &temp); }
