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

void copyfn(struct htable_entry *dst, void *key, void *data)
{
    dst->key = strdup((char *)key);
    dst->data = malloc(4);
    memcpy(dst->data, data, 4);
}

void freefn(struct htable_entry *ent)
{
    free(ent->key);
    free(ent->data);
}

int main(int argc, char **argv)
{
    int i, len, res;
    struct htable *table;
    int data;
    
    table = htable_new(512, 0, &htable_cstring_cmpfn, &copyfn, &freefn);
    assert(table != NULL);
    
    len = sizeof(string_data)/sizeof(string_data[0]);
    for (i = 0; i < len; i++) {
        data = strlen(string_data[i]);
        res = htable_add(table, strlen(string_data[i]), string_data[i], &data);
        assert(res == 1);
        assert(strcmp(table->entries[i]->key, string_data[i]) == 0);
    }
    
    for (i = 0; i < len; i++) {
        data = strlen(string_data[i]);
        assert(strcmp(table->entries[i]->key, string_data[i]) == 0);
        assert(*(int *)table->entries[i]->data == strlen(string_data[i]));
    }
    
    for (i = 0; i < len; i++) {
        data = strlen(string_data[i]) * 2;
        res = htable_add(table, strlen(string_data[i]), string_data[i], &data);
        assert(res == 1);
        assert(strcmp(table->entries[i]->key, string_data[i]) == 0);
    }
    
    for (i = 0; i < len; i++) {
        data = strlen(string_data[i]);
        assert(strcmp(table->entries[i]->key, string_data[i]) == 0);
        assert(*(int *)table->entries[i]->data == strlen(string_data[i]) * 2);
    }
    
    htable_delete(table);
    
    return 0;
}
