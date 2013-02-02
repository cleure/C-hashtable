#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <stdint.h>
#include <time.h>

#include "config.h"

#define __HT_INTERNAL
#include "hashtable.h"
#include "MurmurHash3.h"

#define HT_STRUCT(in) struct HT_EXPORT(in)

/**
* htable_new()
*
* Create a new hash table
*
* @param    uint32_t size
* @return   struct htable *
*               NULL on error
**/
HT_STRUCT(htable) *
HT_EXPORT(htable_new)
HT_ARGS((uint32_t size))
{
    HT_STRUCT(htable) *table = malloc(sizeof(*table));
    if (!table) {
        return NULL;
    }
    
    memset(table, 0, sizeof(*table));
    table->table = malloc(sizeof(*table->table) * size);
    if (!table->table) {
        free(table);
        return NULL;
    }
    
    table->entries = malloc(sizeof(*table->entries) * size);
    if (!table->entries) {
        free(table->table);
        free(table);
        return NULL;
    }
    
    memset(table->table, 0, sizeof(*table->table) * size);
    memset(table->entries, 0, sizeof(*table->entries) * size);
    table->size = size;
    table->used = 0;
    
    srand(time(NULL));
    table->seed = rand();
    
    return table;
}

/**
* htable_clone()
*
* Clone hash table, returning new object.
*
* @param    struct htable *src
* @param    void (*copyfn)(
*                   struct htable_entry *dst,
*                   char *key
*                   void *data)
*
*               If copyfn is NULL, then pointers will simply be linked. Use
*               copyfn when you need to explicitly copy your key and/or data.
*
* @return   struct htable *
*               NULL on error
**/
HT_EXTERN HT_STRUCT(htable) *
HT_EXPORT(htable_clone)
HT_ARGS((
    HT_STRUCT(htable) *src,
    HT_EXPORT(htable_copyfn) copyfn
)) {
    
    uint32_t i, hash;
    
    HT_STRUCT(htable) *dst = HT_EXPORT(htable_new)(src->size);
    HT_STRUCT(htable_entry) *table,
                            **entries;
    
    if (!dst) {
        return NULL;
    }
    
    table = malloc(sizeof(*table) * src->size);
    if (!table) {
        free(dst);
        return NULL;
    }
    
    entries = malloc(sizeof(*entries) * src->size);
    if (!entries) {
        free(table);
        free(dst);
        return NULL;
    }
    
    /* Copy memory */
    memcpy(dst, src, sizeof(*dst));
    
    /* Zero out */
    memset(table, 0, sizeof(*table) * src->size);
    memset(entries, 0, sizeof(*entries) * src->size);
    
    /* Link pointers */
    dst->table = table;
    dst->entries = entries;
    
    if (copyfn == NULL) {
        for (i = 0; i < src->used; i++) {
            hash = src->entries[i]->hash;
            dst->table[hash].key = src->entries[i]->key;
            dst->table[hash].data = src->entries[i]->data;
            dst->table[hash].entry = i;
            dst->table[hash].hash = hash;
            dst->entries[i] = &dst->table[hash];
        }
    } else {
        for (i = 0; i < src->used; i++) {
            hash = src->entries[i]->hash;
            
            copyfn(&dst->table[hash], src->entries[i]->key, src->entries[i]->data);
            dst->entries[i] = &dst->table[hash];
        }
    }
    
    return dst;
}

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
void
HT_EXPORT(htable_delete)
HT_ARGS((
    HT_STRUCT(htable) *table,
    HT_EXPORT(htable_freefn) freefn
)) {
    
    uint32_t i;
    
    if (freefn != NULL) {
        for (i = 0; i < table->used; i++) {
            if (table->entries[i] && table->entries[i]->key) {
                /* Call freefn() */
                freefn(table->entries[i]);
            }
        }
    }
    
    free(table->table);
    free(table->entries);
    free(table);
}

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
int
HT_EXPORT(htable_resize)
HT_ARGS((
    HT_STRUCT(htable) *table,
    uint8_t load_thresh,
    uint32_t new_size
)) {

    uint32_t i, res;
    float load_calc;
    
    HT_STRUCT(htable) tmp_table;
    HT_STRUCT(htable_entry) *new_table;
    HT_STRUCT(htable_entry) **new_entries;
    
    /* Check load_thresh before proceeding */
    load_calc = 100.0f * ((float)table->used / (float)table->size);
    if (load_thresh && load_calc < load_thresh) {
        return 1;
    }
    
    new_table = malloc(sizeof(*new_table) * new_size);
    if (!new_table) {
        return 0;
    }
    
    new_entries = malloc(sizeof(*new_entries) * new_size);
    if (!new_entries) {
        free(new_table);
        return 0;
    }
    
    /* Zero out */
    memset(&tmp_table, 0, sizeof(tmp_table));
    memset(new_table, 0, sizeof(*new_table) * new_size);
    memset(new_entries, 0, sizeof(*new_entries) * new_size);
    
    /* Set variables on tmp_table */
    tmp_table.table = new_table;
    tmp_table.entries = new_entries;
    tmp_table.size = new_size;
    tmp_table.used = 0;
    tmp_table.seed = table->seed;
    
    /* Iterate over source */
    for (i = 0; i < table->used; i++) {
        
        /* Add entries to new table array */
        res = HT_EXPORT(htable_add)(
                        &tmp_table,
                        table->entries[i]->key,
                        table->entries[i]->data,
                        NULL);
        
        /* Catch error */
        if (!res) {
            free(new_table);
            free(new_entries);
            return 0;
        }
    }
    
    /* Free old memory */
    free(table->table);
    free(table->entries);
    
    /* Link up new data */
    table->table = new_table;
    table->entries = new_entries;
    table->size = new_size;
    table->used = tmp_table.used;
    
    return 1;
}

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
int
HT_EXPORT(htable_add)
HT_ARGS((
    HT_STRUCT(htable) *table,
    char *key,
    void *data,
    HT_EXPORT(htable_freefn) freefn
)) {
    
    uint32_t hash,
             step = 0;
    
    /* Get initial hash */
    MurmurHash3_x86_32(key, strlen(key), table->seed, &hash);
    
    do {
        /* Quadratic Probing Function:
            h = (h + (step * step - step) / 2) % size */
        hash = (hash + (step * step - step) / 2) % table->size;
        
        if (table->table[hash].key == NULL) {
            goto insert;
        } else if (strcmp(key, table->table[hash].key) == 0) {
            /* Replace */
            if (freefn != NULL) {
                /* Call freefn() */
                freefn(&(table->table[hash]));
            }
            
            goto replace;
        }
        
        step += 1;
    } while (hash);
    
    return 0;
    
        insert:
            table->table[hash].hash = hash;
            table->table[hash].entry = table->used;
            table->entries[table->used] = &table->table[hash];
            table->used++;
            
        replace:
            table->table[hash].key = key;
            table->table[hash].data = data;
    
    return 1;
}

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
int
HT_EXPORT(htable_add_loop)
HT_ARGS((
    HT_STRUCT(htable) *table,
    char *key,
    void *data,
    HT_EXPORT(htable_freefn) freefn,
    int max_loops
)) {
    int loop;
    int chunk = table->size;
    int size = table->size;
    
    max_loops++;
    for (loop = 1; loop < max_loops; loop++) {
        if (HT_EXPORT(htable_add)(table, key, data, freefn)) {
            return loop;
        }
        
        size += chunk;
        chunk += (chunk / 2);
        
        if (!HT_EXPORT(htable_resize)(table, 0, size)) {
            return 0;
        }
    }
    
    return 0;
}

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
int
HT_EXPORT(htable_remove)
HT_ARGS((
    HT_STRUCT(htable) *table,
    char *key,
    HT_EXPORT(htable_freefn) freefn
)) {

    uint32_t hash,
             step = 0;
    
    /* Get initial hash */
    MurmurHash3_x86_32(key, strlen(key), table->seed, &hash);
    
    do {
        /* Quadratic Probing */
        hash = (hash + (step * step - step) / 2) % table->size;
        
        if (    table->table[hash].key &&
                strcmp(key, table->table[hash].key) == 0) {
            
            if (freefn != NULL) {
                /* Call freefn() */
                freefn(&table->table[hash]);
            }
            
            if (table->used > 0) {
                /* Swap current entry with last entry, then NULL out last entry
                   to maintain linear array of pointers to elements. */
                table->entries[table->table[hash].entry] = table->entries[table->used-1];
                table->entries[table->table[hash].entry]->entry = table->table[hash].entry;
                table->entries[table->used-1] = NULL;
            } else {
                table->entries[0] = NULL;
            }
            
            /* Zero out entry */
            memset(&table->table[hash], 0, sizeof(*table->table));
            
            /* Decrement used count */
            table->used--;
            return 1;
        }
        
        step += 1;
    } while (hash);
    
    return 0;
}

