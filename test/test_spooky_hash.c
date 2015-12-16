#if HAVE_DDTABLE_CONFIG_H
#include "ddtable_config.h"
#else
#include "../build/config/ddtable_config.h"
#endif

#include "../src/spooky-c.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

static inline clock_t get_curr_time(void)
{
    clock_t s_time = clock();
    if (s_time == -1)
    {
        perror("Failure to get current time: ");
        return EXIT_FAILURE;
    }
    return s_time;
}

int main(int argc, char** argv)
{
    int num_exp_calls = 10000;
    int num_hash_calls = 10000;
    int random_seed = 42;
    if (argc > 1)
    {
        num_exp_calls = atoi(argv[1]);
        if (argc > 2)
        {
            num_hash_calls = atoi(argv[2]);
            if (argc > 3)
            {
                random_seed = atoi(argv[3]);
            }
        }
    }
    srand(random_seed);
    
    printf("EXP CALLS: %i\tHASH CALLS: %i\n", num_exp_calls, num_hash_calls);

    const clock_t start_exp_time = get_curr_time();
    for (int i = 0; i < num_exp_calls; i++)
    {
        const double d = rand() % 1000;
        const volatile double v = exp(d);
    }
    const clock_t stop_exp_time = get_curr_time();
    printf("Time to perform EXP Calls: %li\n", stop_exp_time - start_exp_time);

    const clock_t start_hash_time = get_curr_time();
    for (int i = 0; i < num_hash_calls; i++)
    {
        const double d = rand() % 1000;
        spooky_hash64(&d, sizeof(double), 0);
    }
    const clock_t stop_hash_time = get_curr_time();
    printf("Time to perform HASH Calls: %li\n", stop_hash_time - start_hash_time);
    
    return EXIT_SUCCESS;
}
