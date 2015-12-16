#if HAVE_DDTABLE_CONFIG_H
#include "ddtable_config.h"
#else
#include "../build/config/ddtable_config.h"
#endif

#include "libddtable.h"
#include "spooky-c.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

struct ddtable
{
    //! Absolute number of key-value pairs
    uint_fast32_t num_kv_pairs;
    //! Internal size used for hashing
    uint_fast32_t size;
    //! Fast-checker for key existence
    char* ddtable_RESTRICT exists;     
    //! Single-alloc array for kv pairs
    double key_vals[];
};

//! Default NULL value (not a value) for our table
#define DDTABLE_NULL_VAL 0

//! Checks if x is a power of 2
#define IS_POW2(x) ((x != 0) && ((x & (~x + 1)) == x))

//! Enforces size must be power of 2 minus 1 (i.e. use & instead of %)
#ifndef DDTABLE_ENFORCE_POW2
#define DDTABLE_ENFORCE_POW2 1
#endif

//! Sets the seed we pass to spooky
#ifndef SPOOKY_HASH_SEED
#define SPOOKY_HASH_SEED 0
#endif


// TODO: Support other hash functions?
//! Hash function using spooky 64-bit hash
static inline uint_fast32_t dd_hash(const double key, const uint_fast32_t size)
{
    #if DDTABLE_ENFORCE_POW2
    // Can use faster & instead of % if we enforce power of 2 size.
    return spooky_hash64(&key, sizeof(double), SPOOKY_HASH_SEED) & size;
    #else
    return spooky_hash64(&key, sizeof(double), SPOOKY_HASH_SEED) % size;
    #endif
}

//! Gets the next power of two from the given number (e.g. 30 -> 32)
static uint_fast32_t next_power_of_two(uint_fast32_t n)
{
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;
    return n;
}

ddtable_t ddtable_new(const uint_fast32_t num_keys)
{
    // Set the absolute number of key-value pairs, and also
    // set the internal size depending on whether we enforce
    // "power of 2"-sized tables.
#if DDTABLE_ENFORCE_POW2
    // This minus one trick is necessary for &: http://goo.gl/FlcEb0
    const uint_fast32_t ht_size = next_power_of_two(num_keys) - 1;
    const uint_fast32_t ht_num_kv_pairs = ht_size + 1;
#else
    const uint_fast32_t ht_size = num_keys;
    const uint_fast32_t ht_num_kv_pairs = num_keys;
#endif

    ddtable_t new_ht = malloc(sizeof(struct ddtable) +
                              (sizeof(double) * ht_num_kv_pairs * 2));
    assert(new_ht);
    new_ht->size = ht_size;
    new_ht->num_kv_pairs = ht_num_kv_pairs;

#ifndef NDEBUG
    fprintf(stderr, "Created new ddtable %p with size %"PRIuFAST32"\n",
            (void*) new_ht, new_ht->size);
#endif

    // Allocate space for existance array
    new_ht->exists = calloc(new_ht->num_kv_pairs, sizeof(char));
    assert(new_ht->exists);

    return new_ht;
}

void ddtable_free(ddtable_t ddtable)
{
    if (ddtable != NULL)
    {
        if (ddtable->exists != NULL)
        {
            free(ddtable->exists);
        }
        
        free(ddtable);
    }
}

double ddtable_get_val(ddtable_t ddtable, const double key)
{
    const uint_fast32_t indx = dd_hash(key, ddtable->size);

    return (ddtable->exists[indx]) ? 
        ddtable->key_vals[(2 * indx) + 1] : (double) DDTABLE_NULL_VAL;
}

double ddtable_get_check_key(ddtable_t ddtable, const double key)
{
    const uint_fast32_t indx = dd_hash(key, ddtable->size);

    // If the key exists AND it's equal to the given one,
    // then return the value. Otherwise, return DDTABLE_NULL_VAL
    return (ddtable->exists[indx] && ddtable->key_vals[2 * indx] == key)
        ? ddtable->key_vals[(2 * indx) + 1] : (double) DDTABLE_NULL_VAL;
}

int ddtable_set_val(ddtable_t ddtable, const double key, const double val)
{
    const uint_fast32_t indx = dd_hash(key, ddtable->size);

    if (ddtable->exists[indx])
    {
        return 1; // Collision
    } else {
        ddtable->exists[indx] = '1';
        ddtable->key_vals[2 * indx] = key;
        ddtable->key_vals[(2 * indx) + 1] = val;
        return 0;
    }
}
