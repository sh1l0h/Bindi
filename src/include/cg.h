#ifndef __CG_H__
#define __CG_H__
#include <stdio.h>
#include "ast.h"
#include "symtable.h"

typedef struct {
	FILE *output_file;
} CG_T;

extern int is_value_needed;
extern int lable_count;
extern size_t curr_size;

CG_T* CG_init(char* file_name);
const char* CG_size_to_extension(size_t size);
const char* CG_size_register(size_t size, size_t reg);
void CG_generate_code(CG_T* cg, AST_T* tree, symtab_T* table);
void CG_free(CG_T* cg);

#endif
