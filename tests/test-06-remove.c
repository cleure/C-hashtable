#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "config.h"
#include "hashtable.h"

char *string_data[] = {
        "foo", "bar", "baz", "biz", "zap", "meow",
        "camel", "consise", "zebra", "zephyr",
        "bellpepper", "paprika", "meatball", "bmx",
        "tomatoe", "avacado", "trex", "cereal",
        "cheesesteak", "rump", "last-stand", "wild",
        "turkey", "bourbon", "laughter", "white",
        "scotch", "rye"
};

void string_copyfn(struct htable_entry *dst, void *key, void *data)
{
    dst->key = strdup((char *)key);
}

void string_freefn(struct htable_entry *ent)
{
    free(ent->key);
}

int main(int argc, char **argv)
{
    int i, len, res;
    struct htable *table;
    
    table = htable_new(512, 0, &htable_cstring_cmpfn, &string_copyfn, &string_freefn);
    assert(table != NULL);
    
    len = sizeof(string_data)/sizeof(string_data[0]);
    for (i = 0; i < len; i++) {
        res = htable_add(table, strlen(string_data[i]), string_data[i], NULL);
        assert(res == 1);
        assert(strcmp(table->entries[i]->key, string_data[i]) == 0);
    }
    
    for (i = 0; i < len; i++) {
        res = htable_remove(table, strlen(string_data[i]), string_data[i]);
        assert(res == 1);
        
        if (table->used > len/2) {
            assert(strcmp(table->entries[i]->key, string_data[(len-i-1)]) == 0);
        }
    }
    
    htable_delete(table);
    
    return 0;
}
