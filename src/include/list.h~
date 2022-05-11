#ifndef __LIST_H__
#define __LIST_H__

#include <stdio.h>

typedef struct {
	void** list;
	size_t size;
	size_t allocated_size;
	size_t element_size;
} list_T;

list_T *list_init(size_t element_size);

void list_add(list_T* list, void* element);

void *list_get(list_T* list, size_t i);

void list_free(list_T* list);

#endif
