#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <pthread.h>
#include <time.h>

#include "config.h"
#include "hashtable.h"
#include "MurmurHash3.h"

    char *data[] = {
        "foo", "bar", "baz", "biz", "zap", "meow",
        "camel", "consise", "zebra", "zephyr",
        "bellpepper", "paprika", "meatball", "bmx"
    };
    
    char *data2[] = {
        "tomatoe", "avacado", "trex", "cereal",
        "cheesesteak", "rump", "last-stand", "wild",
        "turkey", "bourbon", "laughter", "white"
    };

void freefn(struct htable_entry *a)
{
    return;
}

void *add_entries1(void *ptr)
{
    int i;
    int res;
    struct htable *table = ptr;
    
    for (i = 0; i < sizeof(data)/sizeof(data[0]); i++) {
        res = htable_add_loop(table, data[i], NULL, NULL, 10);
    
        if (!res) {
            printf("Failed to add %s\n", data[i]);
        } else {
            printf("Added %s in %d tries\n", data[i], res);
        }
        //htable_add(table, data[i], NULL, NULL);
    }
    
    printf("Table Size: %d\n", table->size);
    
    return NULL;
}

void *add_entries2(void *ptr)
{
    int i;
    struct htable *table = ptr;
    
    for (i = 0; i < sizeof(data2)/sizeof(data2[0]); i++) {
        htable_add(table, data2[i], NULL, NULL);
    }
    
    return NULL;
}

int main(int argc, char **argv)
{
    int i;
    pthread_t thread1, thread2;
    struct htable *table = htable_new(4);
    
    pthread_create(&thread1, NULL, add_entries1, table);
    //pthread_create(&thread2, NULL, add_entries2, table);
    
    pthread_join(thread1, NULL);
    //pthread_join(thread2, NULL);
    
    for (i = 0; i < table->used; i++) {
        printf("%p ", table->entries[i]);
        printf("%d\t%s\n", table->entries[i]->hash, (char *)table->entries[i]->key);
    }
    
    /*printf("%d %d\n", table->used, sizeof(data)/sizeof(data[0]) + sizeof(data2)/sizeof(data2[0]));*/
    htable_delete(table, &freefn);
    
    return 0;
}
