#include <stdlib.h>
#include <stdio.h>

#include <libddtable.h>

int main()
{
    ddtable_t ddtable = new_ddtable(100);
    free_ddtable(ddtable);
    return EXIT_SUCCESS;
}
