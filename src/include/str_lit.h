#ifndef __STR_LIT_H__
#define __STR_LIT_H__
#include <stdio.h>

typedef struct {
	char* str;
	size_t id;
	size_t initial_size;
} str_lit_T;

str_lit_T* str_lit_init(char* str, size_t id, size_t initial_size);

char* str_to_lit(char* str);

#endif
