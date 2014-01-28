#pragma once

#include "llist.h"

#include <sys/types.h>

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define QUADTREE_ERROR -1
#define QUADTREE_OK 0

typedef struct quadtree_rect quadtree_rect;
struct quadtree_rect {
	double x;
	double y;
	double w;
	double h;
};

typedef struct quadtree_data quadtree_data;
struct quadtree_data {
	void *data;
	size_t size;
	double x;
	double y;
};

/**
 * Quadtree node definition.
 * Children are:
 *   0 -> top right
 *   1 -> top left
 *   2 -> bottom left
 *   3 -> bottom right
 */
typedef struct quadtree_node quadtree_node;
struct quadtree_node {
	llist *points;
	quadtree_rect bounds;
	quadtree_node **children;
};

typedef struct quadtree quadtree;
struct quadtree {
	quadtree_node *root;
	int size;
};

quadtree *quadtree_create();

int quadtree_insert(quadtree *q, void *data, size_t size, double x, double y);

//quadtree_point *quadtree_retrieve(quadtree *q, )

void quadtree_free(quadtree *q);
