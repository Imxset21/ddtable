#if HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef __clang__
// For primary build (I use only gcc for production)
#include <libddtable.h>
#include <spooky-c.h>

#else
// For Flycheck (which uses clang)
#include "../include/libddtable.h"
#include "../common/spooky-c.h"
#define HAVE_STDINT_H 1

#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#if HAVE_STDINT_H
#include <stdint.h>
#elif HAVE_INTTYPES_H
#include <inttypes.h>
#else
#error libddtable requires a definition of uint64_t
#endif

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

typedef struct ddtable
{
    uint64_t num_kv_pairs;     //! Absolute number of key-value pairs
    uint64_t size;             //! Internal size used for hashing
    char* restrict exists;     //! Fast-checker for key existence
    double* restrict key_vals; //! Single-alloc array for kv pairs

} ddtable;

// TODO: Support other hash functions?

//! Hash function using spooky 64-bit hash
static inline uint64_t dd_hash(const double key, const uint64_t size)
{
    // Can use faster & instead of % if we enforce power of 2 size.
    #if DDTABLE_ENFORCE_POW2
    return spooky_hash64(&key, sizeof(double), SPOOKY_HASH_SEED) & size;
    #else
    return spooky_hash64(&key, sizeof(double), SPOOKY_HASH_SEED) % size;
    #endif
}

//! Gets the next power of two from the given number (e.g. 30 -> 32)
static inline uint64_t nextPowerOf2(uint64_t n)
{
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32;
    n++;
    return n;
}

ddtable_t new_ddtable(const uint64_t num_keys)
{
    ddtable_t new_ht = (ddtable_t) malloc(sizeof(struct ddtable));
    assert(new_ht);

    // Set the absolute number of key-value pairs, and also
    // set the internal size depending on whether we enforce
    // "power of 2"-sized tables.
    #if DDTABLE_ENFORCE_POW2
    // This minus one trick is necessary for &: http://goo.gl/FlcEb0
    new_ht->size = nextPowerOf2(num_keys) - 1;
    new_ht->num_kv_pairs = new_ht->size + 1;
    #else
    new_ht->num_kv_pairs = num_keys;
    new_ht->size = num_keys;
    #endif

    // Allocate space for key-value store and existance arrays.
    new_ht->key_vals = (double*) calloc(new_ht->num_kv_pairs*2, sizeof(double));
    assert(new_ht->key_vals);
    new_ht->exists = (char*) calloc(new_ht->num_kv_pairs, sizeof(char));
    assert(new_ht->exists);

    return new_ht;
}

void free_ddtable(ddtable_t ddtable)
{
    if (ddtable != NULL)
    {
        if (ddtable->exists != NULL)
        {
            free(ddtable->exists);
        }
        
        if (ddtable->key_vals != NULL)
        {
            free(ddtable->key_vals);
        }
        
        free(ddtable);
    }
}

double ddtable_get_val(ddtable_t ddtable, const double key)
{
    const uint64_t indx = dd_hash(key, ddtable->size);

    return (ddtable->exists[indx]) ? 
        ddtable->key_vals[(2*indx)+1] : (double) DDTABLE_NULL_VAL;
}

double ddtable_get_val_check_key(ddtable_t ddtable, const double key)
{
    const uint64_t indx = dd_hash(key, ddtable->size);

    // If the key exists AND it's memcmp-identical to the given one,
    // then return the value. Otherwise, return DDTABLE_NULL_VAL
    return (ddtable->exists[indx] && 
            0 == memcmp(&ddtable->key_vals[(2*indx)], &key, sizeof(double))) 
        ? ddtable->key_vals[(2*indx)+1] : (double) DDTABLE_NULL_VAL;
}

int ddtable_set_val(ddtable_t ddtable, const double key, const double val)
{
    const uint64_t indx = dd_hash(key, ddtable->size);

    if (ddtable->exists[indx])
    {
        return 1; // Collision
    } else {
        ddtable->exists[indx] = '1';
        ddtable->key_vals[2*indx] = key;
        ddtable->key_vals[(2*indx)+1] = val;
        return 0;
    }
}

void ddtable_update_val(ddtable_t ddtable, const double key, const double val)
{
    const uint64_t indx = dd_hash(key, ddtable->size);

    ddtable->exists[indx] = '1';
    ddtable->key_vals[2*indx] = key;
    ddtable->key_vals[(2*indx)+1] = val;

    return;
}
