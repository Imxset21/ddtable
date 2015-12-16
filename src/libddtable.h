#ifndef LIBDDTABLE_H
#define LIBDDTABLE_H

#ifdef _cplusplus
extern "C" {
#endif /* _cplusplus */

#include <stdint.h>

/* Hash table for double-valued key-value pairs. */
typedef struct ddtable *ddtable_t;

extern ddtable_t ddtable_new(const uint_fast32_t num_keys);

extern void ddtable_free(ddtable_t ddtable);

extern double ddtable_get_val(const ddtable_t ddtable, const double key);

extern double ddtable_get_check_key(const ddtable_t ddtable, const double key);

extern int ddtable_set_val(ddtable_t ddtable, const double key, const double val);

#ifdef _cplusplus
}
#endif /* _cplusplus */

#endif
