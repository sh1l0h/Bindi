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
	AST_T* result = AST_init(AST_PROG, parser->curr->file, parser->curr->line, parser->curr->column);

	while(parser_match(parser, 1, T_EOF) == -1){
		if(parser_match_keyword(parser, "let"))	AST_add_child(result, parser_parse_VaDec(parser));
		else AST_add_child(result, parser_parse_FunDec(parser));
	}

	return result;
}

AST_T* parser_parse_FunDec(parser_T* parser)
{
	AST_T* result = AST_init(AST_FUNDEC, parser->curr->file, parser->curr->line, parser->curr->column);
	AST_add_child(result, parser_parse_WORD(parser));
	parser_expect(parser, COLON);
	AST_add_child(result, AST_init(AST_COLON, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
	AST_add_child(result, parser_parse_pars(parser));
	parser_expect(parser, RIGHT_ARR);
	AST_add_child(result, AST_init(AST_RIGHT_ARR, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
	AST_add_child(result, parser_parse_VaType(parser));
	parser_expect(parser, LEFT_B);
	AST_add_child(result, AST_init(AST_LEFT_B, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
	while(parser_match(parser, 2, T_EOF, RIGHT_B) == -1) AST_add_child(result, parser_parse_St(parser));
	if((parser->curr -1)->type == T_EOF) return NULL; // add error
	AST_add_child(result, AST_init(AST_RIGHT_B, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
	return result;
}

AST_T* parser_parse_par(parser_T* parser)
{
	AST_T* result = AST_init(AST_PAR, parser->curr->file, parser->curr->line, parser->curr->column);
	AST_add_child(result, parser_parse_WORD(parser));
	parser_expect(parser, COLON);
	AST_add_child(result, AST_init(COLON, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
	AST_add_child(result, parser_parse_VaType(parser));
	return result;
}

AST_T* parser_parse_pars(parser_T* parser)
{
	AST_T* result = AST_init(AST_PARS, parser->curr->file, parser->curr->line, parser->curr->column);
	parser_expect(parser, LEFT_P);
	AST_add_child(result, AST_init(AST_LEFT_P, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
	if(parser_match(parser, 1, RIGHT_P) != -1){
		AST_add_child(result, AST_init(AST_RIGHT_P, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
	}
	else{
		AST_add_child(result, parser_parse_par(parser));
		while(parser_match(parser, 2, RIGHT_P, T_EOF) == -1){
			parser_expect(parser, COMMA);
			AST_add_child(result, AST_init(AST_COMMA, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
			AST_add_child(result, parser_parse_par(parser));
		}
		if((parser->curr -1)->type == T_EOF) return NULL; // add error
		AST_add_child(result, AST_init(AST_RIGHT_P, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
	}
	return result;
}

AST_T* parser_parse_VaDec(parser_T* parser)
{
	AST_T* result = AST_init(AST_VADEC, parser->curr->file, parser->curr->line, parser->curr->column);

	AST_add_child(result, AST_init(AST_LET, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
	AST_add_child(result, parser_parse_WORD(parser));
	parser_expect(parser, COLON);
	AST_add_child(result, AST_init(AST_COLON, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
	AST_add_child(result, parser_parse_VaType(parser));
	parser_expect(parser, SEMICOLON);
	AST_add_child(result, AST_init(AST_SEMICOLON, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));

	return result;
}

AST_T* parser_parse_VaType(parser_T* parser)
{
	AST_T* result = AST_init(AST_VATYPE, parser->curr->file, parser->curr->line, parser->curr->column);
	if(parser_match(parser, 1, RIGHT_ARR) != -1){
		AST_add_child(result, AST_init(AST_RIGHT_ARR, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
		AST_add_child(result, parser_parse_VaType(parser));
	}
	else AST_add_child(result, parser_parse_type(parser));
	return result;
}

AST_T* parser_parse_locVaDec(parser_T* parser)
{
	AST_T* result = AST_init(AST_LOCVADEC, parser->curr->file, parser->curr->line, parser->curr->column);
	AST_add_child(result, AST_init(AST_LET, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
	AST_add_child(result, parser_parse_WORD(parser));
	parser_expect(parser, COLON);
	AST_add_child(result, AST_init(AST_COLON, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
	AST_add_child(result, parser_parse_VaType(parser));
	if(parser_match(parser, 1, EQ) != -1){
		AST_add_child(result, AST_init(AST_EQ, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
		AST_add_child(result, parser_parse_BE(parser));
	}
	return result;
}

AST_T* parser_parse_type(parser_T* parser)
{
	AST_T* result = AST_init(AST_TYPE, parser->curr->file, parser->curr->line, parser->curr->column);
	list_free(result->children);
	result->children = list_init(sizeof(token_T*));
	list_add(result->children, parser->curr++);
	return result;
}

AST_T* parser_parse_St(parser_T* parser)
{
	AST_T* result = AST_init(AST_ST, parser->curr->file, parser->curr->line, parser->curr->column);
	if(parser_match_keyword(parser, "print")){
		AST_add_child(result, AST_init(AST_PRINT, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
		AST_add_child(result, parser_parse_BE(parser));
		parser_expect(parser, SEMICOLON);
		AST_add_child(result, AST_init(AST_SEMICOLON, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
	}
	else if(parser_match_keyword(parser, "if")){
		AST_add_child(result, AST_init(AST_IF, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
		AST_add_child(result, parser_parse_BE(parser));
		AST_add_child(result, parser_parse_St(parser));
		if(parser_match_keyword(parser, "else")){
			AST_add_child(result, AST_init(AST_ELSE, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
			AST_add_child(result, parser_parse_St(parser));
		}
	}
	else if(parser_match_keyword(parser, "while")){
		AST_add_child(result, AST_init(AST_WHILE, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
		AST_add_child(result, parser_parse_BE(parser));
		AST_add_child(result, parser_parse_St(parser));
	}
	else if(parser_match_keyword(parser, "begin")){
		AST_add_child(result, parser_parse_BLOCK(parser));
	}
	else if(parser_match_keyword(parser, "let")){
		AST_add_child(result, parser_parse_locVaDec(parser));
		parser_expect(parser, SEMICOLON);
		AST_add_child(result, AST_init(AST_SEMICOLON, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
	}
	else if(parser_match_keyword(parser, "return")){
		AST_add_child(result, AST_init(AST_RETURN, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
		AST_add_child(result, parser_parse_BE(parser));
		parser_expect(parser, SEMICOLON);
		AST_add_child(result, AST_init(AST_SEMICOLON, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
	}
	else{
		AST_add_child(result, parser_parse_BE(parser));
		int t = parser_match(parser, 3, EQ, PLUS_EQ, MINUS_EQ);
		switch(t){
			case EQ:
				AST_add_child(result, AST_init(AST_EQ, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
				break;
			case PLUS_EQ:
				AST_add_child(result, AST_init(AST_PLUS_EQ, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
				break;
			case MINUS_EQ:
				AST_add_child(result, AST_init(AST_MINUS_EQ, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
				break;
			case -1:
				exit(1);
				
		}
		AST_add_child(result, parser_parse_BE(parser));
		parser_expect(parser, SEMICOLON);
		AST_add_child(result, AST_init(AST_SEMICOLON, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
	}

	return result;
}

AST_T* parser_parse_BLOCK(parser_T* parser)
{
	AST_T* result = AST_init(AST_BLOCK, parser->curr->file, parser->curr->line, parser->curr->column);

	AST_add_child(result, AST_init(AST_BEGIN, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
	while(!parser_match_keyword(parser, "end") && parser->curr->type != T_EOF){
		AST_add_child(result, parser_parse_St(parser));
	}
	AST_add_child(result, AST_init(AST_END, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
	return result;
}

AST_T* parser_parse_BE(parser_T* parser)
{
	AST_T* result = AST_init(AST_BE, parser->curr->file, parser->curr->line, parser->curr->column);
	AST_add_child(result, parser_parse_BT(parser));

	char* c;
	if(parser_match_keyword(parser, "or")){
		AST_add_child(result, AST_init(AST_OR, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
		AST_add_child(result, parser_parse_BE(parser));
	}

	return result;
}

AST_T* parser_parse_BT(parser_T* parser)
{
	AST_T* result = AST_init(AST_BT, parser->curr->file, parser->curr->line, parser->curr->column);
	AST_add_child(result, parser_parse_BU(parser));

	if(parser_match_keyword(parser, "and")){
		AST_add_child(result, AST_init(AST_AND, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
		AST_add_child(result, parser_parse_BT(parser));
	}

	return result;
}
AST_T* parser_parse_BU(parser_T* parser)
{
	AST_T* result = AST_init(AST_BU, parser->curr->file, parser->curr->line, parser->curr->column);
	if(parser_match(parser, 1, BANG) != -1){
		AST_add_child(result, AST_init(AST_BANG, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
		AST_add_child(result, parser_parse_BU(parser));
	}
	else AST_add_child(result, parser_parse_BP(parser));

	return result;
}

AST_T* parser_parse_BP(parser_T* parser)
{
	AST_T* result = AST_init(AST_BP, parser->curr->file, parser->curr->line, parser->curr->column);

	AST_add_child(result, parser_parse_E(parser));
	int t = parser_match(parser, 6, GT, LT, GEQ, LEQ, EQ_EQ, NEQ);
	if(t != -1){
		switch(t){
			case GT:
				AST_add_child(result, AST_init(AST_GT, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
				break;

			case LT:
				AST_add_child(result, AST_init(AST_LT, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
				break;

			case GEQ:
				AST_add_child(result, AST_init(AST_GEQ, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
				break;

			case LEQ:
				AST_add_child(result, AST_init(AST_LEQ, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
				break;

			case EQ_EQ:
				AST_add_child(result, AST_init(AST_EQ_EQ, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
				break;

			case NEQ:
				AST_add_child(result, AST_init(AST_NEQ, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
				break;
		}
		AST_add_child(result, parser_parse_E(parser));
	}
	return result;
}

AST_T* parser_parse_E(parser_T* parser)
{
	AST_T* result = AST_init(AST_E, parser->curr->file, parser->curr->line, parser->curr->column);
	AST_add_child(result, parser_parse_T(parser));

	int t = parser_match(parser, 2, PLUS, MINUS);
	if(t != -1){
		if(t == PLUS) AST_add_child(result, AST_init(AST_PLUS, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
		else AST_add_child(result, AST_init(AST_MINUS, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
		AST_add_child(result, parser_parse_E(parser));
	}
	return result;
}

AST_T* parser_parse_T(parser_T* parser)
{
	AST_T* result = AST_init(AST_TERM, parser->curr->file, parser->curr->line, parser->curr->column);
	AST_add_child(result, parser_parse_U(parser));

	int t = parser_match(parser, 3, STAR, SLASH);
	if(t != -1){
		if(t == STAR) AST_add_child(result, AST_init(AST_STAR, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
		else AST_add_child(result, AST_init(AST_SLASH, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
		AST_add_child(result, parser_parse_T(parser));
	}
	else if(parser_match_keyword(parser, "mod")){
		AST_add_child(result, AST_init(AST_MOD, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
		AST_add_child(result, parser_parse_T(parser));
	}

	return result;
}

AST_T* parser_parse_U(parser_T* parser)
{
	AST_T* result = AST_init(AST_U, parser->curr->file, parser->curr->line, parser->curr->column);
	
	int t = parser_match(parser, 3, RIGHT_ARR, LEFT_ARR, MINUS);
	if(t != -1){
		if(t == RIGHT_ARR) AST_add_child(result, AST_init(AST_RIGHT_ARR, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
		else if(t == MINUS)	AST_add_child(result, AST_init(AST_MINUS, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
		else AST_add_child(result, AST_init(AST_LEFT_ARR, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));

		AST_add_child(result, parser_parse_U(parser));
	}
	else AST_add_child(result, parser_parse_POST(parser));

	return result;
}

AST_T* parser_parse_POST(parser_T* parser)
{
	AST_T* result = AST_init(AST_POST, parser->curr->file, parser->curr->line, parser->curr->column);
	AST_add_child(result, parser_parse_P(parser));
	int t = parser_match(parser, 5, LEFT_B, DOT, PLUS_PLUS, MINUS_MINUS, T_EOF);
	while((t =parser_match(parser, 5, LEFT_B, DOT, PLUS_PLUS, MINUS_MINUS, T_EOF)) != -1 && t != T_EOF){
		if(t == LEFT_B){
			AST_add_child(result, AST_init(AST_LEFT_B, (parser->curr -1)->file, (parser->curr -1)->line, (parser->curr -1)->column));
			AST_add_child(result, parser_parse_BE(parser));
			parser_expect(parser, RIGHT_B);
			AST_add_child(result, AST_init(AST_RIGHT_B, (parser->curr -1)->file, (parser->curr -1)->line, (parser->curr -1)->column));
		}
		else if(t == DOT){
			AST_add_child(result, AST_init(AST_DOT, (parser->curr -1)->file, (parser->curr -1)->line, (parser->curr -1)->column));
			AST_add_child(result, parser_parse_WORD(parser));
		}
		else if(t == PLUS_PLUS) AST_add_child(result, AST_init(AST_PLUS_PLUS, (parser->curr -1)->file, (parser->curr -1)->line, (parser->curr -1)->column));
		else AST_add_child(result, AST_init(AST_MINUS_MINUS, (parser->curr -1)->file, (parser->curr -1)->line, (parser->curr -1)->column));
	}
	if(t == T_EOF); // TODO: add error
	return result;
}

AST_T* parser_parse_P(parser_T* parser)
{
	AST_T* result = AST_init(AST_P, parser->curr->file, parser->curr->line, parser->curr->column);
	if(parser->curr->type == NUM){
		AST_add_child(result, parser_parse_NUM(parser));
	}
	else if(parser_match_keyword(parser, "true")) AST_add_child(result, AST_init(AST_TRUE, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
	else if(parser_match_keyword(parser, "false")) AST_add_child(result, AST_init(AST_FALSE, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
	else if(parser_match_keyword(parser, "none")) AST_add_child(result, AST_init(AST_NONE, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
	else if(parser->curr->type == WORD){
		if((parser->curr+1)->type == LEFT_P) AST_add_child(result, parser_parse_FunCall(parser));
		else AST_add_child(result, parser_parse_WORD(parser));
	}
	else if(parser->curr->type == STR){
		AST_T* child = AST_init(AST_STR, parser->curr->file, parser->curr->line, parser->curr->column);
		list_free(child->children);
		child->children = list_init(sizeof(token_T*));
		list_add(child->children, parser->curr++);
		AST_add_child(result, child);
	}
	else if(parser_match(parser, 1, LEFT_P)){
		AST_add_child(result, AST_init(AST_LEFT_P, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
		AST_add_child(result, parser_parse_BE(parser));
		parser_expect(parser, RIGHT_P);
		AST_add_child(result, AST_init(AST_RIGHT_P, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
	}
	return result;
}

AST_T* parser_parse_FunCall(parser_T* parser)
{
	AST_T* result = AST_init(AST_FUNCALL, parser->curr->file, parser->curr->line, parser->curr->column);
	AST_add_child(result, parser_parse_WORD(parser));
	AST_add_child(result, AST_init(AST_LEFT_P, parser->curr->file, parser->curr->line, (parser->curr++)->column));
	if(parser_match(parser, 1, RIGHT_P) == -1){
		AST_add_child(result, parser_parse_BE(parser));
		while(parser_match(parser, 2, RIGHT_P, T_EOF) == -1){
			parser_expect(parser, COMMA);
			AST_add_child(result, AST_init(AST_COMMA, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
			AST_add_child(result, parser_parse_BE(parser));
		}
		if((parser->curr -1)->type == T_EOF) return NULL; // add error
	}
	AST_add_child(result, AST_init(AST_RIGHT_P, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column));
	return result;
}


AST_T* parser_parse_WORD(parser_T* parser)
{
	parser_expect(parser, WORD);
	AST_T* result = AST_init(AST_WORD, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column);
	list_free(result->children);
	result->children = list_init(sizeof(token_T*));
	list_add(result->children, (parser->curr-1));
	return result;
}

AST_T* parser_parse_NUM(parser_T* parser)
{
	parser_expect(parser, NUM);
	AST_T* result = AST_init(AST_NUM, (parser->curr-1)->file, (parser->curr-1)->line, (parser->curr-1)->column);
	list_free(result->children);
	result->children = list_init(sizeof(token_T*));
	list_add(result->children, (parser->curr-1));
	return result;
}
