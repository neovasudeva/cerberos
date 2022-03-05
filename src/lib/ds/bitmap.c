#include <sys/sys.h>
#include <ds/bitmap.h>

/* 
 * bitmap_find_range
 * finds a range of @param bits with @param length starting from @param first_idx
 * @param bitmap : bitmap to search in
 * @param first_idx : idx to start searching from
 * @param bit : search for range of 1's or 0's
 * @param length : length of @param bit to search for 
 */
size_t bitmap_find_range(bitmap_t* bitmap, size_t first_idx, bool bit, size_t length) {
    size_t found_length = 0;
    size_t found_idx = (size_t) -1;

    for (size_t idx = first_idx; idx < bitmap->size; idx++) {
        if (bitmap_get(bitmap, idx) == bit) {
            if (found_length == 0)
                found_idx = idx;

            found_length++;

        } else {
            found_length = 0;
            found_idx = (size_t) -1; 
        }

        if (found_length == length) 
            return found_idx; 
    }

    return (size_t) -1;
}