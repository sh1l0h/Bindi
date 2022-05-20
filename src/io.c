#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/io.h"

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

void analyze_arguments(int argc, char** argv, char** input, char** output)
{
	int is_out_set = 0;
	int is_in_set = 0;
	for(int i = 1; i < argc; i++){
		char* curr = argv[i];
		if(curr[0] != '-'){
			is_in_set = 1;
			*input = curr;
		}
		else if(!strcmp(curr, "-o")){
			*output = argv[++i];
			is_out_set = 1;
		}
	}
	if(!is_out_set) *output = NULL;
	if(!is_in_set) *input = NULL;
}
