#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

#include <stdlib.h>
#include "list.h"
#include "type.h"


typedef struct {
	list_T** stack;
	size_t allocated_size;
	size_t size;
} symtab_T;

typedef struct {
	char* name;
	var_type_T* type;

	enum {
		SCOPE_GLOBAL,
		SCOPE_ARG,
		SCOPE_LOCAL
	} scope;

	size_t offset;
} symbol_T;

symtab_T* symtab_init();
void symtab_free();

symbol_T* symbol_init(char* name, var_type_T* type, int scope);

void symtab_enter_scope(symtab_T* table);
void symtab_exit_scope(symtab_T* table);

void symtab_add_symbol(symtab_T* table, symbol_T* symbol);

symbol_T* symtab_look_up(symtab_T* table, char* name);

symbol_T* symtab_probe(symtab_T* table, char* name);

list_T* symtab_get_scope(symtab_T* table);

#endif
