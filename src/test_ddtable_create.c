#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

//TODO: Use autocomplete to make rand.h an requirement for check

#ifndef __clang__
#include <libddtable.h>
#else
#include "../include/libddtable.h"
#endif

#ifndef NUM_KEYS
#define NUM_KEYS 1000
#endif

#ifndef D_MIN
#define D_MIN 0.0
#endif

#ifndef D_MAX
#define D_MAX 1.0
#endif

#define IS_POW2(x) ((x != 0) && ((x & (~x + 1)) == x))

// Generates random number in range
#define randd(_MIN, _MAX) _MIN + (double)rand() / RAND_MAX * (_MAX - _MIN);

static double* gen_random_nums(const unsigned int num_vals)
{
    double* restrict arr = (double*) calloc(num_vals, sizeof(double));
    for (unsigned int i = 0; i < num_vals; i++)
    {
        arr[i] = randd(D_MIN, D_MAX);
    }
    return arr;
}

static double* sample_rand_nums
(
    const unsigned int num_vals,
    const double* restrict sample_from,
    const unsigned int sample_from_len
)
{
    double* restrict arr = (double*) calloc(num_vals, sizeof(double));
    
    for (unsigned int i = 0; i < num_vals; i++)
    {
        arr[i] = sample_from[(int)(rand() / (RAND_MAX / sample_from_len + 1))];
    }

    return arr;
}

int rand_test_ddtable()
{
    double* keys = gen_random_nums(NUM_KEYS);
    double* vals = gen_random_nums(NUM_KEYS);

    ddtable_t my_table = new_ddtable(NUM_KEYS*20, DDTABLE_SPOOKY_HASH, DDTABLE_STATIC_SIZE);

    // Used to prevent ifs from being optimized out
    int num_errors = 0;
    
    // Set values
    for (unsigned int i = 0; i < NUM_KEYS; i++)
    {
        if(ddtable_set_val(my_table, keys[i], vals[i]))
        {
            // printf("Collision for key %f\n", keys[i]);
            num_errors++;
        }
    }

    // Retrieve values
    for (unsigned int i = 0; i < NUM_KEYS; i++)
    {
        if(DDTABLE_NULL_VAL == ddtable_get_val_check_key(my_table, keys[i]))
        {
            // printf("Unable to get value for key: %f\n", keys[i]);
            num_errors++;
        }
    }

    free_ddtable(my_table);
    free(keys);
    free(vals);
    
    return EXIT_SUCCESS;
}

#ifdef HAVE_MATH_H
#if HAVE_MATH_H == 1

#include <math.h>

int exp_test_ddtable()
{
    double* restrict random_key_sample = gen_random_nums(10);
    double* restrict keys =  sample_rand_nums(100, random_key_sample, 10);
    double* restrict vals = (double*) calloc(NUM_KEYS, sizeof(double));
    
    ddtable_t ddtable = new_ddtable(NUM_KEYS*10, DDTABLE_SPOOKY_HASH, DDTABLE_STATIC_SIZE);

    unsigned int num_hits = 0;
    unsigned int num_misses = 0;

    for (unsigned int i = 0; i < NUM_KEYS; i++)
    {
        const double curr_key = keys[i];
        const double candidate_val = ddtable_get_val_check_key(ddtable, curr_key);

        if (DDTABLE_NULL_VAL == candidate_val)
        {
            // Cache miss.
            const double new_val = vals[i] = exp(curr_key);
            ddtable_set_val(ddtable, curr_key, new_val);
            num_misses++;
        } else {
            // Cache hit.
            vals[i] = candidate_val;
            num_hits++;
        }
    }

    free_ddtable(ddtable);
    free(random_key_sample);
    free(keys);
    free(vals);

    printf("Misses to hits: %u vs %u\n", num_misses, num_hits);

    return 1;
}
#endif //if HAVE_MATH_H == 1
#endif //ifdef HAVE_MATH_H

static int nonrand_test_ddtable()
{
    ddtable_t my_table = new_ddtable(10, DDTABLE_SPOOKY_HASH, DDTABLE_STATIC_SIZE_POW2);

    const unsigned int num_vals = 4;
    double vals[4] = {4.5, 5.5, 1.0, 22.23};
    double keys[4] = {10.0, 33.22, 15.0, 11.321};

    for (unsigned int i = 0; i < num_vals; i++)
    {
        ddtable_set_val(my_table, keys[i], vals[i]);
    }

    printf("Get val for key %f: %f\n", keys[1], ddtable_get_val(my_table, keys[1]));

    return EXIT_SUCCESS;
}

/* TODO: Add checks for <stddarg.h>
#ifndef NDEBUG
#define aprintf(...) fprintf(stderr, __VA_ARGS__)
#else
#define arprintf(...) do {} while(0)
#endif
*/

#ifndef NUM_KEYS
#define NUM_KEYS 100
#endif

#define NUM_FXNS 5
#define NUM_BEHAVIOURS 2

static const ddtable_hash_fxn hash_fxns[NUM_FXNS] = 
{
    DDTABLE_SUM_HASH,
    DDTABLE_SPOOKY_HASH,
    DDTABLE_MURMUR3_HASH,
    DDTABLE_XX_HASH,
    DDTABLE_CRC32_HASH,
};

static const char* hash_fxn_names[NUM_FXNS] = 
{
    "DDTABLE_SUM_HASH\0",
    "DDTABLE_SPOOKY_HASH\0",
    "DDTABLE_MURMUR3_HASH\0",
    "DDTABLE_XX_HASH\0",
    "DDTABLE_CRC32_HASH\0",
};

static const ddtable_size size_behaviours[NUM_BEHAVIOURS] =
{
    DDTABLE_STATIC_SIZE,
    DDTABLE_STATIC_SIZE_POW2,
};

static const char* size_behaviour_names[NUM_BEHAVIOURS] =
{
    "DDTABLE_STATIC_SIZE\0",
    "DDTABLE_STATIC_SIZE_POW2\0",
};

static int check_creation()
{
    ddtable_t ddtable = NULL;
    
    for (unsigned int i = 0; i < NUM_FXNS; i++)
    {
        for (unsigned int j = 0; j < NUM_BEHAVIOURS; j++)
        {
            ddtable = new_ddtable(NUM_KEYS, hash_fxns[i], size_behaviours[j]);
            #ifndef NDEBUG
            fprintf(
                stderr,
                "Testing table creation using %s fxn and %s size behaviour.\n",
                hash_fxn_names[i],
                size_behaviour_names[j]
            );
            #endif
            assert(ddtable != NULL && "Failed creating ddtable.");
            free_ddtable(ddtable);
        }
    }

    return EXIT_SUCCESS;
}

int main()
{
    if (check_creation() == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }
    if (nonrand_test_ddtable() == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
