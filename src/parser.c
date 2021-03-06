#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "./include/parser.h"

parser_T *parser_init(token_T* tokens)
{
	parser_T* result = (parser_T*) malloc(sizeof(parser_T));
	result->tokens = tokens;
	result->curr = tokens;

	return result;
}

int parser_match(parser_T* parser, int argc, ...)
{
	va_list l;
	va_start(l, argc);
	for(int i = 0; i < argc; i++){
		int type = va_arg(l, int);
		if(type == parser->curr->type){
			va_end(l);
			parser->curr++;
			return type;
		}
	}
	va_end(l);
	return -1;
}

int parser_match_keyword(parser_T* parser, const char* keyword)
{
	if(parser->curr->type == WORD && !strcmp(parser->curr->arg, keyword)){
		parser->curr++;
		return 1;
	}
	return 0;
}

void parser_expect(parser_T* parser, int type)
{
	if(type == parser->curr->type){
		parser->curr++;
		return;
	}
	//TODO: free everything  
	printf("%s:%ld:%ld: error: Expected type %s, but found %s\n", parser->curr->file, parser->curr->line, parser->curr->column, token_type_to_str(type), token_type_to_str(parser->curr->type));
	exit(1);
}

AST_T* parser_parse_prog(parser_T* parser)
{
	AST_T* result = AST_init(AST_PROG);

	while(parser_match(parser, 1, T_EOF) == -1){
		if(parser_match_keyword(parser, "let"))	AST_add_child(result, parser_parse_VaDec(parser));
		else AST_add_child(result, parser_parse_FunDec(parser));
	}

	return result;
}

AST_T* parser_parse_FunDec(parser_T* parser)
{
	AST_T* result = AST_init(AST_FUNDEC);
	AST_add_child(result, parser_parse_WORD(parser));
	parser_expect(parser, COLON);
	AST_add_child(result, AST_init(AST_COLON));
	if(parser_match_keyword(parser, "void")){
		AST_add_child(result, AST_init(AST_VOID));
	}
	else if(parser->curr->type == WORD){
		AST_add_child(result, parser_parse_WORD(parser));
		parser_expect(parser, COLON);
		AST_add_child(result, AST_init(COLON));
		AST_add_child(result, parser_parse_TE(parser));
	}
	else{
		parser_expect(parser, LEFT_P);
		AST_add_child(result, AST_init(AST_LEFT_P));
		AST_add_child(result, parser_parse_WORD(parser));
		parser_expect(parser, COLON);
		AST_add_child(result, AST_init(COLON));
		AST_add_child(result, parser_parse_TE(parser));
		while(parser_match(parser, 2, RIGHT_P, T_EOF) == -1){
			parser_expect(parser, COMMA);
			AST_add_child(result, AST_init(AST_COMMA));
			AST_add_child(result, parser_parse_WORD(parser));
			parser_expect(parser, COLON);
			AST_add_child(result, AST_init(AST_COLON));
			AST_add_child(result, parser_parse_TE(parser));
		}
		if((parser->curr -1)->type == T_EOF) return NULL; // add error
		AST_add_child(result, AST_init(AST_RIGHT_P));
	}
	parser_expect(parser, RIGHT_ARR);
	AST_add_child(result, AST_init(AST_RIGHT_ARR));
	AST_add_child(result, parser_parse_TE(parser));
	parser_expect(parser, LEFT_B);
	AST_add_child(result, AST_init(AST_LEFT_B));
	while(parser_match(parser, 2, T_EOF, RIGHT_B) == -1) AST_add_child(result, parser_parse_St(parser));
	if((parser->curr -1)->type == T_EOF) return NULL; // add error
	AST_add_child(result, AST_init(AST_RIGHT_B));
	return result;
																	 
}

AST_T* parser_parse_TE(parser_T* parser)
{
	AST_T* result = AST_init(AST_TE);
	if(parser_match(parser,1, RIGHT_ARR) != -1){
		AST_add_child(result, AST_init(AST_RIGHT_ARR));
		AST_add_child(result, parser_parse_type(parser));
		return result;
	}
	if(parser_match_keyword(parser, "void")){
		AST_add_child(result, AST_init(AST_VOID));
		return result;
	}
	AST_add_child(result, parser_parse_type(parser));
	if(parser_match(parser,1,LEFT_SB) != -1){
		AST_add_child(result, AST_init(AST_LEFT_SB));
		parser_expect(parser, RIGHT_SB);
		AST_add_child(result, AST_init(AST_RIGHT_SB));
	}
	return result;
}

