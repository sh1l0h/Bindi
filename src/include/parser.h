#ifndef __PARSER_H_
#define __PARSER_H_
#include "token.h"
#include "ast.h"

typedef struct {
	token_T* tokens;
	token_T* curr;

	AST_T* tree;
}  parser_T;


parser_T *parser_init(token_T* tokens);
	
int parser_match(parser_T* parser, int argc, ...);
int parser_match_keyword(parser_T* parser, const char* keyword);
void parser_expect(parser_T* parser, int type);

AST_T* parser_parse_prog(parser_T* parser);
AST_T* parser_parse_VaDec(parser_T* parser);
AST_T* parser_parse_VaType(parser_T* parser);
AST_T* parser_parse_locVaDec(parser_T* parser);
AST_T* parser_parse_FunDec(parser_T* parser);
AST_T* parser_parse_par(parser_T* parser);
AST_T* parser_parse_pars(parser_T* parser);

AST_T* parser_parse_type(parser_T* parser);
AST_T* parser_parse_St(parser_T* parser);
AST_T* parser_parse_BLOCK(parser_T* parser);

AST_T* parser_parse_BE(parser_T* parser);
AST_T* parser_parse_BT(parser_T* parser);
AST_T* parser_parse_BU(parser_T* parser);
AST_T* parser_parse_BP(parser_T* parser);
AST_T* parser_parse_E(parser_T* parser);
AST_T* parser_parse_T(parser_T* parser);
AST_T* parser_parse_U(parser_T* parser);
AST_T* parser_parse_POST(parser_T* parser);
AST_T* parser_parse_P(parser_T* parser);
AST_T* parser_parse_FunCall(parser_T* parser);

AST_T* parser_parse_WORD(parser_T* parser);
AST_T* parser_parse_NUM(parser_T* parser);
#endif
