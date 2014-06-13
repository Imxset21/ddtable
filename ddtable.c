#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <inttypes.h>

#include "ddtable.h"
#include "spooky-c.h"

#ifndef SPOOKY_HASH_SEED
#define SPOOKY_HASH_SEED 0
#endif


// TODO: Support other hash functions?
//! Hash function using spooky 64-bit hash
static inline uint64_t dd_hash(const double key, const uint64_t size)
{
    #if DDTABLE_ENFORCE_POW2
    // Can use faster & instead of % if we enforce power of 2 size.
    return spooky_hash64(&key, sizeof(double), SPOOKY_HASH_SEED) & size;
    #else
    return spooky_hash64(&key, sizeof(double), SPOOKY_HASH_SEED) % size;
    #endif
}

//! Gets the next power of two from the given number (e.g. 30 -> 32)
static uint64_t nextPowerOf2(uint64_t n)
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

double get_val(ddtable_t ddtable, const double key)
{
    const uint64_t indx = dd_hash(key, ddtable->size);

    return (ddtable->exists[indx]) ? 
        ddtable->key_vals[(2*indx)+1] : (double) DDTABLE_NULL_VAL;
}

double get_check_key(ddtable_t ddtable, const double key)
{
    const uint64_t indx = dd_hash(key, ddtable->size);

    // If the key exists AND it's memcmp-identical to the given one,
    // then return the value. Otherwise, return DDTABLE_NULL_VAL
    return (ddtable->exists[indx] && 
            0 == memcmp(&ddtable->key_vals[(2*indx)], &key, sizeof(double))) 
        ? ddtable->key_vals[(2*indx)+1] : (double) DDTABLE_NULL_VAL;
}

int set_val(ddtable_t ddtable, const double key, const double val)
{
    const uint64_t indx = dd_hash(key, ddtable->size);

    if(ddtable->exists[indx])
    {
        return 1; // Collision
    } else {
        ddtable->exists[indx] = '1';
        ddtable->key_vals[2*indx] = key;
        ddtable->key_vals[(2*indx)+1] = val;
        return 0;
    }
}