AST_T* parser_parse_VaDec(parser_T* parser)
{
	AST_T* result = AST_init(AST_VADEC);

	AST_add_child(result, AST_init(AST_LET));
	AST_add_child(result, parser_parse_WORD(parser));
	parser_expect(parser, COLON);
	AST_add_child(result, AST_init(AST_COLON));

	if(parser_match(parser, 1, RIGHT_ARR) != -1){
		AST_add_child(result, AST_init(AST_RIGHT_ARR));
		AST_add_child(result, parser_parse_type(parser));
	}
	else {
		AST_add_child(result, parser_parse_type(parser));
		if(parser_match(parser, 1, LEFT_B) != -1){
			AST_add_child(result, AST_init(AST_LEFT_B));
			AST_add_child(result, parser_parse_NUM(parser));
			parser_expect(parser, RIGHT_B); 
			AST_add_child(result, AST_init(AST_RIGHT_B));
		}
	}
	parser_expect(parser, SEMICOLON);
	AST_add_child(result, AST_init(AST_SEMICOLON));

	return result;
}

AST_T* parser_parse_type(parser_T* parser)
{
	AST_T* result = AST_init(AST_TYPE);
	list_free(result->children);
	result->children = list_init(sizeof(token_T*));
	list_add(result->children, parser->curr++);
	return result;
}

AST_T* parser_parse_St(parser_T* parser)
{

	AST_T* result = AST_init(AST_ST);

	if(parser_match_keyword(parser, "print")){
		AST_add_child(result, AST_init(AST_PRINT));
		AST_add_child(result, parser_parse_BE(parser));
		parser_expect(parser, SEMICOLON);
		AST_add_child(result, AST_init(AST_SEMICOLON));
	}
	else if(parser_match_keyword(parser, "if")){
		AST_add_child(result, AST_init(AST_IF));
		AST_add_child(result, parser_parse_BE(parser));
		AST_add_child(result, parser_parse_St(parser));
		if(parser_match_keyword(parser, "else")){
			AST_add_child(result, AST_init(AST_ELSE));
			AST_add_child(result, parser_parse_St(parser));
		}
	}
	else if(parser_match_keyword(parser, "while")){
		AST_add_child(result, AST_init(AST_WHILE));
		AST_add_child(result, parser_parse_BE(parser));
		AST_add_child(result, parser_parse_St(parser));
	}
	else if(parser_match_keyword(parser, "begin")){
		AST_add_child(result, parser_parser_BLOCK(parser));
	}
	else{
		AST_add_child(result, parser_parse_id(parser));
		parser_expect(parser, EQ);
		AST_add_child(result, AST_init(AST_EQ));
		AST_add_child(result, parser_parse_BE(parser));
		parser_expect(parser, SEMICOLON);
		AST_add_child(result, AST_init(AST_SEMICOLON));
	}

	return result;
}

AST_T* parser_parser_BLOCK(parser_T* parser)
{
	AST_T* result = AST_init(AST_BLOCK);

	AST_add_child(result, AST_init(AST_BEGIN));
	while(!parser_match_keyword(parser, "end") && parser->curr->type != T_EOF){
		AST_add_child(result, parser_parse_St(parser));
	}
	AST_add_child(result, AST_init(AST_END));
	return result;
}

AST_T* parser_parse_BE(parser_T* parser)
{
	AST_T* result = AST_init(AST_BE);
	AST_add_child(result, parser_parse_BT(parser));

	char* c;
	if(parser_match_keyword(parser, "or")){
		AST_add_child(result, AST_init(AST_OR));
		AST_add_child(result, parser_parse_BE(parser));
	}

	return result;
}

AST_T* parser_parse_BT(parser_T* parser)
{
	AST_T* result = AST_init(AST_BT);
	AST_add_child(result, parser_parse_BU(parser));

	if(parser_match_keyword(parser, "and")){
		AST_add_child(result, AST_init(AST_AND));
		AST_add_child(result, parser_parse_BT(parser));
	}

	return result;
}
AST_T* parser_parse_BU(parser_T* parser)
{
	AST_T* result = AST_init(AST_BU);
	if(parser_match(parser, 1, BANG) != -1){
		AST_add_child(result, AST_init(AST_BANG));
		AST_add_child(result, parser_parse_BU(parser));
	}
	else AST_add_child(result, parser_parse_BP(parser));

	return result;
}

AST_T* parser_parse_BP(parser_T* parser)
{
	AST_T* result = AST_init(AST_BP);

	AST_add_child(result, parser_parse_E(parser));
	int t = parser_match(parser, 6, GT, LT, GEQ, LEQ, EQ_EQ, NEQ);
	if(t != -1){
		switch(t){
			case GT:
				AST_add_child(result, AST_init(AST_GT));
				break;

			case LT:
				AST_add_child(result, AST_init(AST_LT));
				break;

			case GEQ:
				AST_add_child(result, AST_init(AST_GEQ));
				break;

			case LEQ:
				AST_add_child(result, AST_init(AST_LEQ));
				break;

			case EQ_EQ:
				AST_add_child(result, AST_init(AST_EQ_EQ));
				break;

			case NEQ:
				AST_add_child(result, AST_init(AST_NEQ));
				break;
		}
		AST_add_child(result, parser_parse_E(parser));
	}
	return result;
}

