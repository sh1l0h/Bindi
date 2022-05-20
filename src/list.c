#include <stdlib.h>
#include "./include/list.h"

#define INITIAL_SIZE 8

list_T* list_init(size_t element_size)
{
	list_T* result = (list_T*) calloc(1, sizeof(list_T));
	result->list = calloc(8,element_size);
	result->allocated_size = INITIAL_SIZE;
	result->element_size = element_size;

	return result;
}

void list_add(list_T* list, void* element)
{
	list->list[list->size++] = element;
	if(list->size == list->allocated_size)
		list->list = realloc(list->list, (list->allocated_size *= 2)*list->element_size);
}
void *list_get(list_T* list, size_t i){
	return list->list[i];
}

void list_free(list_T* list)
{
	free(list->list);
	free(list);
}
