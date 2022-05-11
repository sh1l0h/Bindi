#ifndef __TOKEN_H_
#define __TOKEN_H_

#include <stdlib.h>

typedef union {
	int  _int;
	char* _str;
} arg_T;

typedef struct {
	enum {
		PLUS,
		MINUS,
		STAR,
		SLASH,
		CARET,
		BANG,
		EQ,
		EQ_EQ,
		LT,
		GT,
		GEQ,
		LEQ,
		NEQ,

		COLON,
		SEMICOLON,

		LEFT_B,
		RIGHT_B,
		LEFT_SB,
		RIGHT_SB,
		LEFT_P,
		RIGHT_P,

		RIGHT_ARR,

		WORD,
		STR,
		NUM,

		END,

		T_EOF
	} type;

	arg_T* arg;

	const char* file;
	size_t line;
	size_t column;
} token_T;

token_T* init_token(int type, arg_T* arg, char* file, size_t line, size_t column);

const char* token_type_to_str(int type);

char* token_to_str(token_T* token);

#endif
