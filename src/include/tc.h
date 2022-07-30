#ifndef __TC_H__
#define __TC_H__
#include <stdio.h>
#include "type.h"
#include "ast.h"
#include "code_generator.h"

int TC_match_type(var_type_T* type, size_t ptr_depth, int argc, ...);

var_type_T* TC_get_type_BE(CG_T* cg, AST_T* tree);
var_type_T* TC_get_type_BT(CG_T* cg, AST_T* tree);
var_type_T* TC_get_type_BU(CG_T* cg, AST_T* tree);
var_type_T* TC_get_type_BP(CG_T* cg, AST_T* tree);
var_type_T* TC_get_type_E(CG_T* cg, AST_T* tree);
var_type_T* TC_get_type_T(CG_T* cg, AST_T* tree);
var_type_T* TC_get_type_U(CG_T* cg, AST_T* tree);
var_type_T* TC_get_type_POST(CG_T* cg, AST_T* tree);
var_type_T* TC_get_type_P(CG_T* cg, AST_T* tree);
var_type_T* TC_get_type_FunCall(CG_T* cg, AST_T* tree);
var_type_T* TC_get_type_WORD(CG_T* cg, AST_T* tree);
var_type_T* TC_get_type_NUM(CG_T* cg, AST_T* tree);

#endif 
