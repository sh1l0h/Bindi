#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "./include/tc.h"
#include "./include/symtable.h"

int TC_match_type(type_T* type, int argc, ...)
{
	va_list l;
	va_start(l, argc);
	for(int i = 0; i < argc; i++){
		char* name = va_arg(l, char*);
		if(!strcmp(name, type->name)){
			va_end(l);
			return 1;
		}
	}
	return 0;
}

type_T* TC_get_type_BE(CG_T* cg, AST_T* tree)
{
	if(tree->children->size == 1) return TC_get_type_BT(cg, AST_get_child(tree, 0));

	type_T* t1 = TC_get_type_BT(cg, AST_get_child(tree,0));
	type_T* t2 = TC_get_type_BE(cg, AST_get_child(tree,2));
	if(t1 != t2 || strcmp(t1->name, "bool")){
		printf("error: operator \'or\' cannot be applied to operands of type %s and %s\n", t1->name, t2->name);
		exit(1);
	}
	return t1;
}

type_T* TC_get_type_BT(CG_T* cg, AST_T* tree)
{
	if(tree->children->size == 1) return TC_get_type_BU(cg, AST_get_child(tree, 0));

	type_T* t1 = TC_get_type_BU(cg, AST_get_child(tree,0));
	type_T* t2 = TC_get_type_BT(cg, AST_get_child(tree,2));
	if(t1 != t2 || strcmp(t1->name, "bool")){
		printf("error: operator \'and\' cannot be applied to operands of type %s and %s\n", t1->name, t2->name);
		exit(1);
	}
	return t1;
}

type_T* TC_get_type_BU(CG_T* cg, AST_T* tree)
{
	if(tree->children->size == 1) return TC_get_type_BP(cg, AST_get_child(tree, 0));

	type_T* t = TC_get_type_BU(cg, AST_get_child(tree,1));
	if(strcmp(t->name, "bool")) {
		printf("error: operator \'!\' cannot be applied to operand of type %s\n", t->name);
		exit(1);
	}
	return t;
}

type_T* TC_get_type_BP(CG_T* cg, AST_T* tree)
{
	AST_T* child1 = AST_get_child(tree,0);
	if(tree->children->size == 1) return TC_get_type_E(cg, child1);

	type_T* t1 = TC_get_type_E(cg, child1);
	type_T* t2 = TC_get_type_E(cg, AST_get_child(tree, 2));
	
	if(t1 == t2 || TC_match_type(t1, 3, "byte", "int", "long") && TC_match_type(t2, 3, "byte", "int", "long")) return CG_find_type(cg, "bool");

	printf("error: values of type %s and %s cannot be compared", t1->name, t2->name);
	exit(1);
}

type_T* TC_get_type_E(CG_T* cg, AST_T* tree)
{
	if(tree->children->size == 1) return TC_get_type_T(cg, AST_get_child(tree,0));

	type_T* t1 = TC_get_type_T(cg, AST_get_child(tree,0));
	type_T* t2 = TC_get_type_E(cg, AST_get_child(tree,2));
	if(t1 == t2) return t1;

	if(TC_match_type(t1, 3, "byte", "int", "long") && TC_match_type(t2, 3, "byte", "int", "long")){
		if(TC_match_type(t1, 1, "long") || TC_match_type(t2, 1, "long")) return CG_find_type(cg, "long");
		if(TC_match_type(t1, 1, "int") || TC_match_type(t2, 1, "int")) return CG_find_type(cg, "int");
		return CG_find_type(cg, "byte");
	}

	printf("error: operator \'%c\' cannot be applied to operands of type %s and %s\n", AST_get_child(tree, 1)->type == AST_PLUS ? '+' : '-', t1->name, t2->name);
	// TODO: free everything
	exit(1);

}
type_T* TC_get_type_T(CG_T* cg, AST_T* tree)
{
	if(tree->children->size == 1) return TC_get_type_U(cg, AST_get_child(tree,0));

	type_T* t1 = TC_get_type_U(cg, AST_get_child(tree,0));
	type_T* t2 = TC_get_type_T(cg, AST_get_child(tree,2));
	if(t1 == t2) return t1;

	if(TC_match_type(t1, 3, "byte", "int", "long") && TC_match_type(t2, 3, "byte", "int", "long")){
		if(TC_match_type(t1, 1, "long") || TC_match_type(t2, 1, "long")) return CG_find_type(cg, "long");
		if(TC_match_type(t1, 1, "int") || TC_match_type(t2, 1, "int")) return CG_find_type(cg, "int");
		return CG_find_type(cg, "byte");
	}

	printf("error: operator \'%c\' cannot be applied to operands of type %s and %s\n", AST_get_child(tree, 1)->type == AST_STAR ? '*' : '/', t1->name, t2->name);
	// TODO: free everything
	exit(1);
}
type_T* TC_get_type_U(CG_T* cg, AST_T* tree)
{
	if(tree->children->size == 1) return TC_get_type_P(cg, AST_get_child(tree,0));

	type_T* t = TC_get_type_U(cg, AST_get_child(tree,1));
	
	if(TC_match_type(t, 3, "byte", "int", "long")) return t;
	
	printf("error: operand of type %s cannot be negated\n", t->name);
	// TODO: free everything
	exit(1);
	
}
type_T* TC_get_type_P(CG_T* cg, AST_T* tree)
{
	AST_T* child = AST_get_child(tree, 0);
	if(child->type == AST_NUM) return TC_get_type_NUM(cg, child);
	if(child->type == AST_STR) return CG_find_type(cg, "string");
	if(child->type == AST_TRUE || child->type == AST_FALSE) return CG_find_type(cg, "bool");
	if(child->type == AST_LEFT_P) return TC_get_type_BE(cg, AST_get_child(tree, 1));
	return TC_get_type_id(cg, child);
}

type_T* TC_get_type_id(CG_T* cg, AST_T* tree)
{
	if(tree->children->size == 1) return symtab_look_up(cg->table, AST_get_arg(AST_get_child(tree,0)))->type;
}
type_T* TC_get_type_NUM(CG_T* cg, AST_T* tree)
{
	long num = strtol(AST_get_arg(tree), NULL, 10);

	if(num < 128) return CG_find_type(cg, "byte");
	if(num < 2147483648) return CG_find_type(cg, "int"); 
	return CG_find_type(cg, "long"); 
}