/**
* htable_get()
*
* Get entry from hash table.
*
* @param    struct htable *table
* @param    char *key
* @return   NULL on error, pointer on success
**/
HT_STRUCT(htable_entry) *
HT_EXPORT(htable_get)
HT_ARGS((
    HT_STRUCT(htable) *table,
    char *key
)) {
    
    uint32_t hash,
             step = 0;
    
    /* Get initial hash */
    MurmurHash3_x86_32(key, strlen(key), table->seed, &hash);
    
    do {
        /* Quadratic Probing */
        hash = (hash + (step * step - step) / 2) % table->size;
        
        if (    table->table[hash].key &&
                strcmp(key, table->table[hash].key) == 0) {
            return &(table->table[hash]);
        }
        
        step += 1;
    } while (hash);
    
    return NULL;
}

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
HT_STRUCT(htable_entry) **
HT_EXPORT(htable_intersect)
HT_ARGS((
    HT_STRUCT(htable) *a,
    HT_STRUCT(htable) *b
)) {

    uint32_t max_size, i;
    
    HT_STRUCT(htable_entry) **list, **head;
    HT_STRUCT(htable_entry) *tmp;

    if (a->used > b->used) {
        max_size = a->used;
    } else {
        max_size = a->used;
    }
    
    list = malloc(sizeof(*list) * (max_size+1));
    if (!list) {
        return NULL;
    }
    
    memset(list, 0, sizeof(*list) * (max_size+1));
    head = list;
    
    for (i = 0; i < a->used; i++) {
        tmp = HT_EXPORT(htable_get)(b, a->entries[i]->key);
        
        if (tmp != NULL) {
            list[0] = tmp;
            list++;
        }
    }

    return head;
}

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
HT_STRUCT(htable_entry) **
HT_EXPORT(htable_difference)
HT_ARGS((
    HT_STRUCT(htable) *a,
    HT_STRUCT(htable) *b
)) {

    uint32_t max_size, i;
    
    HT_STRUCT(htable_entry) **list, **head;
    HT_STRUCT(htable_entry) *tmp;
    
    if (a->used > b->used) {
        max_size = a->used;
    } else {
        max_size = b->used;
    }
    
    list = malloc(sizeof(*list) * (max_size+1));
    if (!list) {
        return NULL;
    }
    
    memset(list, 0, sizeof(*list) * (max_size+1));
    head = list;
    
    for (i = 0; i < a->used; i++) {
        tmp = HT_EXPORT(htable_get)(b, a->entries[i]->key);
        if (!tmp) {
            list[0] = a->entries[i];
            list++;
        }
    }

    return head;
}
