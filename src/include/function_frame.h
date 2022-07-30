#ifndef __FUNCTION_FRAME_H__
#define __FUNCTION_FRAME_H__
#include <stdio.h>
#include "list.h"
#include "type.h"

typedef struct {
	char* name;
	list_T* parameters;
	var_type_T* return_type;
	size_t curr_offset;
} fun_frame_T;

fun_frame_T* fun_frame_init(char* name, var_type_T* return_type);
int par_num_to_reg(size_t num);
#endif
