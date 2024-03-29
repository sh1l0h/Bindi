#ifndef __CODE_GENERATOR_H__
#define __CODE_GENERATOR_H__
#include <stdio.h>
#include "ast.h"
#include "symtable.h"
#include "reg_stack.h"
#include "function_frame.h"

#define MAX(a,b) a > b ? a : b

typedef struct{
	FILE* output_file;
	symtab_T* table;
	rstack_T* stack;
	list_T* types;
	list_T* str_lits;
	list_T* funs;
	int lable_count;
	int is_value_needed;
	size_t curr_size;
} CG_T;

const char* CG_register_to_str(int reg, size_t size);

CG_T* CG_init(char* file_name);
void CG_close(CG_T* cg);

type_T* CG_find_type(CG_T* cg, char* name);
fun_frame_T* CG_find_function(CG_T* cg, char* name);

const char* CG_size_to_extension(size_t size);

var_type_T* CG_get_var_type(CG_T* cg, AST_T* tree);
int CG_code(CG_T* cg, AST_T* tree);

#endif
