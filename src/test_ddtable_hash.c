#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#ifndef __clang__
#include <libddtable.h>
#else
#include "../include/libddtable.h"
#endif

// Test-specific variables

#ifndef NUM_KEYS
#define NUM_KEYS 1000
#endif

#ifndef CACHE_SIZE_MULTIPLIER
#define CACHE_SIZE_MULTIPLIER 20
#endif

#ifndef D_MIN
#define D_MIN 0.0
#endif

#ifndef D_MAX
#define D_MAX 1.0
#endif

#ifndef RANDOM_SEED
#define RANDOM_SEED 42
#endif

#define randd() D_MIN + (double)rand() / RAND_MAX * (D_MAX - D_MIN);

static inline double* gen_random_nums(const unsigned int num_vals)
{
    double* restrict arr = (double*) calloc(num_vals, sizeof(double));
    for (unsigned int i = 0; i < num_vals; i++)
    {
        arr[i] = randd();
    }
    return arr;
}

static void test_rand_hash(ddtable_t hash_table, double* keys, double* vals)
{
    // Used to keep track of collisions, misses
    int num_errors = 0;
    
    // Set values
    for (unsigned int i = 0; i < NUM_KEYS; i++)
    {
        if(ddtable_set_val(hash_table, keys[i], vals[i]))
        {
            num_errors++;
        }
    }

    printf("Number of collisions: %i\n", num_errors);
    num_errors = 0;

    // Retrieve values
    for (unsigned int i = 0; i < NUM_KEYS; i++)
    {
        if(DDTABLE_NULL_VAL == ddtable_get_val_check_key(hash_table, keys[i]))
        {
            num_errors++;
        }
    }

    printf("Number of cache misses: %i\n", num_errors);

    free_ddtable(hash_table);
}

int rand_test_ddtable()
{
    // Cache size
    const unsigned int cache_size = CACHE_SIZE_MULTIPLIER * NUM_KEYS;

    // Create random keys and values, shared between hash tables
    double* keys = gen_random_nums(NUM_KEYS);
    double* vals = gen_random_nums(NUM_KEYS);

    // Create hash tables using different functions
    ddtable_t curr_hash_table = NULL;

    curr_hash_table = new_ddtable(cache_size, DDTABLE_SUM_HASH);
    puts("Stats for sum hash table:");
    test_rand_hash(curr_hash_table, keys, vals);

    curr_hash_table = new_ddtable(cache_size, DDTABLE_SPOOKY_HASH);
    puts("Stats for spooky hash table:");
    test_rand_hash(curr_hash_table, keys, vals);

    curr_hash_table = new_ddtable(cache_size, DDTABLE_MURMUR3_HASH);
    puts("Stats for Murmur3 hash table:");
    test_rand_hash(curr_hash_table, keys, vals);

    curr_hash_table = new_ddtable(cache_size, DDTABLE_XX_HASH);
    puts("Stats for XX hash table:");
    test_rand_hash(curr_hash_table, keys, vals);
    
    // Cleanup
    free(keys);
    free(vals);
    
    return EXIT_SUCCESS;
}

int main()
{
    // Set random seed for consitent simulations
    srand(RANDOM_SEED);

    return rand_test_ddtable();
}
