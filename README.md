C-hashtable
===========

A Hash Table Library in C, using MurmurHash3. For memory management, this uses the borrow method. It assumes the memory put into it will exist for the lifetime of the hash table, thus you may need to allocate and copy memory into temporary pointers to pass to htable_add() if the originating memory doesn't exist for the lifetime of the hash table. In this case, you can also specify a function pointer to your own function that will free allocated memory when the data is removed or replaced in the hash table.
