#include "./include/type.h"

type_T* type_init(char* name, size_t size, int type)
{
	type_T* result = (type_T*) malloc(sizeof(type_T));
	result->name = name;
	result->size = size;
	result->type = type;
	result->subtypes = list_init(sizeof(subtype_T*));

	return result;
}

void type_free(type_T* type)
{
	for(int i = 0; i < type->subtypes->size; i++) free(list_get(type->subtypes,i));
	list_free(type->subtypes);
	free(type);
}

subtype_T* subtype_init(char* name, type_T* type)
{
	subtype_T* result = (subtype_T*) malloc(sizeof(subtype_T));
	result->name = name;
	result->type = type;
	result->offset = 0;
	return result;
}

void type_add_sybtype(type_T* type, subtype_T* subtype)
{
	list_T* subs = type->subtypes;

	if(subs->size != 0){
		subtype_T* last = (subtype_T*)subs->list[subs->size-1];
		subtype->offset = last->offset + last->type->size;
	}

	list_add(type->subtypes, subtype);
}
