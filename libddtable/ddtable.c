#if HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef __clang__
// For primary build (I use only gcc for production)
#include <libddtable.h>
#include <spooky-c.h>
#include <MurmurHash3.h>
#include <xxhash.h>
#include <crc32.h>
#else
// For Flycheck (which uses clang)
#include "../include/libddtable.h"
#include "../common/spooky-c.h"
#include "../common/MurmurHash3.h"
#include "../common/xxhash.h"
#include "../common/crc32.h"
#define HAVE_STDINT_H 1
#endif // __clang__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#if HAVE_INTTYPES_H
#include <inttypes.h>
#elif HAVE_STDINT_H
#include <stdint.h>
#else
#error libddtable requires a definition of uint64_t
#endif

//! Checks if x is a power of 2
#define IS_POW2(x) ((x != 0) && ((x & (~x + 1)) == x))

//! Seed we pass to spooky
#ifndef SPOOKY_HASH_SEED
#define SPOOKY_HASH_SEED 0
#endif

//! Seed we pass to MurmurHash3
#ifndef MURMUR_HASH_3_SEED
#define MURMUR_HASH_3_SEED 0
#endif

//! Seed we pass to MurmurHash3
#ifndef XX_HASH_SEED
#define XX_HASH_SEED 0
#endif

//! Internal hash function pointer type.
typedef uint64_t (*ddtable_hash_fxn_t) (const double, const uint64_t);

//! Internal definition of forward declaration from header
typedef struct ddtable
{
    uint64_t num_kv_pairs;     //! Absolute number of key-value pairs
    uint64_t size;             //! Internal size used for hashing
    ddtable_hash_fxn_t my_fxn; //! Function to use for hashing
    char* restrict exists;     //! Fast-checker for key existence
    double* restrict key_vals; //! Single-alloc array for kv pairs

} ddtable;

//! Hash function using CRC32
static uint64_t ddtable_crc32_hash(const double key, const uint64_t size)
{
    const double _key = key;
    return crc32_64((void*) &_key) % size;
}

//! CRC32 can use faster & instead of % if we enforce power of 2 size.
static uint64_t ddtable_crc32_hash_pow2(const double key, const uint64_t size)
{
    const double _key = key;
    return crc32_64((void*) &_key) & size;
}

//! Hash function using XXHash32.
static uint64_t ddtable_xxhash32_hash(const double key, const uint64_t size)
{
    const double _key = key;
    uint32_t* msg = (uint32_t*) &_key;
    uint64_t indx = XXH32(&msg[0], sizeof(uint32_t), XX_HASH_SEED);
    indx += XXH32(&msg[1], sizeof(uint32_t), XX_HASH_SEED);
    return indx % size;
}

//! XXHash32 can use faster & instead of % if we enforce power of 2 size.
static uint64_t ddtable_xxhash32_hash_pow2(const double key, const uint64_t size)
{
    const double _key = key;
    uint32_t* msg = (uint32_t*) &_key;
    uint64_t indx = XXH32(&msg[0], sizeof(uint32_t), XX_HASH_SEED);
    indx += XXH32(&msg[1], sizeof(uint32_t), XX_HASH_SEED);
    return indx & size;
}

//! Murmur3 128-bit (x64) hash.
static uint64_t ddtable_murmur3_hash(const double key, const uint64_t size)
{
    uint64_t indx = 0;
    const double _key = key;
    MurmurHash3_x64_128(&_key, sizeof(double), MURMUR_HASH_3_SEED, &indx); 
    return indx % size;
}

//! Murmur3 can use faster & instead of % if we enforce power of 2 size.
static uint64_t ddtable_murmur3_hash_pow2(const double key, const uint64_t size)
{
    uint64_t indx = 0;
    const double _key = key;
    MurmurHash3_x64_128(&_key, sizeof(double), MURMUR_HASH_3_SEED, &indx); 
    return indx & size;
}

//! Hash function using spooky 64-bit hash.
static uint64_t ddtable_spooky_hash(const double key, const uint64_t size)
{
    return spooky_hash64(&key, sizeof(double), SPOOKY_HASH_SEED) % size;
}

