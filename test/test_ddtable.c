#include <stdlib.h>
#include <stdio.h>

#ifdef __CMAKE__
#include "libddtable.h"
#else
#include "../src/libddtable.h"
#endif

int main(void)
{
    ddtable_t ddtable = new_ddtable(100);
    free_ddtable(ddtable);
    return EXIT_SUCCESS;
}
