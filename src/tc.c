#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "./include/tc.h"
#include "./include/symtable.h"

int TC_match_type(var_type_T* vtype, size_t ptr_depth, int argc, ...)
{
	va_list l;
	va_start(l, argc);
	for(int i = 0; i < argc; i++){
		char* name = va_arg(l, char*);
		if(!strcmp(name, vtype->type->name) && vtype->ptr_depth == ptr_depth){
			va_end(l);
			return 1;
		}}
	return 0;
}

var_type_T* TC_get_type_BE(CG_T* cg, AST_T* tree)
{
	if(tree->children->size == 1) return TC_get_type_BT(cg, AST_get_child(tree, 0));

	var_type_T* t1 = TC_get_type_BT(cg, AST_get_child(tree,0));
	var_type_T* t2 = TC_get_type_BE(cg, AST_get_child(tree,2));
	if(!TC_match_type(t1, 0, 1, "bool") || !TC_match_type(t2,0,1,"bool")){
		printf("%s:%ld:%ld: error: operator \'or\' cannot be applied to operands of type %s and %s\n",tree->file, tree->line, tree->column, t1->type->name, t2->type->name);
		exit(1);
	}
	free(t2);
	t1->is_lvalue = 0;
	return t1;
}

var_type_T* TC_get_type_BT(CG_T* cg, AST_T* tree)
{
	if(tree->children->size == 1) return TC_get_type_BU(cg, AST_get_child(tree, 0));

	var_type_T* t1 = TC_get_type_BU(cg, AST_get_child(tree,0));
	var_type_T* t2 = TC_get_type_BT(cg, AST_get_child(tree,2));
	if(!TC_match_type(t1, 0, 1, "bool") || !TC_match_type(t2,0,1,"bool")){
		printf("%s:%ld:%ld: error: operator \'and\' cannot be applied to operands of type %s and %s\n",tree->file, tree->line, tree->column, t1->type->name, t2->type->name);
		exit(1);
	}
	t1->is_lvalue = 0;
	free(t2);
	return t1;
}

var_type_T* TC_get_type_BU(CG_T* cg, AST_T* tree)
{
	if(tree->children->size == 1) return TC_get_type_BP(cg, AST_get_child(tree, 0));

	var_type_T* t = TC_get_type_BU(cg, AST_get_child(tree,1));
	if(!TC_match_type(t, 0,1, "bool")){
		printf("%s:%ld:%ld: error: operator \'!\' cannot be applied to operand of type %s\n", tree->file, tree->line, tree->column, t->type->name);
		exit(1);
	}
	t->is_lvalue = 0;
	return t;
}

var_type_T* TC_get_type_BP(CG_T* cg, AST_T* tree)
{
	AST_T* child1 = AST_get_child(tree,0);
	if(tree->children->size == 1) return TC_get_type_E(cg, child1);

	var_type_T* t1 = TC_get_type_E(cg, child1);
	var_type_T* t2 = TC_get_type_E(cg, AST_get_child(tree, 2));
	
	if(TC_match_type(t1, 0, 3, "byte", "int", "long") && TC_match_type(t2, 0, 3, "byte", "int", "long")){
		free(t1);
		free(t2);
		return var_type_init(CG_find_type(cg, "bool"),0,0);
	}

	printf("%s:%ld:%ld: error: values of type %s and %s cannot be compared", tree->file, tree->line, tree->column, t1->type->name, t2->type->name);
	exit(1);
}

var_type_T* TC_get_type_E(CG_T* cg, AST_T* tree)
{
	if(tree->children->size == 1) return TC_get_type_T(cg, AST_get_child(tree,0));

	var_type_T* t1 = TC_get_type_T(cg, AST_get_child(tree,0));
	var_type_T* t2 = TC_get_type_E(cg, AST_get_child(tree,2));
	if(var_type_cmp(t1,t2)){
		free(t2);
		t1->is_lvalue = 0;
		return t1;
	}


	if(TC_match_type(t1,0, 3, "byte", "int", "long") && TC_match_type(t2,0, 3, "byte", "int", "long")){
		if(TC_match_type(t1, 0, 1, "long") || TC_match_type(t2, 0, 1, "long")){
			free(t1);
			free(t2);
			return var_type_init(CG_find_type(cg, "long"), 0, 0);
		}
		if(TC_match_type(t1, 0, 1, "int") || TC_match_type(t2, 0, 1, "int")){
			free(t1);
			free(t2);
			return var_type_init(CG_find_type(cg, "int"), 0, 0);
		}
		return var_type_init(CG_find_type(cg, "byte"),0,0);
	}
	else if(t1->ptr_depth > 0 && TC_match_type(t2,0, 3, "byte", "int", "long")){
		free(t2);
		return t1;
	}
	else if(t2->ptr_depth > 0 && TC_match_type(t1,0, 3, "byte", "int", "long")){
		free(t1);
		return t2;
	}

	printf("%s:%ld:%ld: error: operator \'%c\' cannot be applied to operands of type %s and %s\n",tree->file, tree->line, tree->column, AST_get_child(tree, 1)->type == AST_PLUS ? '+' : '-',  t1->type->name, t2->type->name);
	// TODO: free everything
	exit(1);

}
var_type_T* TC_get_type_T(CG_T* cg, AST_T* tree)
{
	if(tree->children->size == 1) return TC_get_type_U(cg, AST_get_child(tree,0));

	var_type_T* t1 = TC_get_type_U(cg, AST_get_child(tree,0));
	var_type_T* t2 = TC_get_type_T(cg, AST_get_child(tree,2));
	if(var_type_cmp(t1,t2)){
		free(t2);
		t1->is_lvalue = 0;
		return t1;
	}

	if(TC_match_type(t1,0, 3, "byte", "int", "long") && TC_match_type(t2,0, 3, "byte", "int", "long")){
		if(TC_match_type(t1, 0, 1, "long") || TC_match_type(t2, 0, 1, "long")){
			free(t1);
			free(t2);
			return var_type_init(CG_find_type(cg, "long"), 0, 0);
		}
		if(TC_match_type(t1, 0, 1, "int") || TC_match_type(t2, 0, 1, "int")){
			free(t1);
			free(t2);
			return var_type_init(CG_find_type(cg, "int"), 0, 0);
		}
		return var_type_init(CG_find_type(cg, "byte"),0,0);
	}

	printf("%s:%ld:%ld: error: operator \'%c\' cannot be applied to operands of type %s and %s\n",tree->file, tree->line, tree->column, AST_get_child(tree, 1)->type == AST_STAR ? '*' : '/', t1->type->name, t2->type->name);
	// TODO: free everything
	exit(1);
}

