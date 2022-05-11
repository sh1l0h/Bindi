#include <stdlib.h>
#include <stdio.h>
#include "include/laxer.h"
#include "include/io.h"
#include "include/parser.h"
#include "include/cg.h"

int main(int argc, char** argv)
{

	const char* input = 0;
	for(int i = 1; i < argc; i++){
		const char* curr = argv[i];
		if(curr[0] != '-') input = curr;
	}

	char* src = read_src_from_file(input);
	laxer_T* laxer = init_laxer(input, src);
	laxer_get_tokens(laxer);

	for(int i = 0; i < laxer->tokens_size; i++){
		char* str = token_to_str(laxer->tokens+i);
		if((laxer->tokens+i)->type == WORD || (laxer->tokens+i)->type == STR)
			printf("%s,%s\n", str, (laxer->tokens+i)->arg->_str);
		else
			printf("%s\n", str);
		free(str);
	}
	parser_T* parser = parser_init(laxer->tokens);

	AST_T* tree = parser_parse_prog(parser);

	CG_T* cg = CG_init("t.asm");
	
	symtab_T* table = symtab_init();
	CG_generate_code(cg, tree,table);
	
	CG_free(cg);
	AST_free(tree);
	laxer_free(laxer);
	free(src);

	return 0;
}
