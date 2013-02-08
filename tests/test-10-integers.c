#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "config.h"
#include "hashtable.h"

int data[] = {
    2, 4, 8, 16, 32, 64, 128, 256, 512,
    1024, 2048, 4096, 8192, 16384, 32768,
    65536, 131072, 262144, 524288, 1048576
};

void copyfn(struct htable_entry *dst, void *key, void *data)
{
    dst->key = malloc(4);
    *(int *)(dst->key) = *(int *)key;
}

void freefn(struct htable_entry *ent)
{
    free(ent->key);
}

int main(int argc, char **argv)
{
    int i, len, res;
    struct htable *table;
    
    table = htable_new(512, 0, &htable_int32_cmpfn, &copyfn, &freefn);
    assert(table != NULL);
    
    len = sizeof(data)/sizeof(data[0]);
    for (i = 0; i < len; i++) {
        res = htable_add(table, 4, &data[i], NULL);
        assert(res == 1);
        assert(*(int*)table->entries[i]->key == data[i]);
    }
    
    htable_delete(table);
    
    return 0;
}
