#pragma once
#include <stdint.h>
#include "config.h"
#include "hashtable-config.h"

#ifdef USE_PTHREAD
  #include <pthread.h>
#endif

#ifdef __HT_INTERNAL
  #define HT_EXTERN
#else
  #define HT_EXTERN extern
#endif

#ifndef HT_FUNC
  #define HT_FUNC(in) in
#endif

#ifndef HT_TYPE
  #define HT_TYPE(in) in
#endif

#define HT_ARGS(in) in

struct HT_TYPE(htable_entry);
struct HT_TYPE(htable);

/* htable_copyfn type definition */
typedef
void (* HT_FUNC(htable_copyfn))
HT_ARGS((
    struct HT_TYPE(htable_entry) *dst,
    char *key,
    void *data
));

/* htable_freefn type definition */
typedef
void (* HT_FUNC(htable_freefn))
HT_ARGS((
    struct HT_TYPE(htable_entry) *ptr
));

/* Hash Table Entry */
struct HT_TYPE(htable_entry) {
    char *key;
    void *data;
    uint32_t entry;
    uint32_t hash;
};

/* Hash Table */
struct HT_TYPE(htable) {
    struct HT_TYPE(htable_entry) *table;
    struct HT_TYPE(htable_entry) **entries;
    uint32_t size;
    uint32_t used;
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
HT_EXTERN struct HT_TYPE(htable) *
HT_FUNC(htable_new)
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
HT_EXTERN struct HT_TYPE(htable) *
HT_FUNC(htable_clone)
HT_ARGS((
    struct HT_TYPE(htable) *src,
    HT_FUNC(htable_copyfn) copyfn
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
HT_FUNC(htable_delete)
HT_ARGS((
    struct HT_TYPE(htable) *table,
    HT_FUNC(htable_freefn) freefn
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
HT_FUNC(htable_resize)
HT_ARGS((
    struct HT_TYPE(htable) *table,
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
HT_FUNC(htable_add)
HT_ARGS((
    struct HT_TYPE(htable) *table,
    char *key,
    void *data,
    HT_FUNC(htable_freefn) freefn
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
HT_FUNC(htable_remove)
HT_ARGS((
    struct HT_TYPE(htable) *table,
    char *key,
    HT_FUNC(htable_freefn) freefn
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
HT_EXTERN struct HT_TYPE(htable_entry) *
HT_FUNC(htable_get)
HT_ARGS((
    struct HT_TYPE(htable) *table,
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
HT_EXTERN struct HT_TYPE(htable_entry) **
HT_FUNC(htable_intersect)
HT_ARGS((
    struct HT_TYPE(htable) *a,
    struct HT_TYPE(htable) *b
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
HT_EXTERN struct HT_TYPE(htable_entry) **
HT_FUNC(htable_difference)
HT_ARGS((
    struct HT_TYPE(htable) *a,
    struct HT_TYPE(htable) *b
));

#ifndef __HT_INTERNAL
  #undef HT_EXTERN
  #undef HT_FUNC
  #undef HT_TYPE
  #undef HT_ARGS
#endif
