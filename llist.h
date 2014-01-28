#pragma once

#include <sys/types.h>

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

typedef struct llist_node llist_node;
struct llist_node {
	void *data;
	size_t size;
	llist_node *prev;
	llist_node *next;
	llist_node *inext;
};

typedef struct llist llist;
struct llist {
	llist_node *head;
	llist_node *tail;
	int size;
};

llist *llist_create();

void llist_add_to_back(llist *l, void *data, size_t size);
void llist_add_to_front(llist *l, void *data, size_t size);

void llist_append(llist *dest, llist *other);

int llist_size(llist *l);
int llist_is_empty(llist *l);

void llist_free(llist *l);
