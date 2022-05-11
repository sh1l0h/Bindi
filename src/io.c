#include "include/io.h"
#include <stdio.h>
#include <stdlib.h>

char* read_src_from_file(const char* path)
{
	FILE* f = fopen(path, "r");
	char* src = 0;
	size_t length = 0;

	if (f){
		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);
		src = (char*) malloc((length+1)*sizeof(char));
		src[length] = '\0';
		
		if(src){
			fread(src, 1, length, f);
		}
		fclose(f);
	}
	if(src) return src;
	return NULL;
}
