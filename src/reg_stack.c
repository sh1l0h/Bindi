#include <stdlib.h>
#include "./include/reg_stack.h"

rstack_T* rstack_init()
{
	rstack_T* result = (rstack_T*) malloc(sizeof(rstack_T));
	result->registers = (int*) malloc(sizeof(int)*14);
	result->top = 6;

	result->registers[6] = R_RBX;
	result->registers[5] = R_R10;
	result->registers[4] = R_R11;
	result->registers[3] = R_R12;
	result->registers[2] = R_R13;
	result->registers[1] = R_R14;
	result->registers[0] = R_R15;
	
	return result;
}

void rstack_free(rstack_T* stack)
{
	free(stack->registers);
	free(stack);
}

int rstack_pop(rstack_T* stack)
{
	return stack->registers[stack->top--];
}

int rstack_is_reg_free(rstack_T* stack, int reg){
	for(int i = stack->top; i >= 0; i--){
		if(stack->registers[i] == reg) return 1;
	}
	return 0;
}

void rstack_push(rstack_T* stack, int reg)
{
	stack->registers[++stack->top] = reg;
}
