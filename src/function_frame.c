#include <stdlib.h>
#include "./include/reg_stack.h"
#include "./include/symtable.h"
#include "./include/function_frame.h"

fun_frame_T* fun_frame_init(char* name, var_type_T* return_type)
{
	fun_frame_T* result = (fun_frame_T*) malloc(sizeof(fun_frame_T));
	result->name = name;
	result->parameters = list_init(sizeof(symbol_T*));
	result->return_type = return_type;
	result->curr_offset = 0;
	return result;
}

int par_num_to_reg(size_t num)
{
	switch(num){
		case 0: return R_RDI;
		case 1: return R_RSI;
		case 2: return R_RDX;
		case 3: return R_RCX;
		case 4: return R_R8;
		case 5: return R_R9;
	}
}
