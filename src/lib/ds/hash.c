#include <ds/hash.h>

hash_t djb_hash(const void* data, size_t size) {
    uint64_t hash = 5381;
    const uint8_t* ptr = CAST(data, uint8_t*);

    for (size_t i = 0; i < size; ++ptr, ++i) {
        hash = ((hash << 5) + hash) + (*ptr);
    }

    return hash;
}
