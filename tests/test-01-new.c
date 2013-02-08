#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "config.h"
#include "hashtable.h"

int main(int argc, char **argv)
{
    struct htable *table;
    
    srand(time(NULL));
    table = htable_new(16, rand(), &htable_cstring_cmpfn, NULL, NULL);
    assert(table != NULL);
    htable_delete(table);
    
    return 0;
}
