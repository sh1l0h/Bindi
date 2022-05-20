#include "./include/str_lit.h"

str_lit_T* str_lit_init(char* lable, char* str, size_t initial_size)
{
	str_lit_T* result = (str_lit_T*) malloc(sizeof(str_lit_T));
	result->lable = lable;
	result->str = str;
	result->initial_size = initial_size;

	return result;
}
