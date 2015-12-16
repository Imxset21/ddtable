#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>

#ifdef __CMAKE__
#include "libddtable.h"
#else
#include "../src/libddtable.h"
#endif

#define DEFAULT_MAX_VAL 1000
#define DEFAULT_RANDOM_SEED 42
#define DEFAULT_NUM_VALS 100
#define DDTABLE_SIZE 2000

static void fill_using_exp(ddtable_t ddtable, const unsigned int num_vals)
{
    int_fast32_t num_collisions = 0;
    for (unsigned int i = 0; i < num_vals; i++)
    {
        const double d = rand() % DEFAULT_MAX_VAL;
        num_collisions += ddtable_set_val(ddtable, d, exp(d));
    }
    printf("Number of Collisions: %"PRIiFAST32"\n", num_collisions);
}

int main(int argc, char** argv)
{
    unsigned int num_vals = DEFAULT_NUM_VALS;
    int random_seed = DEFAULT_RANDOM_SEED;
    if (argc == 1)
    {
        puts("Using default values for number of values, random seed.");
    } else {
        if (argc > 3)
        {
            fputs("Invalid number of arguments.\n", stderr);
            return EXIT_FAILURE;
        }

        // Argument #1 is number of generated values
        if (argc > 1)
        {
            num_vals = atoi(argv[1]);
        }

        // Argument #2 is random seed
        if (argc > 2)
        {
            random_seed = atoi(argv[2]);
        }
    }
    srand(random_seed);
    
    ddtable_t ddtable = ddtable_new(DDTABLE_SIZE);

    fill_using_exp(ddtable, num_vals);
    
    ddtable_free(ddtable);
    
    return EXIT_SUCCESS;
}
