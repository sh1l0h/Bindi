#ifndef __TYPE_H__
#define __TYPE_H__

#include <stdlib.h>
#include "list.h"

typedef struct {
	char* name;
	size_t size;
	enum {
		T_PRIM,
		T_NONPRIM,
		T_PTR
	} type;
	list_T* subtypes;
} type_T;

typedef struct{
	char* name;
	type_T* type;
	size_t offset;
} subtype_T;

type_T* type_init(char* name, size_t size, int type);
void type_free(type_T* type);

subtype_T* subtype_init(char* name, type_T* type);

void type_add_sybtype(type_T* type, subtype_T* subtype);

#endif
