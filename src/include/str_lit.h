#ifndef __STR_LIT_H__
#define __STR_LIT_H__
#include <stdio.h>

typedef struct {
	char* lable;
	char* str;
	size_t initial_size;
} str_lit_T;

str_lit_T* str_lit_init(char* lable, char* str, size_t initial_size);

#endif
