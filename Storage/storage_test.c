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
    Storage s = init_Storage();
    print_Storage(s, print);
    insert_Storage(s, "baa", cmp);
    print_Storage(s, print);

    insert_Storage(s, "aba", cmp);
    print_Storage(s, print);
    insert_Storage(s, "aca", cmp);
    print_Storage(s, print);

    printf("%d\n", contains_Storage(s, "abc", cmp));

    return 0;
}
