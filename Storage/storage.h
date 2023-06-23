#ifndef _storage_h_
#define _storage_h_

#include <stdbool.h>

typedef struct storage* Storage;

typedef void Item;

typedef int(*Comparator)(const Item*, const Item*);

typedef void(*Function)(Item*);

// create
Storage storage_create();
// destroy
void storage_destroy(Storage storage);
// add
void storage_insert(Storage storage, Item* obj, Comparator compare);
// contains; will use a binary search to find in storage.
bool storage_contains(const Storage storage, const Item* to_find, Comparator compare);

//removes the object from the storage and returns it
Item* storage_replace(Storage storage, const Item* obj, Comparator compare);

//returns a pointer to the location of obj in memory using binary search.
Item** storage_location_of(const Storage storage, const Item* obj, Comparator compare);

// calls function do_on on the location in memory provided. Does not guarantee location is not null.
Item* storage_do(Storage storage, const Item* obj, Comparator compare, Item* (*do_on)(Storage, const Item*, Item**));

int storage_size(const Storage storage);

void storage_print(const Storage storage, Function print);

void storage_for_each(const Storage storage, Function function);

#endif