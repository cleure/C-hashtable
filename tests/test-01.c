#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "config.h"
#include "hashtable.h"

int main(int argc, char **argv)
{
    int i;
    char *data[] = {
        "foo", "bar", "baz", "biz", "zap", "meow"
    };
    
    struct htable *table = htable_new(64);
    
    for (i = 0; i < sizeof(data)/sizeof(data[0]); i++) {
        htable_add(table, data[i], NULL, NULL);
    }
    
    for (i = 0; i < table->used; i++) {
        printf("%s\n", table->entries[i]->key);
    }

    htable_delete(table, NULL);
    return 0;
}
