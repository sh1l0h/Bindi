#ifndef __REG_STACK_H__
#define __REG_STACK_H__

#include <stdio.h>

enum registers {
	R_RAX,
	R_RBX,
	R_RCX,
	R_RDX,
	R_RDI,
	R_RSI,
	R_R8,
	R_R9,
	R_R10,
	R_R11,
	R_R12,
	R_R13,
	R_R14,
	R_R15
};

typedef struct {
	int* registers;
	size_t top;
} rstack_T;

rstack_T* rstack_init();
void rstack_free(rstack_T* stack);

int rstack_pop(rstack_T* stack);

int rstack_is_reg_free(rstack_T* stack, int reg);

void rstack_push(rstack_T* stack, int reg);

#endif