//! spooky 64-bit can use faster & instead of % if we enforce power of 2 size.
static uint64_t ddtable_spooky_hash_pow2(const double key, const uint64_t size)
{
    // Can use faster & instead of % if we enforce power of 2 size.
    return spooky_hash64(&key, sizeof(double), SPOOKY_HASH_SEED) & size;
}

//TODO: Actually make this do an add
static uint64_t ddtable_add_hash(const double key, const uint64_t size)
{
    return ((uint64_t) key) % size;
}

//TODO: Actually make this do an add
static uint64_t ddtable_add_hash_pow2(const double key, const uint64_t size)
{
    // Can use faster & instead of % if we enforce power of 2 size.
    return ((uint64_t) key) & size;
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

ddtable_t new_ddtable(const uint64_t num_keys, ddtable_hash_fxn fxn_type, ddtable_size size)
{
    ddtable_t new_ht = (ddtable_t) malloc(sizeof(struct ddtable));
    assert(new_ht);

    // Set the absolute number of key-value pairs, and also
    // set the internal size depending on whether we enforce
    // "power of 2"-sized tables.
    if (size == DDTABLE_STATIC_SIZE_POW2)
    {
        // This minus one trick is necessary for &: http://goo.gl/FlcEb0
        new_ht->size = nextPowerOf2(num_keys) - 1;
        new_ht->num_kv_pairs = new_ht->size + 1;
    } else {
        new_ht->num_kv_pairs = num_keys;
        new_ht->size = num_keys;
    }

    // Choose our function
    switch (fxn_type)
    {
        case DDTABLE_SUM_HASH:
        {
            new_ht->my_fxn = (size == DDTABLE_STATIC_SIZE) ? &ddtable_add_hash : &ddtable_add_hash_pow2;
            break;
        }
        case DDTABLE_MURMUR3_HASH:
        {
            new_ht->my_fxn = (size == DDTABLE_STATIC_SIZE) ? &ddtable_murmur3_hash : &ddtable_murmur3_hash_pow2;
            break;
        }
        case DDTABLE_XX_HASH:
        {
            new_ht->my_fxn = (size == DDTABLE_STATIC_SIZE) ? &ddtable_xxhash32_hash : &ddtable_xxhash32_hash_pow2;
            break;
        }
        case DDTABLE_CRC32_HASH:
        {
            new_ht->my_fxn = (size == DDTABLE_STATIC_SIZE) ? &ddtable_crc32_hash : &ddtable_crc32_hash_pow2;
            break;
        }
        // Spooky is default
        default:
        case DDTABLE_SPOOKY_HASH:
        {
            new_ht->my_fxn = (size == DDTABLE_STATIC_SIZE) ? &ddtable_spooky_hash : &ddtable_spooky_hash_pow2;
        }
    }

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

    return;
}

double ddtable_get_val(ddtable_t ddtable, const double key)
{
    const uint64_t indx = ddtable->my_fxn(key, ddtable->size);

    return (ddtable->exists[indx]) ? 
        ddtable->key_vals[(2*indx)+1] : (double) DDTABLE_NULL_VAL;
}

double ddtable_get_val_check_key(ddtable_t ddtable, const double key)
{
    const uint64_t indx = ddtable->my_fxn(key, ddtable->size);

    // If the key exists AND it's memcmp-identical to the given one,
    // then return the value. Otherwise, return DDTABLE_NULL_VAL
    return (ddtable->exists[indx] && 
            0 == memcmp(&ddtable->key_vals[(2*indx)], &key, sizeof(double))) 
        ? ddtable->key_vals[(2*indx)+1] : (double) DDTABLE_NULL_VAL;
}

int ddtable_set_val(ddtable_t ddtable, const double key, const double val)
{
    const uint64_t indx = ddtable->my_fxn(key, ddtable->size);

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
    const uint64_t indx = ddtable->my_fxn(key, ddtable->size);

    ddtable->exists[indx] = '1';
    ddtable->key_vals[2*indx] = key;
    ddtable->key_vals[(2*indx)+1] = val;

    return;
}
