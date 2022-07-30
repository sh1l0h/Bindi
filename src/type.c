#include <string.h>
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

subtype_T* subtype_init(char* name, var_type_T* type)
{
	subtype_T* result = (subtype_T*) malloc(sizeof(subtype_T));
	result->name = name;
	result->type = type;
	result->offset = 0;
	return result;
}

void type_add_subtype(type_T* type, subtype_T* subtype)
{
	list_T* subs = type->subtypes;

	if(subs->size != 0){
		subtype_T* last = (subtype_T*)subs->list[subs->size-1];
		subtype->offset = last->offset + var_type_size(last->type);
	}

	list_add(type->subtypes, subtype);
}

subtype_T* type_find_subtype(type_T* type, char* name)
{
	for(int i = 0; i < type->subtypes->size; i++){
		subtype_T* sub = (subtype_T*) list_get(type->subtypes,i);
		if(!strcmp(sub->name, name)) return sub;
	}
	return NULL;
}

var_type_T* var_type_init(type_T* type, size_t ptr_depth, int is_lvalue)
{
	var_type_T* vtype = (var_type_T*) malloc(sizeof(var_type_T));
	vtype->type = type;
	vtype->ptr_depth = ptr_depth;
	vtype->is_lvalue = is_lvalue;
	return vtype;
}

var_type_T* var_type_copy(var_type_T* vtype)
{
	var_type_T* result = (var_type_T*) malloc(sizeof(var_type_T));
	result->type = vtype->type;
	result->ptr_depth = vtype->ptr_depth;
	result->is_lvalue = vtype->is_lvalue;
	return result;
}

int var_type_cmp(var_type_T* v1, var_type_T* v2)
{
	return v1->type == v2->type && v1->ptr_depth == v2->ptr_depth;
}

size_t var_type_size(var_type_T* t)
{
	if(t->ptr_depth != 0) return 8;
	return t->type->size;
}
