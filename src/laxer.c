#include "include/laxer.h"
#include "string.h"
#include <stdlib.h>
#include <ctype.h>

laxer_T* init_laxer(const char* file, char* src)
{
	
	laxer_T* result = (laxer_T*) malloc(sizeof(laxer_T));
	result->file = file;
	result->src = src;
	result->curr = 0;
	result->size = strlen(src);
	result->tokens_size = 0;
	result->allocated_tokens_size = 8;
	result->line = 1;
	result->column = 1;

	return result;
}

char laxer_get_curr_char(laxer_T* laxer)
{
	if (laxer->curr < laxer->size) return laxer->src[laxer->curr];
	return '\0';
}

void laxer_add_token(laxer_T* laxer, int type, char* arg, const char* file, size_t line, size_t column)
{
	if(laxer->tokens_size == 0) laxer->tokens = (token_T*) calloc(laxer->allocated_tokens_size, sizeof(token_T));

	laxer->tokens[laxer->tokens_size].type = type;
	laxer->tokens[laxer->tokens_size].arg = arg;
	laxer->tokens[laxer->tokens_size].file = file;
	laxer->tokens[laxer->tokens_size].line = line;
	laxer->tokens[laxer->tokens_size].column = column;

	if(++laxer->tokens_size == laxer->allocated_tokens_size) laxer->tokens = (token_T*) realloc(laxer->tokens,(laxer->allocated_tokens_size *= 2 )*sizeof(token_T));
}

void laxer_tokenize_num(laxer_T* laxer)
{
	char c = laxer_get_curr_char(laxer);
	size_t allocated = 8;
	char* num = (char*) malloc(allocated*sizeof(char));
	
	size_t num_len = 0;
	while(isdigit(c) || c == '.'){
		num[num_len++] = c;
		if(num_len == allocated) num = (char*) realloc(num, (allocated *= 2)*sizeof(char)); 
		laxer->curr++;
		c = laxer_get_curr_char(laxer);
	}
	laxer->curr--;
	num = (char*) realloc(num, (num_len+1)*sizeof(char)); 
	num[num_len] = '\0';

	laxer_add_token(laxer, NUM, num, laxer->file, laxer->line, laxer->column);
	laxer->column += num_len;
}

void laxer_tokenize_word(laxer_T* laxer)
{
	char c = laxer_get_curr_char(laxer);
	size_t allocated = 8;
	char* word = (char*) malloc(allocated*sizeof(char));

	size_t str_len = 0;
	while (isalnum(c) || c == '_') {
		word[str_len++] = c;
		if(str_len == allocated) word = (char*) realloc(word, (allocated *= 2)*sizeof(char)); 
		laxer->curr++;
		c = laxer_get_curr_char(laxer);
	}
	laxer->curr--;
	word = (char*) realloc(word, (str_len+1)*sizeof(char)); 
	word[str_len] = '\0';

	laxer_add_token(laxer, WORD, word, laxer->file, laxer->line, laxer->column);
	laxer->column += str_len;
}

void laxer_tokenize_str(laxer_T* laxer)
{
	laxer->curr++;
	char c = 0; 

	size_t column = laxer->column;
	laxer->column++;

	size_t allocated = 8;
	char* str = (char*) malloc(allocated*sizeof(char));
	size_t str_len = 0;

	while((c = laxer_get_curr_char(laxer)) != '\"' && c != '\0' && c != '\n'){
		if(c == '\\'){
			laxer->curr++;
			char next = laxer_get_curr_char(laxer);
			switch (next){
				case 'n':
					str[str_len++] = '\n';
			}
			if(str_len >= allocated) str = realloc(str, (allocated *= 2)*sizeof(char));

			laxer->curr++;
			laxer->column += 2;
			continue;
		}

		str[str_len++] = c;
		if(str_len >= allocated) str = realloc(str, (allocated *= 2)*sizeof(char));
		laxer->curr++;
		laxer->column++;
	}
	laxer->column++;
	str = (char*) realloc(str, (str_len + 1)*sizeof(char));
	str[str_len] = '\0';

	laxer_add_token(laxer, STR, str, laxer->file, laxer->line, column);
}

void laxer_skip_line_comment(laxer_T* laxer)
{
	char c = 0;
	while((c =laxer_get_curr_char(laxer)) != '\n' &&  c != '\0')
		laxer->curr++;
	laxer->line++;
	laxer->column = 1;
}

void laxer_skip_mul_line_comment(laxer_T* laxer)
{
	char c = 0;
	while((c=laxer_get_curr_char(laxer)) != '\0' && (c != '|' || laxer_peek_char(laxer,1) != '>')) {
		if(c == '\n'){
			laxer->line++;
			laxer->column = 1;
		}
		else
			laxer->column++;
		laxer->curr++;
	}
	if(c = '|'){
		laxer->curr++;
		laxer->column += 2;
	}
}

