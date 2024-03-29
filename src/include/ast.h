#ifndef __AST_H__
#define __AST_H__
#include <stdio.h>
#include "list.h"
#include "type.h"

typedef struct {
	enum {
		AST_PROG,

		AST_VADEC,
		AST_FUNDEC,

		AST_PARTYPE,
		AST_PAR,
		AST_PARS,

		AST_TYPE,
		AST_VATYPE,
		AST_LOCVADEC,

		AST_ST,
		AST_BLOCK,

		AST_BE,
		AST_BP,
		AST_BU,
		AST_BT,
		AST_E,
		AST_POST,
		AST_P,
		AST_U,
		AST_TERM,
		AST_FUNCALL,

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
	} type;

	list_T* children;
	const char* file;
	size_t line;
	size_t column;
} AST_T;

extern const int terminals[];

extern const int num_of_terminals;

AST_T *AST_init(int type, const char* file, size_t line, size_t column);

AST_T *AST_get_child(AST_T* tree, int id);

void AST_add_child(AST_T* tree, AST_T* child);

int AST_has_child_with_type(AST_T* tree, int type);

int AST_is_terminal(AST_T* tree);

char* AST_get_arg(AST_T* word);

void AST_free(AST_T* tree);

size_t AST_count_nodes(AST_T* tree);

size_t AST_count_parameters(AST_T* tree);

#endif
