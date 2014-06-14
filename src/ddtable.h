#ifndef DDTABLE_H
#define DDTABLE_H

#include <inttypes.h>

//! Hash table for double-valued key-value pairs.
typedef struct ddtable
{
    uint64_t num_kv_pairs;     //! Absolute number of key-value pairs
    uint64_t size;             //! Internal size used for hashing
    char* restrict exists;     //! Fast-checker for key existence
    double* restrict key_vals; //! Single-alloc array for kv pairs

} *ddtable_t;

extern ddtable_t new_ddtable(const uint64_t num_keys);

extern void free_ddtable(ddtable_t ddtable);

extern double get_val(ddtable_t ddtable, const double key);

extern double get_check_key(ddtable_t ddtable, const double key);

extern int set_val(ddtable_t ddtable, const double key, const double val);

#endif
