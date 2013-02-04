#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "config.h"
#include "hashtable.h"
#include "MurmurHash3.h"

char *string_data[] = {
        "foo", "bar", "baz", "biz", "zap", "meow",
        "camel", "consise", "zebra", "zephyr",
        "bellpepper", "paprika", "meatball", "bmx",
        "tomatoe", "avacado", "trex", "cereal",
        "cheesesteak", "rump", "last-stand", "wild",
        "turkey", "bourbon", "laughter", "white",
        "scotch", "rye"
};

char *string_data2[] = {
        "foo", "bar", "baz", "biz", "zap", "meow",
        "camel", "consise", "zebra", "zephyr"
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
    struct htable *table2;
    struct htable_entry **intersect;
    
    table = htable_new(512, 0, &htable_cstring_cmpfn, &string_copyfn, &string_freefn);
    assert(table != NULL);
    
    table2 = htable_new(512, 0, &htable_cstring_cmpfn, &string_copyfn, &string_freefn);
    assert(table2 != NULL);
    
    len = sizeof(string_data)/sizeof(string_data[0]);
    for (i = 0; i < len; i++) {
        res = htable_add(table, strlen(string_data[i]), string_data[i], NULL);
        assert(res == 1);
        assert(strcmp(table->entries[i]->key, string_data[i]) == 0);
    }
    
    len = sizeof(string_data2)/sizeof(string_data2[0]);
    for (i = 0; i < len; i++) {
        res = htable_add(table2, strlen(string_data2[i]), string_data2[i], NULL);
        assert(res == 1);
        assert(strcmp(table2->entries[i]->key, string_data2[i]) == 0);
    }
    
    intersect = htable_intersect(table, table2);
    i = 0;
    while (intersect[i]) {
        assert(strcmp(string_data[i], intersect[i]->key) == 0);
        i++;
    }
    
    free(intersect);
    htable_delete(table);
    htable_delete(table2);
    
    return 0;
}