AST_T* parser_parse_E(parser_T* parser)
{
	AST_T* result = AST_init(AST_E);
	AST_add_child(result, parser_parse_T(parser));

	int t = parser_match(parser, 2, PLUS, MINUS);
	if(t != -1){
		if(t == PLUS) AST_add_child(result, AST_init(AST_PLUS));
		else AST_add_child(result, AST_init(AST_MINUS));
		AST_add_child(result, parser_parse_E(parser));
	}
	return result;
}

AST_T* parser_parse_T(parser_T* parser)
{
	AST_T* result = AST_init(AST_TERM);
	AST_add_child(result, parser_parse_U(parser));

	int t = parser_match(parser, 2, STAR, SLASH);
	if(t != -1){
		if(t == STAR) AST_add_child(result, AST_init(AST_STAR));
		else AST_add_child(result, AST_init(AST_SLASH));
		AST_add_child(result, parser_parse_T(parser));
	}

	return result;
}

AST_T* parser_parse_U(parser_T* parser)
{
	AST_T* result = AST_init(AST_U);
	
	int t = parser_match(parser, 3, RIGHT_ARR, LEFT_ARR, MINUS);
	if(t != -1){
		if(t == RIGHT_ARR) AST_add_child(result, AST_init(AST_RIGHT_ARR));
		else if(t == MINUS)	AST_add_child(result, AST_init(AST_MINUS));
		else AST_add_child(result, AST_init(AST_LEFT_ARR));

		AST_add_child(result, parser_parse_U(parser));
	}
	else AST_add_child(result, parser_parse_P(parser));

	return result;
}

AST_T* parser_parse_P(parser_T* parser)
{
	AST_T* result = AST_init(AST_P);
	if(parser->curr->type == NUM){
		AST_add_child(result, parser_parse_NUM(parser));
	}
	else if(parser_match_keyword(parser, "true")) AST_add_child(result, AST_init(AST_TRUE));
	else if(parser_match_keyword(parser, "false")) AST_add_child(result, AST_init(AST_FALSE));
	else if(parser_match_keyword(parser, "none")) AST_add_child(result, AST_init(AST_NONE));
	else if(parser->curr->type == WORD)	AST_add_child(result, parser_parse_id(parser));
	else if(parser->curr->type == STR){
		AST_T* child = AST_init(AST_STR);
		list_free(child->children);
		child->children = list_init(sizeof(token_T*));
		list_add(child->children, parser->curr++);
		AST_add_child(result, child);
	}
	else if(parser_match(parser, 1, LEFT_P)){
		AST_add_child(result, AST_init(AST_LEFT_P));
		AST_add_child(result, parser_parse_BE(parser));
		parser_expect(parser, RIGHT_P);
		AST_add_child(result, AST_init(AST_RIGHT_P));
	}
	return result;
}

AST_T* parser_parse_id(parser_T* parser)
{
	AST_T* result = AST_init(AST_ID);

	AST_add_child(result, parser_parse_WORD(parser));
	int t = parser_match(parser, 2, DOT, LEFT_SB);
	if(t == -1) return result;

	if(t == DOT) {
		AST_add_child(result, AST_init(AST_DOT));
		AST_add_child(result, parser_parse_id(parser));
	}
	else{
		AST_add_child(result, AST_init(AST_LEFT_SB));
		AST_add_child(result, parser_parse_BE(parser));
		parser_expect(parser, RIGHT_SB);
		AST_add_child(result, AST_init(AST_RIGHT_SB));
		if(parser_match(parser, 1, DOT)){
			AST_add_child(result, AST_init(AST_DOT));
			AST_add_child(result, parser_parse_id(parser));
		}
	}

	return result;
}

AST_T* parser_parse_WORD(parser_T* parser)
{
	if(parser->curr->type != WORD) return NULL; // add error
	AST_T* result = AST_init(AST_WORD);
	list_free(result->children);
	result->children = list_init(sizeof(token_T*));
	list_add(result->children, parser->curr++);
	return result;
}

AST_T* parser_parse_NUM(parser_T* parser)
{
	if(parser->curr->type != NUM) return NULL; // add error
	AST_T* result = AST_init(AST_NUM);
	list_free(result->children);
	result->children = list_init(sizeof(token_T*));
	list_add(result->children, parser->curr++);
	return result;
}
