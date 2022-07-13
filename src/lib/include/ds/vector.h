#pragma once

#include <sys/sys.h>

#define VEC_INITIAL_CAPACITY    16

typedef struct {
    void* buf;
    size_t capacity;
    size_t size;
    size_t data_size;
} vector_t;

vector_t* vector_create(size_t data_size);
void vector_destroy(vector_t* vec);
void* vector_get(const vector_t* vec, size_t idx);
void vector_set(const vector_t* vec, size_t idx, const void* data);
void vector_add(vector_t* vec, const void* data);
void vector_remove(vector_t* vec, size_t idx);
void vector_insert(vector_t* vec, size_t idx, const void* data);

/* vector api */
#define VECTOR(data_type)                   vector_create(sizeof(data_type))
#define VECTOR_FREE(vec)                    vector_destroy(vec)
#define VECTOR_GET(vec, idx, data_type)     (*CAST(vector_get(vec, idx), data_type*))
#define VECTOR_SET(vec, idx, data)          { typeof(data) temp = data; vector_set(vec, idx, &temp); }
#define VECTOR_ADD(vec, data)               { typeof(data) temp = data; vector_add(vec, &temp); }
#define VECTOR_REMOVE(vec, idx)             vector_remove(vec, idx)
#define VECTOR_INSERT(vec, idx, data)       { typeof(data) temp = data; vector_insert(vec, idx, &temp); }
