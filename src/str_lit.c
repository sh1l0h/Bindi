#include <stdlib.h>
#include <string.h>
#include "./include/str_lit.h"

str_lit_T* str_lit_init(char* str, size_t id, size_t initial_size)
{
	str_lit_T* result = (str_lit_T*) malloc(sizeof(str_lit_T));
	result->str = str;
	result->id = id;
	result->initial_size = initial_size;

	return result;
}

char* str_to_lit(char* str)
{
	size_t str_len = strlen(str);
	size_t allocated = str_len + 64;
	char* result = (char*) malloc(allocated*sizeof(char));

	int last_was_norm = 0;
	size_t len = 0;
	for(int i = 0; i < str_len; i++){
		if(allocated - 16 < len) result = (char*)realloc(result, (allocated *= 2)*sizeof(char));
		char c = str[i];
		if(c == '\n') {
			if(last_was_norm){
				result[len++] = '\"';
				result[len++] = ',';
				result[len++] = ' ';
			}
			result[len++] = '1';
			result[len++] = '0';
			result[len++] = ',';
			result[len++] = ' ';
			last_was_norm = 0;
		}
		else {
			if(!last_was_norm)
				result[len++] = '\"';
			result[len++] = c;
			last_was_norm = 1;
		}
	}
	if(last_was_norm){
		result[len++] = '\"';
		result[len++] = '\0';
	}
	else{
		result[--len-1] = '\0';
	}
	result = (char*) realloc(result, len*sizeof(char));
	return result;
	
}
