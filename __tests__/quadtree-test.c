#include "../llist.h"
#include "../quadtree.h"

#include <stdio.h>
#include <stdlib.h>

#ifndef assert
#define assert(X) \
	if (!(X)) { \
		fprintf(stderr, "Assertion failed: " # X "\n"); \
		exit(1); \
	}
#endif

void printNode(quadtree_node *qn) {
	if (qn == NULL) {
		printf(" ( NULL )");
	} else {
		printf(" ( [");//{ x: %.2f, y: %.2f, w: %.2f, h: %.2f } [", qn->bounds.x,
			//qn->bounds.y, qn->bounds.w, qn->bounds.h);
		if (!llist_is_empty(qn->points)) {
			llist_node *ln = llist_iter(qn->points);
			while (ln != qn->points->tail) {
				printf(" %c", *(char *)((quadtree_data *)ln->data)->data);
				ln = ln->next;
			}
		}
		printf(" ]");

		for (int i = 0; i < 4; i++) {
			printNode(qn->children[i]);
		}
		printf(" )");
	}
}

int main(int argc, char **argv) {
	quadtree *q = quadtree_create(0, 0, 50, 50);
	assert(q != NULL);

	char c = 'a';
	int ret = quadtree_insert(q, &c, sizeof(char), 10, 42);
	assert(ret == QUADTREE_OK);
	c = 'b';
	ret = quadtree_insert(q, &c, sizeof(char), 10, 42);
	assert(ret == QUADTREE_OK);
	c = 'c';
	ret = quadtree_insert(q, &c, sizeof(char), 2, 3);
	assert(ret == QUADTREE_OK);
	c = 'd';
	ret = quadtree_insert(q, &c, sizeof(char), 3, 13);
	c = 'e';
	ret = quadtree_insert(q, &c, sizeof(char), 14, 23);
	c = 'f';
	ret = quadtree_insert(q, &c, sizeof(char), 23, 23);

	quadtree_node *root = q->root;
	printNode(root);

	quadtree_free(q);
	return 0;
}
