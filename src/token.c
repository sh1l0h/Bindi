#include "include/token.h"
#include <stdio.h>

token_T* init_token(int type, char* arg, char* file, size_t line, size_t column)
{
	token_T* result = (token_T*) malloc(sizeof(token_T));
	result->type = type;
	result->arg = arg;
	result->file = file;
	result->line = line;
	result->column = column;

	return result;
}

const char* token_type_to_str(int type)
{
	switch (type){
		case PLUS:		return "PLUS";
		case MINUS:		return "MINUS";
		case STAR:		return "STAR";
		case SLASH:		return "SLASH";
		case CARET:		return "CARET";
		case BANG:      return "BANG";

		case DOT:       return "DOT";
		case COMMA:     return "COMMA";

		case EQ:		return "EQ";
		case EQ_EQ:		return "EQ_EQ";
		case LT:		return "LT";
		case GT:		return "GT";
		case GEQ:		return "GEQ"; 
		case LEQ:		return "LEQ"; 
		case NEQ:		return "NEQ"; 

		case COLON:		return "COLON";
		case SEMICOLON: return "SEMICOLON";

		case LEFT_B:	return "LEFT_B";
		case RIGHT_B:	return "RIGHT_B";
		case LEFT_SB:	return "LEFT_SB";
		case RIGHT_SB:	return "RIGHT_SB";
		case LEFT_P:	return "LEFT_P";
		case RIGHT_P:	return "RIGHT_P";

		case RIGHT_ARR: return "RIGHT_ARR";
		case LEFT_ARR: return "LEFT_ARR";

		case WORD:		return "WORD";
		case STR:		return "STR";
		case NUM:		return "NUM";
		case END:		return "END";
		case T_EOF:		return "T_EOF";
	}
}

char* token_to_str(token_T* token)
{
	const char* type = token_type_to_str(token->type);
	const char* temp = "<Type = %s, Line = %ld, Column = %ld>";

	char* result = (char*) calloc(sizeof(type) + sizeof(temp) + 32, sizeof(char));
	sprintf(result, temp, type, token->line, token->column);
		 
	return result;
}
