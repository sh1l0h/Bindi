#ifndef __TYPE_H__
#define __TYPE_H__

#include <stdlib.h>
#include "list.h"

typedef struct {
	char* name;
	list_T* subtypes;
	size_t size;
	enum {
		T_PRIM,
		T_STRUCT
	} type;
} type_T;

typedef struct {
	type_T* type;
	size_t ptr_depth;
	int is_lvalue;
} var_type_T;

typedef struct{
	char* name;
	var_type_T* type;
	size_t offset;
} subtype_T;

type_T* type_init(char* name, size_t size, int type);
void type_free(type_T* type);

subtype_T* subtype_init(char* name, var_type_T* type);

void type_add_subtype(type_T* type, subtype_T* subtype);

subtype_T* type_find_subtype(type_T* type, char* name);

var_type_T* var_type_init(type_T* type, size_t ptr_depth, int is_lvalue);
var_type_T* var_type_copy(var_type_T* vtype);
int var_type_cmp(var_type_T* v1, var_type_T* v2);
size_t var_type_size(var_type_T* t);

#endif
