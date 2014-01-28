#include "quadtree.h"

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

static quadtree_data *quadtree_data_create(void *data, size_t size, double x,
		double y) {
	quadtree_data *qd = malloc(sizeof(quadtree_data));
	if (!qd) {
		return NULL;
	}

	if (data != NULL && size != 0) {
		qd->data = malloc(size);
		if (!qd->data) {
			free(qd);
			return NULL;
		}
		memcpy(qd->data, data, size);
	} else {
		qd->data = NULL;
		qd->size = 0;
	}

	qd->size = size;
	qd->x = x;
	qd->y = y;

	return qd;
}

static quadtree_node *quadtree_node_create() {
	quadtree_node *qn = malloc(sizeof(quadtree_node));
	if (!qn) {
		return NULL;
	}

	qn->points = llist_create();
	if (!qn->points) {
		free(qn);
		return NULL;
	}

	qn->children = malloc(sizeof(quadtree_node *) * 4);
	if (!qn->children) {
		free(qn->points);
		free(qn);
		return NULL;
	}

	for (int i = 0; i < 4; i++) {
		qn->children[i] = NULL;
	}

	return qn;
}

quadtree *quadtree_create() {
	quadtree *q = malloc(sizeof(quadtree));
	if (!q) {
		return NULL;
	}

	q->root = quadtree_node_create();
	if (!q->root) {
		free(q);
		return NULL;
	}

	return q;
}

static int quadtree_insert_node(quadtree *q, quadtree_node *qn, void *data,
		size_t size, double x, double y) {
	if (!qn) {
		return QUADTREE_ERROR;
	}

	if (llist_is_empty(qn->points)) {
		quadtree_data *qd = quadtree_data_create(data, size, x, y);
		if (!qd) {
			return QUADTREE_ERROR;
		}
		llist_add_to_back(qn->points, qd, sizeof(quadtree_data));
		free(qd);
		++q->size;
		qn->x = x;
		qn->y = y;

		return QUADTREE_OK;
	} else {
		if (qn->x == x && qn->y == y) { // Same point
			quadtree_data *qd = quadtree_data_create(data, size, x, y);
			if (!qd) {
				return QUADTREE_ERROR;
			}
			llist_add_to_back(qn->points, qd, sizeof(quadtree_data));
			free(qd);
			return QUADTREE_OK;
		} else if (qn->x < x && qn->y < y) { // Above right
			if (!qn->children[0]) {
				qn->children[0] = quadtree_node_create();
			}
			quadtree_insert_node(q, qn->children[0], data, size, x, y);
		} else if (qn->x > x && qn->y < y) { // Above left
			if (!qn->children[1]) {
				qn->children[1] = quadtree_node_create();
			}
			quadtree_insert_node(q, qn->children[1], data, size, x, y);
		} else if (qn->x > x && qn->y > y) { // Below left
			if (!qn->children[2]) {
				qn->children[2] = quadtree_node_create();
			}
			quadtree_insert_node(q, qn->children[2], data, size, x, y);
		} else if (qn->x < x && qn->y > y) { // Below right
			if (!qn->children[3]) {
				qn->children[3] = quadtree_node_create();
			}
			quadtree_insert_node(q, qn->children[3], data, size, x, y);
		}
	}

	// Shouldn't happen
	return QUADTREE_ERROR;
}

int quadtree_insert(quadtree *q, void *data, size_t size, double x, double y) {
	if (!q) {
		return QUADTREE_ERROR;
	}

	return quadtree_insert_node(q, q->root, data, size, x, y);
}

llist *quadtree_retrieve_node(quadtree *q, quadtree_node *qn,
		quadtree_rect *r, llist *list) {
	if (!qn || llist_is_empty(qn->points)) {
		return list;
	}

	if (r->x <= qn->x && r->x + r->w >= qn->x && r->y <= qn->y && r->y + r->w
			>= qn->y) {
		llist_append(list, qn->points);
		for(int i = 0; i < 4; i++) {
			quadtree_retrieve_node(q, qn->children[i], r, list);
		}
	}

	return list;
}

llist *quadtree_retrieve(quadtree *q, quadtree_rect *r) {
	if (!q) {
		return NULL;
	}

	llist *list = llist_create();
	if (!list) {
		return NULL;
	}

	return quadtree_retrieve_node(q, q->root, r, list);
}

static void quadtree_node_free(quadtree_node *qn) {
	if (!qn) {
		return;
	}

	llist_free(qn->points);
	free(qn->children);
	free(qn);
}

void quadtree_free(quadtree *q) {
	// TODO: Free
}
