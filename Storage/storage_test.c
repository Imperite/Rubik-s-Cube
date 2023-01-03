#include "storage.h"

#include <string.h>
#include <stdio.h>


int cmp(void* c1, void* c2) {
    return strcmp(c1, c2);
}

void print(void* s) {
    printf("%s", s);
}

int main(int argc, char const* argv[])
{
    Storage s = storage_create();
    storage_print(s, print);
    storage_insert(s, "baa", cmp);
    storage_print(s, print);

    storage_insert(s, "aba", cmp);
    storage_print(s, print);
    storage_insert(s, "aca", cmp);
    storage_print(s, print);

    printf("%d\n", storage_contains(s, "abc", cmp));

    return 0;
}
