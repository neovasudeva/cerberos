#include <ds/map.h>
#include <ds/hash.h>
#include <mm/kheap.h>
#include <mem/mem.h>
#include <log.h>

/* miscellaneous utility macros */
#define map_needs_resize(map)       (map->size + 1 > ((map->bucket_list->size >> 2) + (map->bucket_list->size >> 1))) // = map->bucket_list->size * 0.75

/*
 * map_pair_create
 * statically allocates a key/val pair
 * @param key : key to create pair with
 * @param key_data_size : size of key in bytes
 * @param val : val to create pair with 
 * @param val_data_size : size of val in bytes
 * @returns a pair
 */
static inline map_pair_t map_pair_create(void* key, size_t key_data_size, void* val, size_t val_data_size) {
    map_pair_t pair = {
        .key = kmalloc(key_data_size),
        .val = kmalloc(val_data_size)
    };

    memcpy(pair.key, key, key_data_size);
    memcpy(pair.val, val, val_data_size);

    return pair;
}

/* 
 * map_create_bucket_list
 * will create the vector of bucket pointers with size of num_buckets. It will 
 * initialize these pointers to point to empty buckets.
 * @param num_buckets : number of bucket pointers to create in bucket pointer vector
 * @returns a vector of num_buckets size with all bucket pointers pointing to empty buckets
 */
static inline map_bucket_list_t* map_create_bucket_list(size_t num_buckets) {
    map_bucket_list_t* bucket_list = VECTOR_CAP(map_bucket_t*, num_buckets);

    for (size_t i = 0; i < num_buckets; i++) 
        BUCKET_LIST_ADD(bucket_list, BUCKET());

    return bucket_list;
}

/* map_bucket_find
 * searches a bucket for a matching key. 
 * @param bucket : bucket to search for match in
 * @param key : key to search within the bucket
 * @param key_data_size : size of key in bytes
 * @returns index of found element or CAST(-1, size_t)
 */
static inline size_t map_bucket_find(map_bucket_t* bucket, void* key, size_t key_data_size) {
    for (size_t i = 0; i < bucket->size; i++) {
        if (memcmp(key, BUCKET_GET(bucket, i).key, key_data_size) == 0)
            return i;
    }

    return CAST(-1, size_t);
}

/*
 * map_resize
 * resizes the map by doubling the number of buckets and rehashing all the key/value
 * pairs into the new buckets
 * @param map : map to resize
 */
static void map_resize(map_t* map) {
    // create new bucket list to replace map's current one
    map_bucket_list_t* new_bucket_list = BUCKET_LIST(map->bucket_list->size * 2);

    // rehash each key/val pair into new map
    for (size_t i = 0; i < map->bucket_list->size; i++) {
        map_bucket_t* bucket = BUCKET_LIST_GET(map->bucket_list, i);

        for (size_t j = 0; j < bucket->size; j++) {
            map_pair_t kv = BUCKET_GET(bucket, j);
            hash_t hash = djb_hash(kv.key, map->key_data_size) % new_bucket_list->size;
            kv = PAIR_COPY(kv, map->key_data_size, map->val_data_size);

            map_bucket_t* new_bucket = BUCKET_LIST_GET(new_bucket_list, hash);
            BUCKET_ADD(new_bucket, kv);
        }
    }

    // free old bucket list and assign new one
    BUCKET_LIST_FREE(map->bucket_list);
    map->bucket_list = new_bucket_list;
}

/* 
 * map_create
 * dynamically allocates and returns a map
 * @param key_data_size : size of key in bytes
 * @param val_data_size : size of val in bytes
 */
inline map_t* map_create(size_t key_data_size, size_t val_data_size) {
    map_t* map = CAST(kmalloc(sizeof(map_t)), map_t*);
    map->size = 0;
    map->key_data_size = key_data_size;
    map->val_data_size = val_data_size;
    map->bucket_list = BUCKET_LIST(MAP_INITIAL_NUM_BUCKETS);
    
    return map;
}

/* 
 * map_destroy
 * frees a dynamically allocated map
 * @param map : map to free
 */
inline void map_destroy(map_t* map) {
    BUCKET_LIST_FREE(map->bucket_list);
    kfree(map);
}

/* 
 * map_insert 
 * inserts key/val pair into the map
 * @param map : map to insert pair
 * @param key : key of key/val pair
 * @param val : val of key/val pair
 * @param replace : true if new key/val pair should replace an existing entry with same key, 
 *      false if not (will print error message of there is an existing entry with same key)
 */
void map_insert(map_t* map, void* key, void* val, bool replace) {
    // check if resize is needed
    if (map_needs_resize(map))
        map_resize(map);

    // hash the key
    hash_t hash = djb_hash(key, map->key_data_size) % map->bucket_list->size;
    map_bucket_t* bucket = BUCKET_LIST_GET(map->bucket_list, hash);
    size_t idx = map_bucket_find(bucket, key, map->key_data_size);

    if (idx == CAST(-1, size_t)) {
        map_pair_t pair = PAIR(key, map->key_data_size, val, map->val_data_size);
        BUCKET_ADD(bucket, pair);
    } else {
        if (replace) {
            // TODO: fix, this is such a bad way to replace the pair. Might be better to just use a linked list
            map_pair_t old_pair = BUCKET_GET(bucket, idx);
            PAIR_FREE(old_pair);
            BUCKET_REMOVE(bucket, idx);
            map_pair_t pair = PAIR(key, map->key_data_size, val, map->val_data_size);
            BUCKET_ADD(bucket, pair);
        } else {
            warning("[map_insert] encountered insert attempt with no replace and the key already exists in the map. Insertion failed.\n");
            return;
        }
    }

    map->size++;
}

/* 
 * map_get 
 * returns a value given a key
 * @param map : map to search in
 * @param key : key to find value with
 * @returns pointer to val or NULL if key is not found
 */
void* map_get(map_t* map, void* key) {
    // hash the key
    hash_t hash = djb_hash(key, map->key_data_size) % map->bucket_list->size;
    map_bucket_t* bucket = BUCKET_LIST_GET(map->bucket_list, hash);
    size_t idx = map_bucket_find(bucket, key, map->key_data_size);

    if (idx == CAST(-1, size_t)) 
        return NULL;
    else
        return BUCKET_GET(bucket, idx).val;
}

/* 
 * map_remove
 * removes a key/val pair from the map
 * @param map : map to search for key/val pair in 
 * @param key : key of key/val pair to remove
 */
void map_remove(map_t* map, void* key) {
    // hash the key
    hash_t hash = djb_hash(key, map->key_data_size) % map->bucket_list->size;
    map_bucket_t* bucket = BUCKET_LIST_GET(map->bucket_list, hash);
    size_t idx = map_bucket_find(bucket, key, map->key_data_size);
    
    if (idx == CAST(-1, size_t)) 
        warning("[map_remove] attempt to remove from map, but pair was not found with specified key\n");
    else {
        BUCKET_REMOVE(bucket, idx);
        map->size--;
    }
}
