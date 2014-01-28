#include "quadtree.h"

#include <stdio.h>
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

static int quadtree_data_same_location(quadtree_data *d1, double x, double y) {
	return d1->x == x && d1->y == y;
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

static void quadtree_node_set_bounds(quadtree_node *qn, double x, double y,
		double w, double h) {
	if (!qn) {
		return;
	}

	qn->bounds.x = x;
	qn->bounds.y = y;
	qn->bounds.w = w;
	qn->bounds.h = h;
}

quadtree *quadtree_create(double x, double y, double w, double h) {
	quadtree *q = malloc(sizeof(quadtree));
	if (!q) {
		return NULL;
	}

	q->root = quadtree_node_create();
	if (!q->root) {
		free(q);
		return NULL;
	}

	quadtree_node_set_bounds(q->root, x, y, w, h);

	return q;
}

static int quadtree_node_point_inside(quadtree_node *qn, double x, double y) {
	if (!qn) {
		return FALSE;
	}

	return qn->bounds.x <= x && qn->bounds.x + qn->bounds.w > x
		&& qn->bounds.y <= y && qn->bounds.y + qn->bounds.h > y;
}

static int point_inside(double sX, double sY, double sW, double sH, double x,
		double y) {
	return sX <= x && sX + sW > x
		&& sY <= y && sY + sH > y;
}

static int quadtree_node_has_children(quadtree_node *qn) {
	if (!qn) {
		return FALSE;
	}

	for (int i = 0; i < 4; i++) {
		if (qn->children[i]) {
			return TRUE;
		}
	}

	return FALSE;
}

static int quadtree_insert_node(quadtree *q, quadtree_node *qn, void *data,
		size_t size, double x, double y) {
	if (!qn) {
		return QUADTREE_ERROR;
	}

	if (!quadtree_node_has_children(qn) && llist_is_empty(qn->points)) {
		quadtree_data *qd = quadtree_data_create(data, size, x, y);
		if (!qd) {
			return QUADTREE_ERROR;
		}
		llist_add_to_back(qn->points, qd, sizeof(quadtree_data));
		free(qd);
		++q->size;

		return QUADTREE_OK;
	} else {
		quadtree_data *d = llist_first(qn->points);
		if (d && quadtree_data_same_location(d, x, y)) { // Same point
			quadtree_data *qd = quadtree_data_create(data, size, x, y);
			if (!qd) {
				return QUADTREE_ERROR;
			}
			llist_add_to_back(qn->points, qd, sizeof(quadtree_data));
			free(qd);
			return QUADTREE_OK;
		} else {
			quadtree_data *e = llist_first(qn->points);
			// Split node
			double minX = qn->bounds.x;
			double minY = qn->bounds.y;
			double midX = qn->bounds.x + qn->bounds.w / 2;
			double midY = qn->bounds.y + qn->bounds.h / 2;
			double maxX = qn->bounds.x + qn->bounds.w;
			double maxY = qn->bounds.y + qn->bounds.h;
			double xDim[4][2] = {
				{ midX, maxX - midX },
				{ minX, midX - minX },
				{ minX, midX - minX },
				{ midX, maxX - midX }
			};
			double yDim[4][2] = {
				{ midY, maxY - midY },
				{ midY, maxY - midY },
				{ minY, midY - minY },
				{ minY, midY - minY }
			};

			quadtree_data *d = llist_first(qn->points);

			int ret = QUADTREE_ERROR;

			for (int i = 0; i < 4; i++) {
				if (point_inside(xDim[i][0], yDim[i][0], xDim[i][1], yDim[i][1], x, y)
						|| (d && point_inside(xDim[i][0], yDim[i][0], xDim[i][1], yDim[i][1],
							d->x, d->y))) {
					if (!qn->children[i]) {
						qn->children[i] = quadtree_node_create();
					}
						quadtree_node_set_bounds(qn->children[i], xDim[i][0], yDim[i][0],
							xDim[i][1], yDim[i][1]);
						if (point_inside(xDim[i][0], yDim[i][0], xDim[i][1], yDim[i][1], x,
								y)) {
							quadtree_insert_node(q, qn->children[i], data, size, x, y);
							ret = QUADTREE_OK;
						} else if (point_inside(xDim[i][0], yDim[i][0], xDim[i][1],
								yDim[i][1], d->x, d->y)) {
							// Move contained points into child node
							qn->children[i]->points = qn->points;
							qn->points = NULL;
							ret = QUADTREE_OK;
						}
				}
			}
			return ret;
		}
	}

	// Shouldn't happen
	return QUADTREE_ERROR;
}

static int quadtree_rect_contains(quadtree_rect *r, double x, double y) {
	if (!r) {
		return FALSE;
	}

	return r->x <= x && r->x + r->h >= x && r->y <= y && r->y + r->h >= y;
}

static int quadtree_rect_contains_node(quadtree_rect *r, quadtree_node *qn) {
	if (!r || !qn) {
		return FALSE;
	}

	return quadtree_rect_contains(r, qn->bounds.x, qn->bounds.y)
		|| quadtree_rect_contains(r, qn->bounds.x + qn->bounds.w, qn->bounds.y)
		|| quadtree_rect_contains(r, qn->bounds.x + qn->bounds.w,
			qn->bounds.y + qn->bounds.h)
		|| quadtree_rect_contains(r, qn->bounds.x, qn->bounds.y + qn->bounds.h);
}

int quadtree_insert(quadtree *q, void *data, size_t size, double x, double y) {
	if (!q || !quadtree_rect_contains(&q->root->bounds, x, y)) {
		return QUADTREE_ERROR;
	}

	return quadtree_insert_node(q, q->root, data, size, x, y);
}

llist *quadtree_retrieve_node(quadtree *q, quadtree_node *qn,
		quadtree_rect *r, llist *list) {
	if (!qn) {
		return list;
	}

	if (qn->points && !llist_is_empty(qn->points)) {
		quadtree_data *d = llist_first(qn->points);
		if (quadtree_rect_contains(r, d->x, d->y)) {
			llist_append(list, qn->points);
		}
	} else {
		for(int i = 0; i < 4; i++) {
			if (quadtree_rect_contains_node(r, qn->children[i])) {
				quadtree_retrieve_node(q, qn->children[i], r, list);
			}
		}
	}

	return list;
}

llist *quadtree_retrieve(quadtree *q, quadtree_rect *r) {
	if (!q || !r) {
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
