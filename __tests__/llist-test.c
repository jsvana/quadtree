#include "../llist.h"

#include <stdio.h>
#include <stdlib.h>

#ifndef assert
#define assert(X) \
	if (!(X)) { \
		fprintf(stderr, "Assertion failed: " # X "\n"); \
		exit(1); \
	}
#endif

int main(int argc, char **argv) {
	llist *l = llist_create();

	assert(llist_size(l) == 0);
	assert(llist_is_empty(l));

	int i;
	i = 1;
	llist_add_to_front(l, &i, sizeof(int));
	assert(llist_size(l) == 1);
	assert(*(int *)llist_first(l) == 1);
	i = 2;
	llist_add_to_front(l, &i, sizeof(int));
	i = 3;
	llist_add_to_front(l, &i, sizeof(int));
	assert(*(int *)llist_first(l) == 3);

	llist *l2 = llist_create();
	i = 4;
	llist_add_to_back(l2, &i, sizeof(int));
	assert(llist_size(l2) == 1);
	assert(*(int *)llist_last(l2) == 4);
	i = 5;
	llist_add_to_back(l2, &i, sizeof(int));
	i = 6;
	llist_add_to_back(l2, &i, sizeof(int));
	assert(*(int *)llist_last(l2) == 6);

	llist_iter_prep(l);
	llist_append(l, l2);
	llist_node *n = llist_iter(l);
	assert(n != NULL);

	int vals[6] = {3, 2, 1, 4, 5, 6};
	i = 0;

	while (n != NULL) {
		assert(vals[i] == *(int *)n->data);
		++i;
		n = n->inext;
	}

	llist_free(l);
	llist_free(l2);

	printf("All tests pass\n");
	return 0;
}
