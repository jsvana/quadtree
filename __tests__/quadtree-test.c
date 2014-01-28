#include "../quadtree.h"

#include <stdio.h>

int main(int argc, char **argv) {
	quadtree *q = quadtree_create();

	quadtree_free(q);
	return 0;
}
