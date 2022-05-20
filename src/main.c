#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "include/laxer.h"
#include "include/io.h"
#include "include/parser.h"
#include "include/code_generator.h"

int main(int argc, char** argv)
{

	char* input = 0;
	char* output = 0;

	analyze_arguments(argc, argv, &input, &output);

	if(!input){
		printf("error: input file is missing\n");
		return 1;
	}
	if(!output){
		output = "a.out";
	}

	char* src = read_src_from_file(input);
	laxer_T* laxer = init_laxer(input, src);
	laxer_get_tokens(laxer);

	for(int i = 0; i < laxer->tokens_size; i++){
		char* str = token_to_str(laxer->tokens+i);
		if((laxer->tokens+i)->type == WORD || (laxer->tokens+i)->type == STR || (laxer->tokens+i)->type == NUM)
			printf("%s,%s\n", str, (laxer->tokens+i)->arg);
		else
			printf("%s\n", str);
		free(str);
	}

	parser_T* parser = parser_init(laxer->tokens);

	AST_T* tree = parser_parse_prog(parser);

	CG_T* cg = CG_init(".temp.asm");

	CG_code(cg, tree);

	CG_close(cg);

	system("nasm -f elf64 .temp.asm");
	//	system("rm .temp.asm");

	char cmd2[strlen(output) + 16];
	sprintf(cmd2, "ld .temp.o -o %s", output);
	system(cmd2);
	system("rm .temp.o");
	
	AST_free(tree);
	laxer_free(laxer);
	free(src);

	return 0;
}
