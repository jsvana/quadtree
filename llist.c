#include "llist.h"

#include <stdlib.h>
#include <string.h>

static llist_node *llist_node_create() {
	llist_node *ln = malloc(sizeof(llist_node));
	if (!ln) {
		return NULL;
	}

	ln->data = NULL;
	ln->size = 0;
	ln->prev = NULL;
	ln->next = NULL;
	ln->inext = NULL;

	return ln;
}

llist *llist_create() {
	llist *l = malloc(sizeof(llist));
	if (!l) {
		return NULL;
	}

	l->head = llist_node_create();
	if (!l->head) {
		free(l);
		return NULL;
	}

	l->tail = llist_node_create();
	if (!l->tail) {
		free(l->head);
		free(l);
		return NULL;
	}

	l->head->next = l->tail;
	l->tail->prev = l->head;

	l->size = 0;

	return l;
}

void llist_add_to_back(llist *l, void *data, size_t size) {
	if (!l) {
		return;
	}

	llist_node *ln = llist_node_create();
	ln->data = malloc(size);
	if (!ln->data) {
		// TODO: Handle failure
		return;
	}
	ln->size = size;

	llist_node *prev = l->tail->prev;
	prev->next = ln;
	ln->prev = prev;
	l->tail->prev = ln;
	ln->next = l->tail;

	++l->size;
}

void llist_add_to_front(llist *l, void *data, size_t size) {
	if (!l) {
		return;
	}

	llist_node *ln = llist_node_create();
	ln->data = malloc(size);
	if (!ln->data) {
		// TODO: Handle failure
		return;
	}
	ln->size = size;

	llist_node *next = l->head->next;
	next->prev = ln;
	ln->next = next;
	l->head->next = ln;
	ln->prev = l->head;

	++l->size;
}

static void llist_node_free(llist_node *ln) {
	if (!ln) {
		return;
	}

	if (ln->data) {
		free(ln->data);
	}

	free(ln);
}

void llist_append(llist *dest, llist *other) {
	if (!dest || !other || llist_is_empty(other)) {
		return;
	}

	llist_node *ln = dest->tail->prev;
	ln->inext = other->head->next;
	ln = ln->inext;
	while (ln != other->tail) {
		ln->inext = ln->next;
		ln = ln->inext;
	}
}

int llist_size(llist *l) {
	if (!l) {
		return 0;
	}

	return l->size;
}

int llist_is_empty(llist *l) {
	if (!l) {
		return TRUE;
	}

	return llist_size(l) == 0;
}

void llist_free(llist *l) {
	if (!l) {
		return;
	}

	if (llist_size(l) > 0) {
		llist_node *ln = l->head->next;
		llist_node *next;
		while (ln != l->tail) {
			next = ln->next;
			llist_node_free(ln);
			ln = next;
		}
	}

	free(l);
}
