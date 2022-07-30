#include <string.h>
#include "./include/symtable.h"
#include "./include/list.h"

symtab_T* symtab_init()
{
	symtab_T* result = (symtab_T*) malloc(sizeof(symtab_T));

	result->stack = (list_T**) malloc((result->allocated_size = 4)*sizeof(list_T*));
	result->size = 0;

	return result;
}

void symtab_free(symtab_T* table){
	for(int i = 0; i < table->size; i++){
		list_free(table->stack[i]);
	}
	free(table->stack);
}

symbol_T* symbol_init(char* name, var_type_T* type, int scope)
{
	symbol_T* result = (symbol_T*) malloc(sizeof(symbol_T));

	result->name = name;
	result->type = type;
	result->scope = scope;

	return result;
}


void symtab_enter_scope(symtab_T* table)
{
	table->stack[table->size++] = list_init(sizeof(symbol_T));
	if(table->size == table->allocated_size) table->stack = (list_T**) realloc(table->stack, (table->allocated_size *= 2)*sizeof(list_T*));
}

void symtab_exit_scope(symtab_T* table)
{
	list_T* s = symtab_get_scope(table);
	for(int i = 0; i < s->size; i++){
		symbol_T* sym = (symbol_T*) list_get(s, i);
		if(sym->scope != SCOPE_ARG) free(sym); 
	}
	list_free(s);
	table->size--;
}

void symtab_add_symbol(symtab_T* table, symbol_T* symbol)
{
	list_add(symtab_get_scope(table), (void*) symbol);
}

symbol_T* symtab_look_up(symtab_T* table, char* name)
{
	for(int i = table->size - 1; i >= 0; i--){
		list_T* list = table->stack[i];
		for(int j = 0; j < list->size; j++){
			symbol_T* sym = (symbol_T*) list_get(list, j);
			if(!strcmp(sym->name, name)) return sym;
		}
	}
	return NULL;
}

symbol_T* symtab_probe(symtab_T* table, char* name)
{
	list_T* top = symtab_get_scope(table);
	for(int i = 0; i < top->size; i++){
		symbol_T* sym = (symbol_T*) list_get(top, i);
		if(!strcmp(sym->name, name)) return sym;
	}
	return NULL;
}

list_T* symtab_get_scope(symtab_T* table)
{
	return table->stack[table->size - 1];
}
