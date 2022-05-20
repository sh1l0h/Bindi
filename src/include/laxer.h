#ifndef __LAXER_H_
#define __LAXER_H_

#include "token.h"
#include <stdlib.h>

typedef struct {
	const char* file;
	char* src;
	size_t curr;
	size_t size;
	size_t line;
	size_t column;

	token_T* tokens;
	size_t tokens_size;
	size_t allocated_tokens_size;
} laxer_T;
	
laxer_T* init_laxer(const char* file, char* src);

char laxer_get_curr_char(laxer_T* laxer);

void laxer_add_token(laxer_T* laxer, int type, char* arg, const char* file, size_t line, size_t column);

void laxer_tokenize_num(laxer_T* laxer);

void laxer_tokenize_word(laxer_T* laxer);

void laxer_tokenize_str(laxer_T* laxer);

void laxer_skip_line_comment(laxer_T* laxer);

void laxer_skip_mul_line_comment(laxer_T* laxer);

void laxer_advance(laxer_T* laxer);

char laxer_peek_char(laxer_T* laxer, int offset);

int laxer_match(laxer_T* laxer, char c);

token_T* laxer_get_tokens(laxer_T* laxer);

void laxer_free(laxer_T* laxer);
	
#endif