void laxer_advance(laxer_T* laxer)
{
	char c = laxer_get_curr_char(laxer);

	if(isalpha(c)) laxer_tokenize_word(laxer);
	else if(isdigit(c)) laxer_tokenize_num(laxer);

	switch (c){
		case '+': 
			laxer_add_token(laxer, PLUS, NULL, laxer->file, laxer->line, laxer->column++);
			break;

		case '*': 
			laxer_add_token(laxer, STAR, NULL, laxer->file, laxer->line, laxer->column++);
			break;

		case '/': 
			laxer_add_token(laxer, SLASH, NULL, laxer->file, laxer->line, laxer->column++);
			break;

		case ':':
			laxer_add_token(laxer, COLON, NULL, laxer->file, laxer->line, laxer->column++);
			break;

		case ';':
			laxer_add_token(laxer, SEMICOLON, NULL, laxer->file, laxer->line, laxer->column++);
			break;
			
		case '{':
			laxer_add_token(laxer, LEFT_B, NULL, laxer->file, laxer->line, laxer->column++);
			break;

		case '}':
			laxer_add_token(laxer, RIGHT_B, NULL, laxer->file, laxer->line, laxer->column++);
			break;

		case '(':
			laxer_add_token(laxer, LEFT_P, NULL, laxer->file, laxer->line, laxer->column++);
			break;

		case ')':
			laxer_add_token(laxer, RIGHT_P, NULL, laxer->file, laxer->line, laxer->column++);
			break;

		case '[':
			laxer_add_token(laxer, LEFT_SB, NULL, laxer->file, laxer->line, laxer->column++);
			break;

		case ']':
			laxer_add_token(laxer, RIGHT_SB, NULL, laxer->file, laxer->line, laxer->column++);
			break;

		case ',':
			laxer_add_token(laxer, COMMA, NULL, laxer->file, laxer->line, laxer->column++);
			break;

		case '"':
			laxer_tokenize_str(laxer);
			break;

		case '|':
			laxer_skip_line_comment(laxer);
			break;

		case '<':
			if(laxer_match(laxer, '|')) laxer_skip_mul_line_comment(laxer);
			else if(laxer_match(laxer, '=')) laxer_add_token(laxer, LEQ, NULL, laxer->file, laxer->line, laxer->column-2);
			else if(laxer_match(laxer, '-')) laxer_add_token(laxer, LEFT_ARR, NULL, laxer->file, laxer->line, laxer->column-2);
			else laxer_add_token(laxer, LT, NULL, laxer->file, laxer->line, laxer->column++);
			break;

		case '>':
			if(laxer_match(laxer, '=')) laxer_add_token(laxer, GEQ, NULL, laxer->file, laxer->line, laxer->column-2);
			else laxer_add_token(laxer, GT, NULL, laxer->file, laxer->line, laxer->column++);
			break;

		case '!':
			if(laxer_match(laxer, '=')) laxer_add_token(laxer, NEQ, NULL, laxer->file, laxer->line, laxer->column-2);
			else laxer_add_token(laxer, BANG, NULL, laxer->file, laxer->line, laxer->column++);
			break;

		case '=':
			if(laxer_match(laxer, '=')) laxer_add_token(laxer, EQ_EQ, NULL, laxer->file, laxer->line, laxer->column-2);
			else laxer_add_token(laxer, EQ, NULL, laxer->file, laxer->line, laxer->column);
			break;
			
		case '-':
			if (laxer_match(laxer, '>')) laxer_add_token(laxer, RIGHT_ARR, NULL, laxer->file, laxer->line, laxer->column-2);
			else laxer_add_token(laxer, MINUS, NULL, laxer->file, laxer->line, laxer->column++);
			break;

		case '\n':
			laxer->line++;
			laxer->column = 1;
			break;

		case ' ':
		case '\t':
			laxer->column++;
			break;
	}
}

char laxer_peek_char(laxer_T* laxer, int offset)
{
	int index = laxer->curr + offset;
	if(index < laxer->size && index >= 0) return laxer->src[index];
	return '\0';
}

int laxer_match(laxer_T* laxer, char c)
{
	if(laxer_peek_char(laxer,1) == c){
		laxer->column += 2;
		laxer->curr++;
		return 1;
	}
	return 0;
}
token_T* laxer_get_tokens(laxer_T* laxer)
{
	while (laxer_get_curr_char(laxer) != '\0'){
		laxer_advance(laxer);
		laxer->curr++;
	}

	laxer_add_token(laxer, T_EOF, NULL, laxer->file, laxer->line, laxer->column);
}

void laxer_free(laxer_T* laxer)
{
	for(int i = 0; i < laxer->tokens_size; i++){
		if((laxer->tokens[i]).arg){
			free((laxer->tokens[i]).arg);
		}
	}
	free(laxer->tokens);
	free(laxer);
}
