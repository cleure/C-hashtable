#pragma once
#include <stdint.h>
#include "config.h"
#include "hashtable-config.h"

#ifdef __HT_INTERNAL
  #define HT_EXTERN
#else
  #define HT_EXTERN extern
#endif

#ifndef HT_EXPORT
    #define HT_EXPORT(SYM) SYM
#endif

#define HT_ARGS(SYM) SYM

struct HT_EXPORT(htable_entry);
struct HT_EXPORT(htable);

/* htable_copyfn type definition */
typedef
void (* HT_EXPORT(htable_copyfn))
HT_ARGS((
    struct HT_EXPORT(htable_entry) *dst,
    char *key,
    void *data
));

/* htable_freefn type definition */
typedef
void (* HT_EXPORT(htable_freefn))
HT_ARGS((
    struct HT_EXPORT(htable_entry) *ptr
));

/* Hash Table Entry */
struct HT_EXPORT(htable_entry) {
    char *key;
    void *data;
    uint32_t entry;
    uint32_t hash;
};

/* Hash Table */
struct HT_EXPORT(htable) {
    struct HT_EXPORT(htable_entry) *table;
    struct HT_EXPORT(htable_entry) **entries;
    uint32_t size;
    uint32_t used;
    uint32_t seed;
};

/************************************************************************
* Creation and maintenance functions
************************************************************************/

/**
* htable_new()
*
* Create a new hash table
*
* @param    uint32_t size
* @return   struct htable *
*               NULL on error
**/
HT_EXTERN struct HT_EXPORT(htable) *
HT_EXPORT(htable_new)
HT_ARGS((
    uint32_t size
));

/**
* htable_clone()
*
* Clone hash table, returning new object.
*
* @param    struct htable *src
* @param    void (*copyfn)(
*                   struct htable_entry *dst,
*                   struct htable_entry *dst)
*
*               If copyfn is NULL, then pointers will simply be linked. Use
*               copyfn when you need to explicitly copy your key and/or data.
*
* @return   struct htable *
*               NULL on error
**/
HT_EXTERN struct HT_EXPORT(htable) *
HT_EXPORT(htable_clone)
HT_ARGS((
    struct HT_EXPORT(htable) *src,
    HT_EXPORT(htable_copyfn) copyfn
));

/**
* htable_delete()
*
* Delete hash table created by htable_new()
*
* @param    struct htable *table
* @param    void (*freefn)(struct htable_entry *)
*               If not null, will be called for each entry to free data
*               allocated outside of the htable_* functions.
* @return   void
**/
HT_EXTERN void
HT_EXPORT(htable_delete)
HT_ARGS((
    struct HT_EXPORT(htable) *table,
    HT_EXPORT(htable_freefn) freefn
));

/**
* htable_resize()
*
* Resize hash table, normally for growing, but can also be used to
* shrink the size of the table.
*
* @param    struct htable *table
* @param    uint8_t load_thresh
*               Number between 0 and 100. If load factor is below it,
*               then resize won't trigger. Use 0 to disable.
*
* @param    uint32_t new_size
* @return   0 on error, 1 on success
**/
HT_EXTERN int
HT_EXPORT(htable_resize)
HT_ARGS((
    struct HT_EXPORT(htable) *table,
    uint8_t load_thresh,
    uint32_t new_size
));

/************************************************************************
* Manipulation functions
************************************************************************/

/**
* htable_add()
*
* Add item to hash table. 
*
* @param    struct htable *table
* @param    char *key
* @param    void *data
* @param    void (*freefn)(struct htable_entry *)
*               If not null, will be called when an item is replaced.
*
* @return   0 on error, 1 on success
**/
HT_EXTERN int
HT_EXPORT(htable_add)
HT_ARGS((
    struct HT_EXPORT(htable) *table,
    char *key,
    void *data,
    HT_EXPORT(htable_freefn) freefn
));

/**
* htable_add_loop()
*
* Unlink htable_add(), which will fail on its first try, htable_add_loop()
* will try a maximum of "max_loops" times to add an item before giving up.
* It uses a quadratic resizing algorithm, so its best to keep max_loops
* relatively small.
*
* @param    struct htable *table
* @param    char *key
* @param    void *data
* @param    void (*freefn)(struct htable_entry *)
*               If not null, will be called when an item is replaced.
* @param    int max_loops
*
* @return   0 on failure, otherwise the number of loops it took to
*           successfully add item.
**/
HT_EXTERN int
HT_EXPORT(htable_add_loop)
HT_ARGS((
    struct HT_EXPORT(htable) *table,
    char *key,
    void *data,
    HT_EXPORT(htable_freefn) freefn,
    int max_loops
));

/**
* htable_remove()
*
* Remove item from hash table.
*
*
* @param    struct htable *table
* @param    char *key
* @param    void (*freefn)(struct htable_entry *)
*               If not null, will be called when an item is removed.
*
* @return   0 on error, 1 on success
**/
HT_EXTERN int
HT_EXPORT(htable_remove)
HT_ARGS((
    struct HT_EXPORT(htable) *table,
    char *key,
    HT_EXPORT(htable_freefn) freefn
));

/**
* htable_get()
*
* Get entry from hash table.
*
* @param    struct htable *table
* @param    char *key
* @return   NULL on error, pointer on success
**/
HT_EXTERN struct HT_EXPORT(htable_entry) *
HT_EXPORT(htable_get)
HT_ARGS((
    struct HT_EXPORT(htable) *table,
    char *key
));

/************************************************************************
* Utility functions
************************************************************************/

/**
* htable_intersect()
*
* Get intersection of two hash tables, by key. Entries in the list
* are pointers to elements in b, thus free()'ing b and then trying
* to access elements in the list will likely cause a segfault. The
* returned list is created with malloc(), so the user should free
* it once it's no longer needed.
*
* Usage:
*
* uint32_t i;
* struct htable_entry **list = htable_intersect(a, b);
* 
* for (i = 0; list[i]; i++) {
*   printf("%s\n", list[i]->key);
* }
*
* free(list);
*
* @param    struct htable *a
* @param    struct htable *b
* @return   struct htable_entry **
*               NULL on error
**/
HT_EXTERN struct HT_EXPORT(htable_entry) **
HT_EXPORT(htable_intersect)
HT_ARGS((
    struct HT_EXPORT(htable) *a,
    struct HT_EXPORT(htable) *b
));

/**
* htable_difference()
*
* Get difference of two hash tables, by key. Entries in the list
* are pointers to elements in b, thus free()'ing b and then trying
* to access elements in the list will likely cause a segfault. The
* returned list is created with malloc(), so the user should free
* it once it's no longer needed.
*
* Usage:
*
* uint32_t i;
* struct htable_entry **list = htable_difference(a, b);
* 
* for (i = 0; list[i]; i++) {
*   printf("%s\n", list[i]->key);
* }
*
* free(list);
*
* @param    struct htable *a
* @param    struct htable *b
* @return   struct htable_entry **
*               NULL on error
**/
HT_EXTERN struct HT_EXPORT(htable_entry) **
HT_EXPORT(htable_difference)
HT_ARGS((
    struct HT_EXPORT(htable) *a,
    struct HT_EXPORT(htable) *b
));

#ifndef __HT_INTERNAL
  #undef HT_EXTERN
  #undef HT_ARGS
  #undef HT_EXPORT
#endif
