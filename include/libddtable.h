#ifndef LIBDDTABLE_H_INCLUDED
#define LIBDDTABLE_H_INCLUDED

#include <stdint.h>

//! Default NULL value (not a value) for ddtable
#ifndef DDTABLE_NULL_VAL
#define DDTABLE_NULL_VAL 0xdeadbeef
#endif

//! Hash table for double-valued key-value pairs.
typedef struct ddtable *ddtable_t;

/**
   Allocates and initializes a new ddtable in memory.

   @param[in] num_keys Number of initial keys.
   
   @returns new ddtable
 */
extern ddtable_t new_ddtable(const uint64_t num_keys);

/**
   Deletes an existing ddtable.

   @param[in] ddtable Table to be deleted.
 */
extern void free_ddtable(ddtable_t ddtable);

/**
   Gets the value associated with the given key in the given table.

   If the index of the table where this key hashes to is empty,
   returns DDTABLE_NULL_VAL instead.

   Note that this relies on the hash function to prevent collisions;
   this function provides no guarantees that the given key exactly as
   given is in the table. This means that if two keys collide, and
   the colliding key is in the table, its value will be fetched 
   instead.

   @see ddtable_get_val_check_key

   @param[in] ddtable Table to read value from.
   @param[in] key
   
   @returns value associated with key, or DDTABLE_NULL_VAL
 */
extern double ddtable_get_val(ddtable_t ddtable, const double key);

/**
   Gets the value associated with the given key exactly as given.

   If the index of the table where this key hashes to is empty,
   returns DDTABLE_NULL_VAL instead.

   Unlike get_val, this checks if the given key, and key at the index 
   the given key hashes to, match exactly (via memcmp). If they do
   not match exactly (i.e. memcmp returns a non-zero value), 
   this function returns DDTABLE_NULL_VAL instead.

   @see ddtable_get_val

   @param[in] ddtable Table to read value from
   @param[in] key

   @returns value associated with key, or DDTABLE_NULL_VALUE
 */
extern double ddtable_get_val_check_key
(
    ddtable_t ddtable,
    const double key
);

/**
   Inserts the given key-value pair into the given hash table.

   This function will NOT clobber existing key-value pairs, so it 
   will return 1 instead of 0 if such a collision occurs.

   @see ddtable_update_val

   @param[in] ddtable Table to set key-value pair
   @param[in] key
   @param[in] val
   
   @returns 0 if successful, 1 otherwise.
 */
extern int ddtable_set_val
(
    ddtable_t ddtable,
    const double key,
    const double val
);

/**
   Updates the given key-value pair into the given hash table.

   This function WILL clobber existing key-value pairs.

   @see ddtable_set_val

   @param[in] ddtable Table to set key-value pair
   @param[in] key
   @param[in] val   
 */
extern void ddtable_update_val
(
    ddtable_t ddtable,
    const double key,
    const double val
);

#endif // LIBDDTABLE_H_INCLUDED
