#ifndef LIBDDTABLE_H
#define LIBDDTABLE_H

#include <stdint.h>

//! Hash table for double-valued key-value pairs.
typedef struct ddtable *ddtable_t;

extern ddtable_t new_ddtable(const uint64_t num_keys);

extern void free_ddtable(ddtable_t ddtable);

extern double get_val(ddtable_t ddtable, const double key);

extern double get_check_key(ddtable_t ddtable, const double key);

extern int set_val(ddtable_t ddtable, const double key, const double val);

#endif