var_type_T* TC_get_type_U(CG_T* cg, AST_T* tree)
{
	if(tree->children->size == 1) return TC_get_type_POST(cg, AST_get_child(tree,0));

	var_type_T* t = TC_get_type_U(cg, AST_get_child(tree,1));
										   
	if(AST_get_child(tree, 0)->type == AST_MINUS){
		if(TC_match_type(t, 0, 3, "byte", "int", "long")){
			t->is_lvalue = 0;
			return t;
		}
	
		if(t->ptr_depth != 0)
			printf("%s:%ld:%ld: error: a pointer cannot be negated\n", tree->file, tree->line, tree->column);
		else
			printf("%s:%ld:%ld: error: operand of type %s cannot be negated\n", tree->file, tree->line, tree->column, t->type->name);
		// TODO: free everything
		exit(1);
	}
	if(AST_get_child(tree, 0)->type == AST_LEFT_ARR){
		t->is_lvalue = 1;
		if(t->ptr_depth == 0){
			printf("%s:%ld:%ld: error: a non-pointer cannot be dereferenced\n", tree->file, tree->line, tree->column);
			// TODO: free everything
			exit(1);
		}
		t->ptr_depth--;
		return t;
	}
	if(!t->is_lvalue){
		printf("%s:%ld:%ld: error: lvalue required as an operand of \'->\' operator\n", tree->file, tree->line, tree->column);
		// TODO: free everything
		exit(1);
	}
	t->is_lvalue = 0;
	t->ptr_depth++;
	return t;
}

var_type_T* TC_get_type_POST(CG_T* cg, AST_T* tree)
{
	var_type_T* t = TC_get_type_P(cg, AST_get_child(tree,0));
	for(int i = 1; i < tree->children->size; i++){
		switch(AST_get_child(tree, i)->type){
			case AST_DOT:
				{
					subtype_T* sub = type_find_subtype(t->type, AST_get_arg(AST_get_child(tree,i++)));
					free(t);
					t = var_type_copy(sub->type);
					t->is_lvalue = 1;
				}
			case AST_LEFT_B:
				{
					t->ptr_depth--;
					t->is_lvalue = 1;
					i += 2;
				}
		}
	}
	return t;
}

var_type_T* TC_get_type_P(CG_T* cg, AST_T* tree)
{
	AST_T* child = AST_get_child(tree, 0);
	if(child->type == AST_NUM) return TC_get_type_NUM(cg, child);
	if(child->type == AST_STR) return var_type_init(CG_find_type(cg, "string"), 0, 0);
	if(child->type == AST_TRUE || child->type == AST_FALSE) return var_type_init(CG_find_type(cg, "bool"), 0, 0);
	if(child->type == AST_LEFT_P) return TC_get_type_BE(cg, AST_get_child(tree, 1));
	if(child->type == AST_FUNCALL) return TC_get_type_FunCall(cg, AST_get_child(tree, 0));
	return TC_get_type_WORD(cg, child);
}

var_type_T* TC_get_type_FunCall(CG_T* cg, AST_T* tree)
{
	fun_frame_T* t = CG_find_function(cg, AST_get_arg(AST_get_child(tree, 0)));
	if(!t){
		printf("%s:%ld:%ld: error: unbound function \'%s\'\n", tree->file, tree->line, tree->column, AST_get_arg(AST_get_child(tree, 0)));
		// TODO: free everything
		exit(1);
	}
	return var_type_copy(t->return_type);
}

var_type_T* TC_get_type_WORD(CG_T* cg, AST_T* tree)
{
	symbol_T* t = symtab_look_up(cg->table, AST_get_arg(tree));
	if(!t){
		printf("%s:%ld:%ld: error: unbound variable \'%s\'\n", tree->file, tree->line, tree->column, AST_get_arg(tree));
		// TODO: free everything
		exit(1);
	}
	return var_type_copy(t->type);
}

var_type_T* TC_get_type_NUM(CG_T* cg, AST_T* tree)
{
	long num = strtol(AST_get_arg(tree), NULL, 10);

	if(num < 128) return var_type_init(CG_find_type(cg, "byte"),0, 0);
	if(num < 2147483648) return var_type_init(CG_find_type(cg, "int"),0, 0); 
	return var_type_init(CG_find_type(cg, "long"), 0, 0); 
}
