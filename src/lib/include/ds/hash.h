#pragma once

#include <sys/sys.h>

/* 
 * Good resource on hash functions
 * https://www.partow.net/programming/hashfunctions/
 */

typedef uint64_t hash_t;

hash_t djb_hash(const void* data, size_t size);