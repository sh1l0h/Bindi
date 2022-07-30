#include <stdlib.h>
#include "./include/ast.h"
#include "./include/token.h"

const int terminals[] = {
	AST_TYPE,

	AST_PLUS,
	AST_PLUS_PLUS,
	AST_MINUS,
	AST_MINUS_MINUS,
	AST_STAR,
	AST_SLASH,
	AST_CARET,
	AST_BANG,

	AST_MOD,

	AST_DOT,
	AST_COMMA,

	AST_EQ,
	AST_EQ_EQ,
	AST_PLUS_EQ,
	AST_MINUS_EQ,
	AST_LT,
	AST_GT,
	AST_GEQ,
	AST_LEQ,
	AST_NEQ,

	AST_COLON,
	AST_SEMICOLON,

	AST_LEFT_B,
	AST_RIGHT_B,
	AST_LEFT_SB,
	AST_RIGHT_SB,
	AST_LEFT_P,
	AST_RIGHT_P,

	AST_RIGHT_ARR,
	AST_LEFT_ARR,

	AST_IF,
	AST_ELSE,

	AST_WHILE,

	AST_AND,
	AST_OR,

	AST_LET,
		
	AST_TRUE,
	AST_FALSE,

	AST_PRINT,
	AST_RETURN,
	AST_VOID,
	AST_NONE,

	AST_BEGIN,
	AST_END,
		
	AST_WORD,
	AST_STR,
	AST_NUM
};

const int num_of_terminals = sizeof(terminals)/sizeof(int);

AST_T *AST_init(int type, const char* file, size_t line, size_t column)
{
	AST_T *result = (AST_T*) malloc(sizeof(AST_T));
	result->type = type;
	result->children = list_init(sizeof(AST_T*));
	result->file = file;
	result->line = line;
	result->column = column;
	return result;
}

AST_T *AST_get_child(AST_T* tree, int id)
{
	return ((AST_T**) tree->children->list)[id];
}

void AST_add_child(AST_T* tree, AST_T* child)
{
	list_add(tree->children, child);
}

int AST_has_child_with_type(AST_T* tree, int type)
{
	if(tree->type == type) return 1;
	if(AST_is_terminal(tree)) return 0;
	for(int i = 0; i < tree->children->size; i++){
		if(AST_has_child_with_type(AST_get_child(tree,i),type)) return 1;
	}
	return 0;
}

int AST_is_terminal(AST_T* tree)
{
	for(int i = 0; i < num_of_terminals; i++) if (terminals[i] == tree->type) return 1;
	return 0;
}

char* AST_get_arg(AST_T* word)
{
	return ((token_T*)*word->children->list)->arg;
}

void AST_free(AST_T* tree)
{
	if(!AST_is_terminal(tree))
		for(int i = 0; i < tree->children->size; i++) AST_free(AST_get_child(tree, i));
	free(tree->children->list);
	free(tree->children);
	free(tree);
}

size_t AST_count_nodes(AST_T* tree)
{
	if(AST_is_terminal(tree)) return 1;
	size_t sum = 0;
	for(int i = 0; i < tree->children->size; i++)
		sum += AST_count_nodes(AST_get_child(tree, i));
	return sum +1;
}
		
size_t AST_count_parameters(AST_T* tree)
{
	size_t result = 0;
	for(int i = 0; i < tree->children->size; i++){
		AST_T* curr = AST_get_child(tree, i);
		if(curr->type == AST_PAR) result++;
	}
	return result;
}

AST_T* AST_get_parameter(AST_T* tree, int id){
	int count = 0;
	for(int i = 0; i < tree->children->size; i++){
		if(count == id) return AST_get_child(tree, i);
		count++;
	}
	return NULL;
}

