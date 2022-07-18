#pragma once

#include <sys/sys.h>
#include <ds/vector.h>

/* 
 * Kraken's Hashmap
 *
 * Typically, hashmaps require objects to have an equality operator and a hashing operator such 
 * that when 2 objects are considered equal by the equality operator, the hash generated from
 * both objects are also the same.
 * 
 * However, in C, this is a hassle. Kraken's hashmap will assume that "equality" between 2 objects
 * in the hashmap means ALL the data between the objects match. Similarly, hashes for objects 
 * are generated using ALL the bytes of the object.
 * 
 * This solution works, but has some caveats. For example, structs that have a spinlock in them would
 * break Kraken's hashmap because it would include the spinlock when checking for equality and 
 * generating hashes.
 * 
 * Kraken's hashmap implementation is simple, but it comes at the cost of versatility. As a result,
 * one must take extra care when using the hashmap.
 * 
 */

#define MAP_INITIAL_NUM_BUCKETS VEC_INITIAL_CAPACITY

/* pair = 2 pointers to dynamically allocated key and value */
/* NOTE: all pairs are statically allocated */
/* TODO: perhaps make pair it's own data structure */
typedef struct {
    void* key;
    void* val;
} map_pair_t;

#define PAIR(key, key_size, val, val_size)          map_pair_create(key, key_size, val, val_size)
#define PAIR_FREE(pair)                             { kfree(pair.key); kfree(pair.val); }
#define PAIR_COPY(pair, key_size, val_size)         map_pair_create(pair.key, key_size, pair.val, val_size)

/* map_bucket_t = vector of map_pair_t */
typedef vector_t map_bucket_t;

#define BUCKET()                                    VECTOR_CAP(map_pair_t, 2) 
#define BUCKET_GET(bucket, idx)                     VECTOR_GET(bucket, idx, map_pair_t)
#define BUCKET_ADD(bucket, pair)                    VECTOR_ADD(bucket, pair)
#define BUCKET_REMOVE(bucket, idx)                  VECTOR_REMOVE(bucket, idx)                 
#define BUCKET_FIND(bucket, key, key_data_size)     map_bucket_find(bucket, key, key_data_size)
#define BUCKET_FREE(bucket) {                       \
    for (size_t j = 0; j < bucket->size; j++)       \
        PAIR_FREE(BUCKET_GET(bucket, j));           \
    VECTOR_FREE(bucket);                            \
}
// TODO: variable j needs to be unique ^^

/* map_bucket_list_t = vector of map_bucket_t* */
typedef vector_t map_bucket_list_t;     

#define BUCKET_LIST(num_buckets)                    map_create_bucket_list(num_buckets)
#define BUCKET_LIST_GET(bucket_list, idx)           VECTOR_GET(bucket_list, idx, map_bucket_t*) 
#define BUCKET_LIST_ADD(bucket_list, bucket)        VECTOR_ADD(bucket_list, bucket)
#define BUCKET_LIST_FREE(bucket_list) {             \
    for (size_t i = 0; i < bucket_list->size; i++)  \
        BUCKET_FREE(                                \
            BUCKET_LIST_GET(bucket_list, i)         \
        );                                          \
    VECTOR_FREE(bucket_list);                       \
}               
// TODO: variable i needs to be unique ^^

/* map_t struct */
typedef struct {
    map_bucket_list_t* bucket_list;
    size_t size;
    size_t key_data_size;
    size_t val_data_size;
} map_t;

map_t* map_create(size_t key_data_size, size_t val_data_size);
void map_destroy(map_t* map);
void map_insert(map_t* map, void* key, void* val, bool replace);
void* map_get(map_t* map, void* key);
void map_remove(map_t* map, void* key);

/* map api */
#define MAP(key_type, val_type)                     map_create(sizeof(key_type), sizeof(val_type))
#define MAP_FREE(map)                               map_destroy(map)
#define MAP_INSERT(map, key, val) {                 \
    typeof(key) temp_key = key;                     \
    typeof(val) temp_val = val;                     \
    map_insert(map, &temp_key, &temp_val, true);    \
}
#define MAP_PUT(map, key, val)                      MAP_INSERT(map, key, val)
#define MAP_INSERT_NO_REPLACE(map, key, val) {      \
    typeof(key) temp_key = key;                     \
    typeof(val) temp_val = val;                     \
    map_insert(map, &temp_key, &temp_val, false);   \
}
#define MAP_PUT_NO_REPLACE(map, key, val)           MAP_INSERT_NO_REPLACE(map, key, val)
#define MAP_GET(map, key, val_data_type) ({         \
    typeof(key) temp_key = key;                     \
    *CAST(map_get(map, &temp_key), val_data_type*); \
})
#define MAP_REMOVE(map, key) {                      \
    typeof(key) temp_key = key;                     \
    map_remove(map, &temp_key);                     \
}
