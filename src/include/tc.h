#ifndef __TC_H__
#define __TC_H__
#include "type.h"
#include "ast.h"
#include "code_generator.h"

int TC_match_type(type_T* type, int argc, ...);

type_T* TC_get_type_BE(CG_T* cg, AST_T* tree);
type_T* TC_get_type_BT(CG_T* cg, AST_T* tree);
type_T* TC_get_type_BU(CG_T* cg, AST_T* tree);
type_T* TC_get_type_BP(CG_T* cg, AST_T* tree);

type_T* TC_get_type_E(CG_T* cg, AST_T* tree);
type_T* TC_get_type_T(CG_T* cg, AST_T* tree);
type_T* TC_get_type_U(CG_T* cg, AST_T* tree);
type_T* TC_get_type_P(CG_T* cg, AST_T* tree);

type_T* TC_get_type_id(CG_T* cg, AST_T* tree);
type_T* TC_get_type_NUM(CG_T* cg, AST_T* tree);

#endif 
