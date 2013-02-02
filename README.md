C-hashtable
===========

A Hash Table Library in C, using MurmurHash3. For memory management, this uses the borrow method. It assumes the memory put into it will exist for the lifetime of the hash table, thus you may need to allocate and copy memory into temporary pointers to pass to htable_add() if the originating memory doesn't exist for the lifetime of the hash table. In this case, you can also specify a function pointer to your own function that will free allocated memory when the data is removed or replaced in the hash table.

Methods:

	struct htable * htable_new(uint32_t size);

	struct htable * htable_clone(struct htable *, void (*copyfn)(struct htable_entry *dst, char *key, void *data));

	void htable_delete(struct htable *);

	int htable_resize(struct htable *, uint8_t load_thresh, uint32_t new_size);

	int htable_add(struct htable *, char *key, void *data, void (*freefn)(struct htable_entry *));

	int htable_remove(struct htable *, char *key, void (*freefn)(struct htable_entry *));

	struct htable_entry * htable_get(struct htable *, char *key);

	struct htable_entry ** htable_intersect(struct htable *a, struct htable *b);

	struct htable_entry ** htable_difference(struct htable *a, struct htable *b);
