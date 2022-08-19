#include <string.h>
#include "./include/tc.h"
#include "./include/code_generator.h"
#include "./include/reg_stack.h"
#include "./include/token.h"
#include "./include/str_lit.h"
#include "./include/function_frame.h"


const char* CG_register_to_str(int reg, size_t size)
{
	switch(reg){
		case R_RAX:
			switch(size){
				case 1:
					return "al";
				case 2:
					return "ax";
				case 4:
					return "eax";
				default:
					return "rax";
			}

		case R_RBX:
			switch(size){
				case 1:
					return "bl";
				case 2:
					return "bx";
				case 4:
					return "ebx";
				default:
					return "rbx";
			}

		case R_RCX:
			switch(size){
				case 1:
					return "cl";
				case 2:
					return "cx";
				case 4:
					return "ecx";
				default:
					return "rcx";
			}

		case R_RDX:
			switch(size){
				case 1:
					return "dl";
				case 2:
					return "dx";
				case 4:
					return "edx";
				default:
					return "rdx";
			}

		case R_RDI:
			switch(size){
				case 1:
					return "dil";
				case 2:
					return "di";
				case 4:
					return "edi";
				default:
					return "rdi";
			}

		case R_RSI:
			switch(size){
				case 1:
					return "sil";
				case 2:
					return "si";
				case 4:
					return "esi";
				default:
					return "rsi";
			}

		case R_R8:
			switch(size){
				case 1:
					return "r8b";
				case 2:
					return "r8w";
				case 4:
					return "r8d";
				default:
					return "r8";
			}

		case R_R9:
			switch(size){
				case 1:
					return "r9b";
				case 2:
					return "r9w";
				case 4:
					return "r9d";
				default:
					return "r9";
			}

		case R_R10:
			switch(size){
				case 1:
					return "r10b";
				case 2:
					return "r10w";
				case 4:
					return "r10d";
				default:
					return "r10";
			}

		case R_R11:
			switch(size){
				case 1:
					return "r11b";
				case 2:
					return "r11w";
				case 4:
					return "r11d";
				default:
					return "r11";
			}

		case R_R12:
			switch(size){
				case 1:
					return "r12b";
				case 2:
					return "r12w";
				case 4:
					return "r12d";
				default:
					return "r12";
			}

		case R_R13:
			switch(size){
				case 1:
					return "r13b";
				case 2:
					return "r13w";
				case 4:
					return "r13d";
				default:
					return "r13";
			}

		case R_R14:
			switch(size){
				case 1:
					return "r14b";
				case 2:
					return "r14w";
				case 4:
					return "r14d";
				default:
					return "r14";
			}

		case R_R15:
			switch(size){
				case 1:
					return "r15b";
				case 2:
					return "r15w";
				case 4:
					return "r15d";
				default:
					return "r15";
			}
	}
	return "";
}

CG_T* CG_init(char* file_name)
{
	CG_T* result = (CG_T*)calloc(1, sizeof(CG_T));
	result->output_file = fopen(file_name, "w");
	result->table = symtab_init();
	result->stack = rstack_init();
	result->types = list_init(sizeof(type_T*));
	result->str_lits = list_init(sizeof(str_lit_T*));
	result->funs = list_init(sizeof(fun_frame_T*));

	list_add(result->types, (void*) type_init("void", 0, T_PRIM));
	list_add(result->types, (void*) type_init("byte", 1, T_PRIM));
	type_T* int_t = type_init("int", 4, T_PRIM);
	list_add(result->types, (void*) int_t);
	type_T* char_t = type_init("char", 1, T_PRIM);
	list_add(result->types, (void*) char_t);
	list_add(result->types, (void*) type_init("long", 8, T_PRIM));
	list_add(result->types, (void*) type_init("bool", 1, T_PRIM));
	type_T* str = type_init("string", 12, T_STRUCT);
	type_add_subtype(str, subtype_init("size", var_type_init(int_t, 0, 1)));
	type_add_subtype(str, subtype_init("str_ptr", var_type_init(char_t, 0, 1)));
	list_add(result->types, (void*) str);

	return result;
}

void CG_close(CG_T* cg)
{
	fclose(cg->output_file);
	rstack_free(cg->stack);
	symtab_free(cg->table);
	free(cg);
}

type_T* CG_find_type(CG_T* cg, char* name)
{
	list_T* types = cg->types;
	for(int i = 0; i < types->size; i++) {
		type_T* curr = (type_T*) list_get(types,i);
		if(!strcmp(curr->name, name)) return curr;
	}
	return NULL;
}

fun_frame_T* CG_find_function(CG_T* cg, char* name)
{
	list_T* funs = cg->funs;
	for(int i = 0; i < funs->size; i++) {
		fun_frame_T* curr = (fun_frame_T*) list_get(funs,i);
		if(!strcmp(curr->name, name)) return curr;
	}
	return NULL;
}

const char* CG_size_to_extension(size_t size)
{
	switch(size){
		case 1: return "BYTE";
		case 2: return "WORD";
		case 4: return "DWORD";
		case 8: return "QWORD";
		default: return "";
	}
}

var_type_T* CG_get_var_type(CG_T* cg, AST_T* tree)
{
	if(tree->children->size == 1) return var_type_init(CG_find_type(cg, AST_get_arg(AST_get_child(tree, 0))), 0, 1);
	var_type_T* t = CG_get_var_type(cg, AST_get_child(tree, 1)); 
	t->ptr_depth++;
	return t; 
}

int CG_code(CG_T* cg, AST_T* tree)
{
	switch(tree->type){
		case AST_PROG:
			{
				symtab_enter_scope(cg->table);
				fprintf(cg->output_file, "section .bss\n");
				for(int i = 0; i < tree->children->size; i++){
					AST_T* curr = AST_get_child(tree, i);
					if(curr->type != AST_VADEC) continue;
					var_type_T* type = CG_get_var_type(cg, AST_get_child(curr,3));
					symtab_add_symbol(cg->table, symbol_init(AST_get_arg(AST_get_child(curr,1)), type, SCOPE_GLOBAL));
					CG_code(cg, curr);
				}
				fprintf(cg->output_file, "section .text\n");
				fprintf(cg->output_file, "        global _start\n\n");
				fprintf(cg->output_file, "_start:\n");
				fprintf(cg->output_file, "        pop     rdi\n");
				fprintf(cg->output_file, "        mov     rsi, rsp\n");
				fprintf(cg->output_file, "        call    main\n");
				fprintf(cg->output_file, "        movsx   rdi, eax\n");
				fprintf(cg->output_file, "        mov     rax, 60\n");
				fprintf(cg->output_file, "        syscall\n\n");
				for(int i = 0; i < tree->children->size; i++){
					AST_T* curr = AST_get_child(tree, i);
					if(curr->type == AST_VADEC) continue;
					CG_code(cg, curr);
				}
				fprintf(cg->output_file, "print_char:\n");
				fprintf(cg->output_file, "        dec     rsp\n");
				fprintf(cg->output_file, "        mov     BYTE [rsp], dil\n");
				fprintf(cg->output_file, "        mov     rax, 1\n");
				fprintf(cg->output_file, "        mov     rdi, 1\n");
				fprintf(cg->output_file, "        mov     rdx, 1\n");
				fprintf(cg->output_file, "        mov     rsi, rsp\n");
				fprintf(cg->output_file, "        syscall\n");
				fprintf(cg->output_file, "        inc     rsp\n");
				fprintf(cg->output_file, "        ret\n");
				fprintf(cg->output_file, "print_int:\n");
				fprintf(cg->output_file, "        push    rbp\n");
				fprintf(cg->output_file, "        mov     rbp, rsp\n");
				fprintf(cg->output_file, "        sub     rsp, 64\n");
				fprintf(cg->output_file, "        mov     DWORD  [rbp-52], edi\n");
				fprintf(cg->output_file, "        mov     BYTE  [rbp-17], 0\n");
				fprintf(cg->output_file, "        mov     BYTE  [rbp-18], 10\n");
				fprintf(cg->output_file, "        mov     DWORD  [rbp-4], 0\n");
				fprintf(cg->output_file, "        cmp     DWORD  [rbp-52], 0\n");
				fprintf(cg->output_file, "        jns     .L2\n");
				fprintf(cg->output_file, "        mov     DWORD  [rbp-4], 1\n");
				fprintf(cg->output_file, "        neg     DWORD  [rbp-52]\n");
				fprintf(cg->output_file, ".L2:\n");
				fprintf(cg->output_file, "        mov     DWORD  [rbp-8], 0\n");
				fprintf(cg->output_file, "        cmp     DWORD  [rbp-52], 0\n");
				fprintf(cg->output_file, "        jne     .L5\n");
				fprintf(cg->output_file, "        add     DWORD  [rbp-8], 1\n");
				fprintf(cg->output_file, "        mov     eax, 30\n");
				fprintf(cg->output_file, "        sub     eax, DWORD  [rbp-8]\n");
				fprintf(cg->output_file, "        cdqe\n");
				fprintf(cg->output_file, "        mov     BYTE  [rbp-48+rax], 48\n");
				fprintf(cg->output_file, "        jmp     .L4\n");
				fprintf(cg->output_file, ".L6:\n");
				fprintf(cg->output_file, "        mov     ecx, DWORD  [rbp-52]\n");
				fprintf(cg->output_file, "        movsx   rax, ecx\n");
				fprintf(cg->output_file, "        imul    rax, rax, 1717986919\n");
				fprintf(cg->output_file, "        shr     rax, 32\n");
				fprintf(cg->output_file, "        mov     edx, eax\n");
				fprintf(cg->output_file, "        sar     edx, 2\n");
				fprintf(cg->output_file, "        mov     eax, ecx\n");
				fprintf(cg->output_file, "        sar     eax, 31\n");
				fprintf(cg->output_file, "        sub     edx, eax\n");
				fprintf(cg->output_file, "        mov     eax, edx\n");
				fprintf(cg->output_file, "        sal     eax, 2\n");
				fprintf(cg->output_file, "        add     eax, edx\n");
				fprintf(cg->output_file, "        add     eax, eax\n");
				fprintf(cg->output_file, "        sub     ecx, eax\n");
				fprintf(cg->output_file, "        mov     edx, ecx\n");
				fprintf(cg->output_file, "        mov     eax, edx\n");
				fprintf(cg->output_file, "        lea     edx, [rax+48]\n");
				fprintf(cg->output_file, "        add     DWORD  [rbp-8], 1\n");
				fprintf(cg->output_file, "        mov     eax, 30\n");
				fprintf(cg->output_file, "        sub     eax, DWORD  [rbp-8]\n");
				fprintf(cg->output_file, "        cdqe\n");
				fprintf(cg->output_file, "        mov     BYTE  [rbp-48+rax], dl\n");
				fprintf(cg->output_file, "        mov     eax, DWORD  [rbp-52]\n");
				fprintf(cg->output_file, "        movsx   rdx, eax\n");
				fprintf(cg->output_file, "        imul    rdx, rdx, 1717986919\n");
				fprintf(cg->output_file, "        shr     rdx, 32\n");
				fprintf(cg->output_file, "        mov     ecx, edx\n");
				fprintf(cg->output_file, "        sar     ecx, 2\n");
				fprintf(cg->output_file, "        cdq\n");
				fprintf(cg->output_file, "        mov     eax, ecx\n");
				fprintf(cg->output_file, "        sub     eax, edx\n");
				fprintf(cg->output_file, "        mov     DWORD  [rbp-52], eax\n");
				fprintf(cg->output_file, ".L5:\n");
				fprintf(cg->output_file, "        cmp     DWORD  [rbp-52], 0\n");
				fprintf(cg->output_file, "        jg      .L6\n");
				fprintf(cg->output_file, "        cmp     DWORD  [rbp-4], 0\n");
				fprintf(cg->output_file, "        je      .L4\n");
				fprintf(cg->output_file, "        add     DWORD  [rbp-8], 1\n");
				fprintf(cg->output_file, "        mov     eax, 30\n");
				fprintf(cg->output_file, "        sub     eax, DWORD  [rbp-8]\n");
				fprintf(cg->output_file, "        cdqe\n");
				fprintf(cg->output_file, "        mov     BYTE  [rbp-48+rax], 45\n");
				fprintf(cg->output_file, ".L4:\n");
				fprintf(cg->output_file, "        mov     eax, DWORD  [rbp-8]\n");
				fprintf(cg->output_file, "        lea     edx, [rax+2]\n");
				fprintf(cg->output_file, "        mov     eax, 30\n");
				fprintf(cg->output_file, "        sub     eax, DWORD  [rbp-8]\n");
				fprintf(cg->output_file, "        lea     rcx, [rbp-48]\n");
				fprintf(cg->output_file, "        cdqe\n");
				fprintf(cg->output_file, "        add     rax, rcx\n");
				fprintf(cg->output_file, "        mov     rsi, rax\n");
				fprintf(cg->output_file, "        mov     edi, 1\n");
				fprintf(cg->output_file, "        mov     eax, 0\n");
				fprintf(cg->output_file, "        mov     rax, 1\n");
				fprintf(cg->output_file, "        syscall\n");
				fprintf(cg->output_file, "        nop\n");
				fprintf(cg->output_file, "        leave\n");
				fprintf(cg->output_file, "        ret\n\n");

                fprintf(cg->output_file, "print_bool:\n");
                fprintf(cg->output_file, "        push    rbp\n");
                fprintf(cg->output_file, "        mov     rbp, rsp\n");
                fprintf(cg->output_file, "        sub     rsp, 32\n");
                fprintf(cg->output_file, "        mov     eax, edi\n");
                fprintf(cg->output_file, "        mov     BYTE  [rbp-20], al\n");
                fprintf(cg->output_file, "        mov     DWORD  [rbp-6], 1702195828\n");
                fprintf(cg->output_file, "        mov     WORD  [rbp-2], 10\n");
                fprintf(cg->output_file, "        mov     DWORD  [rbp-13], 1936482662\n");
                fprintf(cg->output_file, "        mov     DWORD  [rbp-10], 681331\n");
                fprintf(cg->output_file, "        cmp     BYTE  [rbp-20], 0\n");
                fprintf(cg->output_file, "        jne     .L8\n");
                fprintf(cg->output_file, "        lea     rax, [rbp-13]\n");
                fprintf(cg->output_file, "        mov     edx, 6\n");
                fprintf(cg->output_file, "        mov     rsi, rax\n");
                fprintf(cg->output_file, "        mov     edi, 1\n");
                fprintf(cg->output_file, "        mov     rax, 1\n");
                fprintf(cg->output_file, "        syscall\n");
                fprintf(cg->output_file, "        jmp     .L10\n");
                fprintf(cg->output_file, ".L8:\n");
                fprintf(cg->output_file, "        lea     rax, [rbp-6]\n");
                fprintf(cg->output_file, "        mov     edx, 5\n");
                fprintf(cg->output_file, "        mov     rsi, rax\n");
                fprintf(cg->output_file, "        mov     edi, 1\n");
                fprintf(cg->output_file, "        mov     rax, 1\n");
                fprintf(cg->output_file, "        syscall\n");
                fprintf(cg->output_file, ".L10:\n");
                fprintf(cg->output_file, "        nop\n");
                fprintf(cg->output_file, "        leave\n");
                fprintf(cg->output_file, "        ret\n");

                fprintf(cg->output_file, "print_byte:\n");
                fprintf(cg->output_file, "        push    rbp\n");
                fprintf(cg->output_file, "        mov     rbp, rsp\n");
                fprintf(cg->output_file, "        sub     rsp, 64\n");
                fprintf(cg->output_file, "        mov     eax, edi\n");
                fprintf(cg->output_file, "        mov     BYTE  [rbp-52], al\n");
                fprintf(cg->output_file, "        mov     BYTE  [rbp-17], 0\n");
                fprintf(cg->output_file, "        mov     BYTE  [rbp-18], 10\n");
                fprintf(cg->output_file, "        mov     BYTE  [rbp-1], 0\n");
                fprintf(cg->output_file, "        cmp     BYTE  [rbp-52], 0\n");
                fprintf(cg->output_file, "        jns     .L11\n");
                fprintf(cg->output_file, "        mov     BYTE  [rbp-1], 1\n");
                fprintf(cg->output_file, "        movzx   eax, BYTE  [rbp-52]\n");
                fprintf(cg->output_file, "        neg     eax\n");
                fprintf(cg->output_file, "        mov     BYTE  [rbp-52], al\n");
                fprintf(cg->output_file, ".L11:\n");
                fprintf(cg->output_file, "        mov     BYTE  [rbp-2], 0\n");
                fprintf(cg->output_file, "        cmp     BYTE  [rbp-52], 0\n");
                fprintf(cg->output_file, "        jne     .L13\n");
                fprintf(cg->output_file, "        movzx   eax, BYTE  [rbp-2]\n");
                fprintf(cg->output_file, "        add     eax, 1\n");
                fprintf(cg->output_file, "        mov     BYTE  [rbp-2], al\n");
                fprintf(cg->output_file, "        movsx   eax, BYTE  [rbp-2]\n");
                fprintf(cg->output_file, "        mov     edx, 30\n");
                fprintf(cg->output_file, "        sub     edx, eax\n");
                fprintf(cg->output_file, "        movsx   rax, edx\n");
                fprintf(cg->output_file, "        mov     BYTE  [rbp-48+rax], 48\n");
                fprintf(cg->output_file, "        jmp     .L14\n");
                fprintf(cg->output_file, ".L12:\n");
                fprintf(cg->output_file, "        movzx   edx, BYTE  [rbp-52]\n");
                fprintf(cg->output_file, "        mov     ecx, 103\n");
                fprintf(cg->output_file, "        mov     eax, ecx\n");
                fprintf(cg->output_file, "        imul    dl\n");
                fprintf(cg->output_file, "        shr     ax, 8\n");
                fprintf(cg->output_file, "        sar     al, 2\n");
                fprintf(cg->output_file, "        mov     esi, edx\n");
                fprintf(cg->output_file, "        sar     sil, 7\n");
                fprintf(cg->output_file, "        sub     eax, esi\n");
                fprintf(cg->output_file, "        mov     ecx, eax\n");
                fprintf(cg->output_file, "        mov     eax, ecx\n");
                fprintf(cg->output_file, "        sal     eax, 2\n");
                fprintf(cg->output_file, "        add     eax, ecx\n");
                fprintf(cg->output_file, "        add     eax, eax\n");
                fprintf(cg->output_file, "        mov     ecx, edx\n");
                fprintf(cg->output_file, "        sub     ecx, eax\n");
                fprintf(cg->output_file, "        mov     eax, ecx\n");
                fprintf(cg->output_file, "        lea     ecx, [rax+48]\n");
                fprintf(cg->output_file, "        movzx   eax, BYTE  [rbp-2]\n");
                fprintf(cg->output_file, "        add     eax, 1\n");
                fprintf(cg->output_file, "        mov     BYTE  [rbp-2], al\n");
                fprintf(cg->output_file, "        movsx   eax, BYTE  [rbp-2]\n");
                fprintf(cg->output_file, "        mov     edx, 30\n");
                fprintf(cg->output_file, "        sub     edx, eax\n");
                fprintf(cg->output_file, "        movsx   rax, edx\n");
                fprintf(cg->output_file, "        mov     BYTE  [rbp-48+rax], cl\n");
                fprintf(cg->output_file, "        movzx   edx, BYTE  [rbp-52]\n");
                fprintf(cg->output_file, "        mov     ecx, 103\n");
                fprintf(cg->output_file, "        mov     eax, ecx\n");
                fprintf(cg->output_file, "        imul    dl\n");
                fprintf(cg->output_file, "        shr     ax, 8\n");
                fprintf(cg->output_file, "        sar     al, 2\n");
                fprintf(cg->output_file, "        sar     dl, 7\n");
                fprintf(cg->output_file, "        sub     eax, edx\n");
                fprintf(cg->output_file, "        mov     BYTE  [rbp-52], al\n");
                fprintf(cg->output_file, ".L13:\n");
                fprintf(cg->output_file, "        cmp     BYTE  [rbp-52], 0\n");
                fprintf(cg->output_file, "        jg      .L12\n");
                fprintf(cg->output_file, "        cmp     BYTE  [rbp-1], 0\n");
                fprintf(cg->output_file, "        je      .L14\n");
                fprintf(cg->output_file, "        movzx   eax, BYTE  [rbp-2]\n");
                fprintf(cg->output_file, "        add     eax, 1\n");
                fprintf(cg->output_file, "        mov     BYTE  [rbp-2], al\n");
                fprintf(cg->output_file, "        movsx   eax, BYTE  [rbp-2]\n");
                fprintf(cg->output_file, "        mov     edx, 30\n");
                fprintf(cg->output_file, "        sub     edx, eax\n");
                fprintf(cg->output_file, "        movsx   rax, edx\n");
                fprintf(cg->output_file, "        mov     BYTE  [rbp-48+rax], 45\n");
                fprintf(cg->output_file, ".L14:\n");
                fprintf(cg->output_file, "        movsx   eax, BYTE  [rbp-2]\n");
                fprintf(cg->output_file, "        lea     edx, [rax+2]\n");
                fprintf(cg->output_file, "        movsx   eax, BYTE  [rbp-2]\n");
                fprintf(cg->output_file, "        mov     ecx, 30\n");
                fprintf(cg->output_file, "        mov     esi, ecx\n");
                fprintf(cg->output_file, "        sub     esi, eax\n");
                fprintf(cg->output_file, "        lea     rcx, [rbp-48]\n");
                fprintf(cg->output_file, "        movsx   rax, esi\n");
                fprintf(cg->output_file, "        add     rax, rcx\n");
                fprintf(cg->output_file, "        mov     rsi, rax\n");
                fprintf(cg->output_file, "        mov     edi, 1\n");
                fprintf(cg->output_file, "        mov     rax, 1\n");
                fprintf(cg->output_file, "        syscall\n");
                fprintf(cg->output_file, "        nop\n");
                fprintf(cg->output_file, "        leave\n");
                fprintf(cg->output_file, "        ret\n");
                fprintf(cg->output_file, "print_long:\n");
                fprintf(cg->output_file, "        push    rbp\n");
                fprintf(cg->output_file, "        mov     rbp, rsp\n");
                fprintf(cg->output_file, "        sub     rsp, 64\n");
                fprintf(cg->output_file, "        mov     QWORD  [rbp-56], rdi\n");
                fprintf(cg->output_file, "        mov     BYTE  [rbp-17], 0\n");
                fprintf(cg->output_file, "        mov     BYTE  [rbp-18], 10\n");
                fprintf(cg->output_file, "        mov     BYTE  [rbp-1], 0\n");
                fprintf(cg->output_file, "        cmp     QWORD  [rbp-56], 0\n");
                fprintf(cg->output_file, "        jns     .L15\n");
                fprintf(cg->output_file, "        mov     BYTE  [rbp-1], 1\n");
                fprintf(cg->output_file, "        neg     QWORD  [rbp-56]\n");
                fprintf(cg->output_file, ".L15:\n");
                fprintf(cg->output_file, "        mov     DWORD  [rbp-8], 0\n");
                fprintf(cg->output_file, "        cmp     QWORD  [rbp-56], 0\n");
                fprintf(cg->output_file, "        jne     .L17\n");
                fprintf(cg->output_file, "        add     DWORD  [rbp-8], 1\n");
                fprintf(cg->output_file, "        mov     eax, 30\n");
                fprintf(cg->output_file, "        sub     eax, DWORD  [rbp-8]\n");
                fprintf(cg->output_file, "        cdqe\n");
                fprintf(cg->output_file, "        mov     BYTE  [rbp-48+rax], 48\n");
                fprintf(cg->output_file, "        jmp     .L18\n");
                fprintf(cg->output_file, ".L16:\n");
                fprintf(cg->output_file, "        mov     rcx, QWORD  [rbp-56]\n");
                fprintf(cg->output_file, "        mov     rdx, 7378697629483820647\n");
                fprintf(cg->output_file, "        mov     rax, rcx\n");
                fprintf(cg->output_file, "        imul    rdx\n");
                fprintf(cg->output_file, "        sar     rdx, 2\n");
                fprintf(cg->output_file, "        mov     rax, rcx\n");
                fprintf(cg->output_file, "        sar     rax, 63\n");
                fprintf(cg->output_file, "        sub     rdx, rax\n");
                fprintf(cg->output_file, "        mov     rax, rdx\n");
                fprintf(cg->output_file, "        sal     rax, 2\n");
                fprintf(cg->output_file, "        add     rax, rdx\n");
                fprintf(cg->output_file, "        add     rax, rax\n");
                fprintf(cg->output_file, "        sub     rcx, rax\n");
                fprintf(cg->output_file, "        mov     rdx, rcx\n");
                fprintf(cg->output_file, "        mov     eax, edx\n");
                fprintf(cg->output_file, "        lea     edx, [rax+48]\n");
                fprintf(cg->output_file, "        add     DWORD  [rbp-8], 1\n");
                fprintf(cg->output_file, "        mov     eax, 30\n");
                fprintf(cg->output_file, "        sub     eax, DWORD  [rbp-8]\n");
                fprintf(cg->output_file, "        cdqe\n");
                fprintf(cg->output_file, "        mov     BYTE  [rbp-48+rax], dl\n");
                fprintf(cg->output_file, "        mov     rcx, QWORD  [rbp-56]\n");
                fprintf(cg->output_file, "        mov     rdx, 7378697629483820647\n");
                fprintf(cg->output_file, "        mov     rax, rcx\n");
                fprintf(cg->output_file, "        imul    rdx\n");
                fprintf(cg->output_file, "        mov     rax, rdx\n");
                fprintf(cg->output_file, "        sar     rax, 2\n");
                fprintf(cg->output_file, "        sar     rcx, 63\n");
                fprintf(cg->output_file, "        mov     rdx, rcx\n");
                fprintf(cg->output_file, "        sub     rax, rdx\n");
                fprintf(cg->output_file, "        mov     QWORD  [rbp-56], rax\n");
                fprintf(cg->output_file, ".L17:\n");
                fprintf(cg->output_file, "        cmp     QWORD  [rbp-56], 0\n");
                fprintf(cg->output_file, "        jg      .L16\n");
                fprintf(cg->output_file, "        cmp     BYTE  [rbp-1], 0\n");
                fprintf(cg->output_file, "        je      .L18\n");
                fprintf(cg->output_file, "        add     DWORD  [rbp-8], 1\n");
                fprintf(cg->output_file, "        mov     eax, 30\n");
                fprintf(cg->output_file, "        sub     eax, DWORD  [rbp-8]\n");
                fprintf(cg->output_file, "        cdqe\n");
                fprintf(cg->output_file, "        mov     BYTE  [rbp-48+rax], 45\n");
                fprintf(cg->output_file, ".L18:\n");
                fprintf(cg->output_file, "        mov     eax, DWORD  [rbp-8]\n");
                fprintf(cg->output_file, "        lea     edx, [rax+2]\n");
                fprintf(cg->output_file, "        mov     eax, 30\n");
                fprintf(cg->output_file, "        sub     eax, DWORD  [rbp-8]\n");
                fprintf(cg->output_file, "        lea     rcx, [rbp-48]\n");
                fprintf(cg->output_file, "        cdqe\n");
                fprintf(cg->output_file, "        add     rax, rcx\n");
                fprintf(cg->output_file, "        mov     rsi, rax\n");
                fprintf(cg->output_file, "        mov     edi, 1\n");
                fprintf(cg->output_file, "        mov     rax, 1\n");
                fprintf(cg->output_file, "        syscall\n");
                fprintf(cg->output_file, "        nop\n");
                fprintf(cg->output_file, "        leave\n");
                fprintf(cg->output_file, "        ret\n");
                fprintf(cg->output_file, "print_string:");
                fprintf(cg->output_file, "        mov    rax, 1\n");
                fprintf(cg->output_file, "        mov     rsi, rdi\n");
                fprintf(cg->output_file, "        mov     DWORD edx, [rdi-4]\n");
                fprintf(cg->output_file, "        mov     rdi, 1\n");
                fprintf(cg->output_file, "        syscall\n");
                fprintf(cg->output_file, "        ret\n");

                fprintf(cg->output_file, "section .data\n");
				for(int i = 0; i < cg->str_lits->size; i++){
					str_lit_T* curr = (str_lit_T*) list_get(cg->str_lits, i);
					fprintf(cg->output_file, "        sl%ld_size: dd %ld\n", curr->id, curr->initial_size);
					fprintf(cg->output_file, "        sl%ld: db %s\n", curr->id, str_to_lit(curr->str));
				}

				symtab_exit_scope(cg->table);
				return -1;
			}

		case AST_FUNDEC:
			{
				symtab_enter_scope(cg->table);
                fprintf(cg->output_file, "%s:\n", AST_get_arg(AST_get_child(tree,0)));
                fprintf(cg->output_file, "        push    rbp\n");
                fprintf(cg->output_file, "        mov     rbp, rsp\n");
				var_type_T* ret_type = CG_get_var_type(cg, AST_get_child(tree,4));  
				ret_type->is_lvalue = 0;
				list_add(cg->funs, fun_frame_init(AST_get_arg(AST_get_child(tree,0)), ret_type));
				CG_code(cg, AST_get_child(tree, 2));
				for(int i = 5; i < tree->children->size; i++){
					AST_T* curr = AST_get_child(tree, i);
					if(curr->type == AST_ST) CG_code(cg, curr);
				}
                fprintf(cg->output_file, ".end:\n");
                fprintf(cg->output_file, "        leave\n");
                fprintf(cg->output_file, "        ret\n");
				symtab_exit_scope(cg->table);
				return -1;
			}

		case AST_PARS:
			{
				size_t args_count = 0;
				for(int i = 0; i < tree->children->size; i++){
					AST_T* curr = AST_get_child(tree, i);
					if(curr->type != AST_PAR) continue;
					fun_frame_T* fframe = (fun_frame_T*)list_get(cg->funs, cg->funs->size -1);
					var_type_T* t = CG_get_var_type(cg, AST_get_child(curr,2));
					size_t size = var_type_size(t);
					symbol_T* sym = symbol_init(AST_get_arg(AST_get_child(curr,0)), t, SCOPE_ARG);
					fframe->curr_offset += size;
					sym->offset = fframe->curr_offset;
					list_add(fframe->parameters,(void*)sym);
					symtab_add_symbol(cg->table, sym);
					fprintf(cg->output_file, "        sub     rsp, %ld\n", size); 
					fprintf(cg->output_file, "        mov     %s [rbp-%ld], %s\n", CG_size_to_extension(size), fframe->curr_offset, CG_register_to_str(par_num_to_reg(args_count), size));
					args_count++;
				}
				return -1;
			}

		case AST_BLOCK:
			{
				symtab_enter_scope(cg->table);
				fun_frame_T* fframe = (fun_frame_T*)list_get(cg->funs, cg->funs->size -1);
				int t = fframe->curr_offset;
				for(int i = 0; i < tree->children->size; i++) CG_code(cg, AST_get_child(tree, i));
				fframe->curr_offset = t;
				symtab_exit_scope(cg->table);
				return -1;
			}

		case AST_VADEC:
			{

				list_T* l = symtab_get_scope(cg->table);
				symbol_T* sym = list_get(l, l->size - 1);
				fprintf(cg->output_file, "        %s: resb %ld\n", sym->name, var_type_size(sym->type));
				return -1;
			}
			

		case AST_ST:
			{
				if(tree->children->size == 1) return CG_code(cg, AST_get_child(tree, 0));
				AST_T* mid = AST_get_child(tree, 1);
				if(AST_get_child(tree,0)->type == AST_BE){
					var_type_T* t1 = TC_get_type_BE(cg, AST_get_child(tree, 0));
					if(!t1->is_lvalue){
						printf("%s:%ld:%ld: error: lvalue is expected as left value of assigment\n", tree->file, tree->line, tree->column);
						//TODO: free
						exit(1);
					}
					var_type_T* t2 = TC_get_type_BE(cg, AST_get_child(tree, 2));
					if(!var_type_cmp(t1, t2) && (!TC_match_type(t1, 0, 3, "int", "byte", "long") || !TC_match_type(t2, 0, 3, "int", "byte", "long"))){
						printf("%s:%ld:%ld: error: type error\n", tree->file, tree->line, tree->column);
						exit(1);
					}
					else if(TC_match_type(t1, 0, 1, "byte") && !TC_match_type(t2, 0, 1, "byte")){
						printf("%s:%ld:%ld: error: value of type %s cannot be assigned to a variable of type bype\n", tree->file, tree->line, tree->column, t2->type->name);
						exit(1);
					}
					else if(TC_match_type(t1, 0, 1, "int") && TC_match_type(t2, 0, 1, "long")){
						printf("%s:%ld:%ld: error: value of type %s cannot be assigned to a variable of type int\n", tree->file, tree->line, tree->column, t2->type->name);
						exit(1);
					}
					size_t size = var_type_size(t1);
					cg->curr_size = size; 
					cg->is_value_needed = 1;	
					int reg2 = CG_code(cg, AST_get_child(tree, 2));
					cg->curr_size = 8;
					cg->is_value_needed = 0;	
					int reg1 = CG_code(cg, AST_get_child(tree, 0));
					if(mid->type == AST_PLUS_EQ)
						fprintf(cg->output_file, "        add     %s, %s [%s]\n",  CG_register_to_str(reg2, size), CG_size_to_extension(size), CG_register_to_str(reg1, 8));
					else if(mid->type == AST_MINUS_EQ)
						fprintf(cg->output_file, "        sub     %s, %s [%s]\n",  CG_register_to_str(reg2, size), CG_size_to_extension(size), CG_register_to_str(reg1, 8));
					fprintf(cg->output_file, "        mov     %s [%s], %s\n", CG_size_to_extension(size), CG_register_to_str(reg1, 8), CG_register_to_str(reg2, size));
					rstack_push(cg->stack, reg1);
					rstack_push(cg->stack, reg2);

					return -1;	
				}
				if(AST_get_child(tree,0)->type == AST_WHILE){
					size_t lable = cg->lable_count;
					cg->lable_count += 2;
					fprintf(cg->output_file, ".l%ld:\n", lable);
					if(!TC_match_type(TC_get_type_BE(cg, AST_get_child(tree, 1)), 0, 1, "bool")){
						printf("error: while operator expects value of type bool as a condition\n");
						exit(1);
					}
					int reg1 = CG_code(cg, AST_get_child(tree, 1));
					rstack_push(cg->stack, reg1);
					fprintf(cg->output_file, "        cmp     %s, 0\n", CG_register_to_str(reg1, 1));
					fprintf(cg->output_file, "        je      .l%ld\n", lable+1);
					CG_code(cg, AST_get_child(tree,2));
					fprintf(cg->output_file, "        jmp     .l%ld\n", lable);
					fprintf(cg->output_file, ".l%ld:\n", lable+1);

					return -1;
				}
				if(AST_get_child(tree,0)->type == AST_IF){
					size_t lable = cg->lable_count;
					int has_else = tree->children->size > 3; 
					cg->lable_count += has_else ? 2 : 1;
					if(!TC_match_type(TC_get_type_BE(cg, AST_get_child(tree, 1)), 0, 1, "bool")){
						printf("error: if operator expects value of type bool as a condition\n");
						exit(1);
					}
					int reg1 = CG_code(cg, AST_get_child(tree, 1));
					rstack_push(cg->stack, reg1);
					fprintf(cg->output_file, "        cmp     %s, 0\n", CG_register_to_str(reg1, 1));
					fprintf(cg->output_file, "        je      .l%ld\n", lable);
					CG_code(cg, AST_get_child(tree, 2));
					if(has_else) fprintf(cg->output_file, "        jmp     .l%ld\n", lable+1); 
					fprintf(cg->output_file, ".l%ld:\n", lable);
					if(has_else){
						CG_code(cg, AST_get_child(tree, 4));
						fprintf(cg->output_file, ".l%ld:\n", lable + 1);
					}

					return -1;
				}
				if(AST_get_child(tree,0)->type == AST_PRINT){
					var_type_T* etype = TC_get_type_BE(cg, AST_get_child(tree, 1));
					if(etype->ptr_depth != 0);// error
					cg->curr_size = var_type_size(etype); 
					cg->is_value_needed = 1;
					int reg = CG_code(cg, AST_get_child(tree, 1));
					fprintf(cg->output_file, "        mov     %s, %s\n", CG_register_to_str(R_RDI, cg->curr_size), CG_register_to_str(reg, cg->curr_size));
					fprintf(cg->output_file, "        call    print_%s\n", etype->type->name);
					rstack_push(cg->stack, reg);
					return -1;
				}
				if(AST_get_child(tree,0)->type == AST_LOCVADEC)	return CG_code(cg, AST_get_child(tree, 0));
				if(AST_get_child(tree,0)->type == AST_RETURN){
					fun_frame_T* fframe = (fun_frame_T*)list_get(cg->funs, cg->funs->size -1);
					var_type_T* t1 = fframe->return_type;
					var_type_T* t2 = TC_get_type_BE(cg, AST_get_child(tree, 1));
					if(!var_type_cmp(t1, t2) && (!TC_match_type(t1, 0, 3, "int", "byte", "long") || !TC_match_type(t2, 0, 3, "int", "byte", "long"))){
						printf("%s:%ld:%ld: error: type error\n", tree->file, tree->line, tree->column);
						exit(1);
					}
					else if(TC_match_type(t1, 0, 1, "byte") && !TC_match_type(t2, 0, 1, "byte")){
						printf("%s:%ld:%ld: error: value of type %s cannot be assigned to a variable of type bype\n", tree->file, tree->line, tree->column, t2->type->name);
						exit(1);
					}
					else if(TC_match_type(t1, 0, 1, "int") && TC_match_type(t2, 0, 1, "long")){
						printf("%s:%ld:%ld: error: value of type %s cannot be assigned to a variable of type int\n", tree->file, tree->line, tree->column, t2->type->name);
						exit(1);
					}
					cg->is_value_needed = 1;
					cg->curr_size = var_type_size(t1);
					int reg = CG_code(cg, AST_get_child(tree,1));
					fprintf(cg->output_file, "        mov     %s, %s\n", CG_register_to_str(R_RAX, cg->curr_size), CG_register_to_str(reg, cg->curr_size));
					fprintf(cg->output_file, "        jmp     .end\n");
					rstack_push(cg->stack, reg);
					return -1;
				}
			}

		case AST_LOCVADEC:
			{
				fun_frame_T* fframe = (fun_frame_T*)list_get(cg->funs, cg->funs->size -1);
				var_type_T* t1 = CG_get_var_type(cg, AST_get_child(tree,3));
				symbol_T* sym = symbol_init(AST_get_arg(AST_get_child(tree,1)), t1, SCOPE_LOCAL); 
				size_t size = var_type_size(t1);
				fframe->curr_offset += size;
				sym->offset = fframe->curr_offset; 
				symtab_add_symbol(cg->table, sym);
				fprintf(cg->output_file, "        sub     rsp, %ld\n", size); 

				if(tree->children->size > 4){
					var_type_T* t2 = TC_get_type_BE(cg, AST_get_child(tree, 5));
					if(!var_type_cmp(t1, t2) && (!TC_match_type(t1, 0, 3, "int", "byte", "long") || !TC_match_type(t2, 0, 3, "int", "byte", "long"))){
						printf("%s:%ld:%ld: error: type error\n", tree->file, tree->line, tree->column);
						exit(1);
					}
					else if(TC_match_type(t1, 0, 1, "byte") && !TC_match_type(t2, 0, 1, "byte")){
						// TODO: error
						exit(1);
					}
					else if(TC_match_type(t1, 0, 1, "int") && TC_match_type(t2, 0, 1, "long")){
						exit(1);
					}
					cg->curr_size = size;
					cg->is_value_needed = 1;
					int reg = CG_code(cg, AST_get_child(tree,5));
					rstack_push(cg->stack, reg);
					fprintf(cg->output_file, "        mov     %s [rbp-%ld], %s\n", CG_size_to_extension(size), sym->offset, CG_register_to_str(reg, size));
				}
				
				return -1;
			}

		case AST_BE:
			{
				if(tree->children->size == 1) return CG_code(cg, AST_get_child(tree,0));
				int reg1 = -1;
				int reg2 = -1;

				if(AST_count_nodes(AST_get_child(tree, 0)) > AST_count_nodes(AST_get_child(tree,2))){
					reg1 = CG_code(cg, AST_get_child(tree,0));
					if(AST_has_child_with_type(AST_get_child(tree,2), AST_FUNCALL)){
						fprintf(cg->output_file, "        sub     rsp, %ld\n", cg->curr_size);
						fprintf(cg->output_file, "        mov     %s [rsp], %s\n", CG_size_to_extension(cg->curr_size), CG_register_to_str(reg1, cg->curr_size));
						rstack_push(cg->stack,reg1);
						reg2 = CG_code(cg, AST_get_child(tree,2));
						reg1 = rstack_pop(cg->stack);
						fprintf(cg->output_file, "        mov     %s, %s [rsp]\n", CG_register_to_str(reg1, cg->curr_size), CG_size_to_extension(cg->curr_size));
						fprintf(cg->output_file, "        add     rsp, %ld\n", cg->curr_size);
					}
					else reg2 = CG_code(cg, AST_get_child(tree,2));
				}
				else{
					reg2 = CG_code(cg, AST_get_child(tree,2));
					if(AST_has_child_with_type(AST_get_child(tree,0), AST_FUNCALL)){
						fprintf(cg->output_file, "        sub     rsp, %ld\n", cg->curr_size);
						fprintf(cg->output_file, "        mov     %s [rsp], %s\n", CG_size_to_extension(cg->curr_size), CG_register_to_str(reg2, cg->curr_size));
						rstack_push(cg->stack,reg2);
						reg1 = CG_code(cg, AST_get_child(tree,0));
						reg2 = rstack_pop(cg->stack);
						fprintf(cg->output_file, "        mov     %s, %s [rsp]\n", CG_register_to_str(reg2, cg->curr_size), CG_size_to_extension(cg->curr_size));
						fprintf(cg->output_file, "        add     rsp, %ld\n", cg->curr_size);
					}
					else reg1 = CG_code(cg, AST_get_child(tree,0));
				}

				fprintf(cg->output_file, "        or      %s, %s\n", CG_register_to_str(reg1, 1),CG_register_to_str(reg2, 1));

				rstack_push(cg->stack, reg2);
				return reg1;
			}

		case AST_BT:
			{
				if(tree->children->size == 1) return CG_code(cg, AST_get_child(tree,0));
				int reg1 = -1;
				int reg2 = -1;

				if(AST_count_nodes(AST_get_child(tree, 0)) > AST_count_nodes(AST_get_child(tree,2))){
					reg1 = CG_code(cg, AST_get_child(tree,0));
					if(AST_has_child_with_type(AST_get_child(tree,2), AST_FUNCALL)){
						fprintf(cg->output_file, "        sub     rsp, %ld\n", cg->curr_size);
						fprintf(cg->output_file, "        mov     %s [rsp], %s\n", CG_size_to_extension(cg->curr_size), CG_register_to_str(reg1, cg->curr_size));
						rstack_push(cg->stack,reg1);
						reg2 = CG_code(cg, AST_get_child(tree,2));
						reg1 = rstack_pop(cg->stack);
						fprintf(cg->output_file, "        mov     %s, %s [rsp]\n", CG_register_to_str(reg1, cg->curr_size), CG_size_to_extension(cg->curr_size));
						fprintf(cg->output_file, "        add     rsp, %ld\n", cg->curr_size);
					}
					else reg2 = CG_code(cg, AST_get_child(tree,2));
				}
				else{
					reg2 = CG_code(cg, AST_get_child(tree,2));
					if(AST_has_child_with_type(AST_get_child(tree,0), AST_FUNCALL)){
						fprintf(cg->output_file, "        sub     rsp, %ld\n", cg->curr_size);
						fprintf(cg->output_file, "        mov     %s [rsp], %s\n", CG_size_to_extension(cg->curr_size), CG_register_to_str(reg2, cg->curr_size));
						rstack_push(cg->stack,reg2);
						reg1 = CG_code(cg, AST_get_child(tree,0));
						reg2 = rstack_pop(cg->stack);
						fprintf(cg->output_file, "        mov     %s, %s [rsp]\n", CG_register_to_str(reg2, cg->curr_size), CG_size_to_extension(cg->curr_size));
						fprintf(cg->output_file, "        add     rsp, %ld\n", cg->curr_size);
					}
					else reg1 = CG_code(cg, AST_get_child(tree,0));
				}

				fprintf(cg->output_file, "        and     %s, %s\n", CG_register_to_str(reg1, 1),CG_register_to_str(reg2, 1));

				rstack_push(cg->stack, reg2);
				return reg1;
			}

		case AST_BU:
			{
				if(tree->children->size == 1) return CG_code(cg, AST_get_child(tree,0));

				int reg = CG_code(cg, AST_get_child(tree,1));
				fprintf(cg->output_file, "        cmp     %s, 0\n", CG_register_to_str(reg, 1));
				fprintf(cg->output_file, "        sete    %s\n", CG_register_to_str(reg, 1));
				return reg;
			}

		case AST_BP:
			{
				if(tree->children->size == 1) return CG_code(cg, AST_get_child(tree,0));
				int reg1 = -1;
				int reg2 = -1;

				size_t size1 = TC_get_type_E(cg, AST_get_child(tree,0))->type->size;
				size_t size2 = TC_get_type_E(cg, AST_get_child(tree,2))->type->size;

				cg->curr_size = MAX(size1, size2);
				cg->is_value_needed = 1;
				if(AST_count_nodes(AST_get_child(tree, 0)) > AST_count_nodes(AST_get_child(tree,2))){
					reg1 = CG_code(cg, AST_get_child(tree,0));
					if(AST_has_child_with_type(AST_get_child(tree,2), AST_FUNCALL)){
						fprintf(cg->output_file, "        sub     rsp, %ld\n", cg->curr_size);
						fprintf(cg->output_file, "        mov     %s [rsp], %s\n", CG_size_to_extension(cg->curr_size), CG_register_to_str(reg1, cg->curr_size));
						rstack_push(cg->stack,reg1);
						reg2 = CG_code(cg, AST_get_child(tree,2));
						reg1 = rstack_pop(cg->stack);
						fprintf(cg->output_file, "        mov     %s, %s [rsp]\n", CG_register_to_str(reg1, cg->curr_size), CG_size_to_extension(cg->curr_size));
						fprintf(cg->output_file, "        add     rsp, %ld\n", cg->curr_size);
					}
					else reg2 = CG_code(cg, AST_get_child(tree,2));
				}
				else{
					reg2 = CG_code(cg, AST_get_child(tree,2));
					if(AST_has_child_with_type(AST_get_child(tree,0), AST_FUNCALL)){
						fprintf(cg->output_file, "        sub     rsp, %ld\n", cg->curr_size);
						fprintf(cg->output_file, "        mov     %s [rsp], %s\n", CG_size_to_extension(cg->curr_size), CG_register_to_str(reg2, cg->curr_size));
						rstack_push(cg->stack,reg2);
						reg1 = CG_code(cg, AST_get_child(tree,0));
						reg2 = rstack_pop(cg->stack);
						fprintf(cg->output_file, "        mov     %s, %s [rsp]\n", CG_register_to_str(reg2, cg->curr_size), CG_size_to_extension(cg->curr_size));
						fprintf(cg->output_file, "        add     rsp, %ld\n", cg->curr_size);
					}
					else reg1 = CG_code(cg, AST_get_child(tree,0));
				}

				fprintf(cg->output_file, "        cmp     %s, %s\n", CG_register_to_str(reg1, cg->curr_size), CG_register_to_str(reg2, cg->curr_size));
				switch(AST_get_child(tree,1)->type){
					case AST_GT:
						fprintf(cg->output_file, "        setg    %s\n", CG_register_to_str(reg1, 1));
						break;
					case AST_LT:
						fprintf(cg->output_file, "        setl    %s\n", CG_register_to_str(reg1, 1));
						break;

					case AST_GEQ:
						fprintf(cg->output_file, "        setge   %s\n", CG_register_to_str(reg1, 1));
						break;

					case AST_LEQ:
						fprintf(cg->output_file, "        setle   %s\n", CG_register_to_str(reg1, 1));
						break;

					case AST_EQ_EQ:
						fprintf(cg->output_file, "        sete    %s\n", CG_register_to_str(reg1, 1));
						break;

					case AST_NEQ:
						fprintf(cg->output_file, "        setne   %s\n", CG_register_to_str(reg1, 1));
						break;
				}
				rstack_push(cg->stack, reg2);

				return reg1;
			}

		case AST_E:
			{
				if(tree->children->size == 1) return CG_code(cg, AST_get_child(tree,0));

				int reg1 = 0;
				int reg2 = 0;
				if(AST_count_nodes(AST_get_child(tree, 0)) > AST_count_nodes(AST_get_child(tree,2))){
					reg1 = CG_code(cg, AST_get_child(tree,0));
					if(AST_has_child_with_type(AST_get_child(tree,2), AST_FUNCALL)){
						fprintf(cg->output_file, "        sub     rsp, %ld\n", cg->curr_size);
						fprintf(cg->output_file, "        mov     %s [rsp], %s\n", CG_size_to_extension(cg->curr_size), CG_register_to_str(reg1, cg->curr_size));
						rstack_push(cg->stack,reg1);
						reg2 = CG_code(cg, AST_get_child(tree,2));
						reg1 = rstack_pop(cg->stack);
						fprintf(cg->output_file, "        mov     %s, %s [rsp]\n", CG_register_to_str(reg1, cg->curr_size), CG_size_to_extension(cg->curr_size));
						fprintf(cg->output_file, "        add     rsp, %ld\n", cg->curr_size);
					}
					else reg2 = CG_code(cg, AST_get_child(tree,2));
				}
				else{
					reg2 = CG_code(cg, AST_get_child(tree,2));
					if(AST_has_child_with_type(AST_get_child(tree,0), AST_FUNCALL)){
						fprintf(cg->output_file, "        sub     rsp, %ld\n", cg->curr_size);
						fprintf(cg->output_file, "        mov     %s [rsp], %s\n", CG_size_to_extension(cg->curr_size), CG_register_to_str(reg2, cg->curr_size));
						rstack_push(cg->stack,reg2);
						reg1 = CG_code(cg, AST_get_child(tree,0));
						reg2 = rstack_pop(cg->stack);
						fprintf(cg->output_file, "        mov     %s, %s [rsp]\n", CG_register_to_str(reg2, cg->curr_size), CG_size_to_extension(cg->curr_size));
						fprintf(cg->output_file, "        add     rsp, %ld\n", cg->curr_size);
					}
					else reg1 = CG_code(cg, AST_get_child(tree,0));
				}

				var_type_T* t1 = TC_get_type_T(cg, AST_get_child(tree,0));
				var_type_T* t2 = TC_get_type_E(cg, AST_get_child(tree,2));
				if(t1->ptr_depth > 0 && t2->ptr_depth == 0){
					t1->ptr_depth--;
					if(cg->curr_size != 1)
						fprintf(cg->output_file, "        imul    %s, %ld\n", CG_register_to_str(reg2, cg->curr_size), var_type_size(t1));
					else{
						fprintf(cg->output_file, "        mov     %s, %s\n", CG_register_to_str(R_RAX, cg->curr_size), CG_register_to_str(reg2, cg->curr_size));
						fprintf(cg->output_file, "        imul    %ld\n", var_type_size(t1));
						fprintf(cg->output_file, "        mov     %s, %s\n", CG_register_to_str(reg2, cg->curr_size), CG_register_to_str(R_RAX, cg->curr_size));
					}
				}
				else if(t2->ptr_depth > 0 && t1->ptr_depth == 0){
					t2->ptr_depth--;
					if(cg->curr_size != 1)
						fprintf(cg->output_file, "        imul    %s, %ld\n", CG_register_to_str(reg1, cg->curr_size), var_type_size(t2));
					else{
						fprintf(cg->output_file, "        mov     %s, %s\n", CG_register_to_str(R_RAX, cg->curr_size), CG_register_to_str(reg1, cg->curr_size));
						fprintf(cg->output_file, "        imul    %ld\n", var_type_size(t2));
						fprintf(cg->output_file, "        mov     %s, %s\n", CG_register_to_str(reg1, cg->curr_size), CG_register_to_str(R_RAX, cg->curr_size));
					}
				}

				if(AST_get_child(tree,1)->type == AST_PLUS)
					fprintf(cg->output_file, "        add     %s, %s\n", CG_register_to_str(reg1, cg->curr_size), CG_register_to_str(reg2, cg->curr_size));
				else
					fprintf(cg->output_file, "        sub     %s, %s\n", CG_register_to_str(reg1, cg->curr_size), CG_register_to_str(reg2, cg->curr_size));

				free(t1);
				free(t2);
				rstack_push(cg->stack, reg2);
				return reg1;
			}

		case AST_TERM:
			{
				if(tree->children->size == 1) return CG_code(cg, AST_get_child(tree,0));

				int reg1 = 0;
				int reg2 = 0;
				if(AST_count_nodes(AST_get_child(tree, 0)) > AST_count_nodes(AST_get_child(tree,2))){
					reg1 = CG_code(cg, AST_get_child(tree,0));
					if(AST_has_child_with_type(AST_get_child(tree,2), AST_FUNCALL)){
						fprintf(cg->output_file, "        sub     rsp, %ld\n", cg->curr_size);
						fprintf(cg->output_file, "        mov     %s [rsp], %s\n", CG_size_to_extension(cg->curr_size), CG_register_to_str(reg1, cg->curr_size));
						rstack_push(cg->stack,reg1);
						reg2 = CG_code(cg, AST_get_child(tree,2));
						reg1 = rstack_pop(cg->stack);
						fprintf(cg->output_file, "        mov     %s, %s [rsp]\n", CG_register_to_str(reg1, cg->curr_size), CG_size_to_extension(cg->curr_size));
						fprintf(cg->output_file, "        add     rsp, %ld\n", cg->curr_size);
					}
					else reg2 = CG_code(cg, AST_get_child(tree,2));
				}
				else{
					reg2 = CG_code(cg, AST_get_child(tree,2));
					if(AST_has_child_with_type(AST_get_child(tree,0), AST_FUNCALL)){
						fprintf(cg->output_file, "        sub     rsp, %ld\n", cg->curr_size);
						fprintf(cg->output_file, "        mov     %s [rsp], %s\n", CG_size_to_extension(cg->curr_size), CG_register_to_str(reg2, cg->curr_size));
						rstack_push(cg->stack,reg2);
						reg1 = CG_code(cg, AST_get_child(tree,0));
						reg2 = rstack_pop(cg->stack);
						fprintf(cg->output_file, "        mov     %s, %s [rsp]\n", CG_register_to_str(reg2, cg->curr_size), CG_size_to_extension(cg->curr_size));
						fprintf(cg->output_file, "        add     rsp, %ld\n", cg->curr_size);
					}
					else reg1 = CG_code(cg, AST_get_child(tree,0));
				}

				if(AST_get_child(tree, 1)->type == AST_STAR){
					if(cg->curr_size != 1)
						fprintf(cg->output_file, "        imul    %s, %s\n", CG_register_to_str(reg1, cg->curr_size), CG_register_to_str(reg2, cg->curr_size));
					else{
						fprintf(cg->output_file, "        mov     %s, %s\n", CG_register_to_str(R_RAX, cg->curr_size), CG_register_to_str(reg1, cg->curr_size));
						fprintf(cg->output_file, "        imul    %s\n", CG_register_to_str(reg2, cg->curr_size));
						fprintf(cg->output_file, "        mov     %s, %s\n", CG_register_to_str(reg1, cg->curr_size), CG_register_to_str(R_RAX, cg->curr_size));
					}
				}
				else if(AST_get_child(tree, 1)->type == AST_SLASH){
					fprintf(cg->output_file, "        xor     rdx, rdx\n");
					fprintf(cg->output_file, "        mov     %s, %s\n", CG_register_to_str(R_RAX, cg->curr_size), CG_register_to_str(reg1, cg->curr_size));
					fprintf(cg->output_file, "        idiv    %s\n", CG_register_to_str(reg2, cg->curr_size));
					fprintf(cg->output_file, "        mov     %s, %s\n", CG_register_to_str(reg1, cg->curr_size), CG_register_to_str(R_RAX, cg->curr_size));
				}
				else{
					fprintf(cg->output_file, "        xor     rdx, rdx\n");
					fprintf(cg->output_file, "        mov     %s, %s\n", CG_register_to_str(R_RAX, cg->curr_size), CG_register_to_str(reg1, cg->curr_size));
					fprintf(cg->output_file, "        idiv    %s\n", CG_register_to_str(reg2, cg->curr_size));
					if(cg->curr_size == 1){
						fprintf(cg->output_file, "        mov     al, ah\n");
						fprintf(cg->output_file, "        mov     %s, al\n", CG_register_to_str(reg1, 1));
					}
					else fprintf(cg->output_file, "        mov     %s, %s\n", CG_register_to_str(reg1, cg->curr_size), CG_register_to_str(R_RDX, cg->curr_size));
				}
				rstack_push(cg->stack, reg2);

				return reg1;
			}

		case AST_U:
			{
				AST_T* first = AST_get_child(tree, 0);
				if(first->type == AST_P) return CG_code(cg, AST_get_child(tree,0));
				switch(first->type){
					case AST_MINUS:
						{
							int reg = CG_code(cg, AST_get_child(tree,1));
							fprintf(cg->output_file, "        neg     %s\n", CG_register_to_str(reg, cg->curr_size));
							return reg;
						}
					case AST_RIGHT_ARR:
						{
							int t = cg->is_value_needed;
							cg->is_value_needed = 0;
							int reg = CG_code(cg, AST_get_child(tree, 1));
							cg->is_value_needed = t;
							return reg;
						}
					case AST_LEFT_ARR:
						{
							int t = cg->is_value_needed;
							int t2 = cg->curr_size;
							cg->is_value_needed = 1;
							cg->curr_size = 8;
							int reg = CG_code(cg, AST_get_child(tree, 1));
							cg->is_value_needed = t;
							cg->curr_size = t2;
							if(cg->is_value_needed)	fprintf(cg->output_file, "        mov     %s, %s [%s]\n", CG_register_to_str(reg, cg->curr_size), CG_size_to_extension(cg->curr_size), CG_register_to_str(reg, 8));
							return reg;
						}
				}

			}	

		case AST_POST:
			{
				if(tree->children->size == 1) return CG_code(cg, AST_get_child(tree,0));
				var_type_T *t = TC_get_type_P(cg, AST_get_child(tree,0));
				size_t size = var_type_size(t);
				cg->is_value_needed = 1;
				cg->curr_size = size;
				int reg = CG_code(cg, AST_get_child(tree,0));

				for(int i = 1; i < tree->children->size; i++){
					AST_T* curr = AST_get_child(tree, i);
					if(curr->type == AST_LEFT_B){
						AST_T* BE = AST_get_child(tree,i+1);
						int has_funcall = AST_has_child_with_type(BE, AST_FUNCALL);
						if(has_funcall){
							fprintf(cg->output_file, "        sub     rsp, %ld\n", cg->curr_size);
							fprintf(cg->output_file, "        mov     %s [rsp], %s\n", CG_size_to_extension(cg->curr_size), CG_register_to_str(reg, cg->curr_size));
							rstack_push(cg->stack, reg);
						}
						var_type_T *t1 = TC_get_type_BE(cg, BE);
						cg->curr_size = var_type_size(t1);
						cg->is_value_needed = 1;
						int reg1 = CG_code(cg, BE);
						if(has_funcall){
							reg = rstack_pop(cg->stack);
							fprintf(cg->output_file, "        mov     %s, %s [rsp]\n", CG_register_to_str(reg, cg->curr_size), CG_size_to_extension(cg->curr_size));
							fprintf(cg->output_file, "        add     rsp, %ld\n", cg->curr_size);
						}
						t->ptr_depth--;
						size_t size = var_type_size(t);
						if(cg->curr_size != 1)
							fprintf(cg->output_file, "        imul    %s, %ld\n", CG_register_to_str(reg1, cg->curr_size), size);
						else{
							fprintf(cg->output_file, "        mov     %s, %s\n", CG_register_to_str(R_RAX, cg->curr_size), CG_register_to_str(reg1, cg->curr_size));
							fprintf(cg->output_file, "        imul    %ld\n", size);
							fprintf(cg->output_file, "        mov     %s, %s\n", CG_register_to_str(reg1, cg->curr_size), CG_register_to_str(R_RAX, cg->curr_size));
						}
						fprintf(cg->output_file, "        add     %s, %s\n", CG_register_to_str(reg, 8), CG_register_to_str(reg1, 8));
						
						i += 2;
					}
				}

			}

		case AST_P:
			{
				if(AST_get_child(tree, 0)->type == AST_NUM || AST_get_child(tree,0)->type == AST_WORD || AST_get_child(tree, 0)->type == AST_STR || AST_get_child(tree,0)->type == AST_FUNCALL) return CG_code(cg, AST_get_child(tree,0));
				if(AST_get_child(tree, 0)->type == AST_LEFT_P) return CG_code(cg, AST_get_child(tree,1));

				int reg = rstack_pop(cg->stack);
				if(AST_get_child(tree, 0)->type == AST_TRUE) fprintf(cg->output_file, "        mov     %s, 1\n", CG_register_to_str(reg, 1));
				else if(AST_get_child(tree, 0)->type == AST_FALSE) fprintf(cg->output_file, "        mov     %s, 0\n", CG_register_to_str(reg, 1));
				return reg;
			}

		case AST_FUNCALL:
			{
				size_t curr_size = cg->curr_size;
				char* name = AST_get_arg(AST_get_child(tree,0));
				fun_frame_T* fframe = CG_find_function(cg, name);
				size_t args_count = 0;
				for(int i = 2; i <tree->children->size -1; i++){
					AST_T* curr = AST_get_child(tree, i);
					if(curr->type != AST_BE) continue;
					var_type_T* arg_type = ((symbol_T*) list_get(fframe->parameters, args_count))->type;
					var_type_T* BE_type = TC_get_type_BE(cg, curr);
					if(!var_type_cmp(arg_type, BE_type));
					if(AST_has_child_with_type(curr, AST_FUNCALL)){
						for(int i = 0; i < args_count; i++){
							var_type_T* t = ((symbol_T*) list_get(fframe->parameters, i))->type;
							size_t size = var_type_size(t);
							fprintf(cg->output_file, "        sub     rsp, %ld\n", size);
							fprintf(cg->output_file, "        mov     %s [rsp], %s\n", CG_size_to_extension(size), CG_register_to_str(par_num_to_reg(i), size));
						}
					}
					cg->is_value_needed = 1;
					size_t size = var_type_size(arg_type);
					cg->curr_size = size;
					int reg = CG_code(cg, curr);
					rstack_push(cg->stack, reg);
					fprintf(cg->output_file, "        mov     %s, %s\n", CG_register_to_str(par_num_to_reg(args_count), size), CG_register_to_str(reg, size));
					if(AST_has_child_with_type(curr, AST_FUNCALL)){
						for(int i = 0; i < args_count; i++){
							var_type_T* t = ((symbol_T*) list_get(fframe->parameters, i))->type;
							size_t size = var_type_size(t);
							fprintf(cg->output_file, "        mov     %s, %s [rsp]\n",  CG_register_to_str(par_num_to_reg(i), size), CG_size_to_extension(size));
							fprintf(cg->output_file, "        add     rsp, %ld\n",size);
						}
					}
					free(BE_type);
					args_count++;
				}
				cg->curr_size = curr_size;
				fprintf(cg->output_file, "        call    %s\n", name); 
				int reg = rstack_pop(cg->stack);
				fprintf(cg->output_file, "        mov     %s, %s\n", CG_register_to_str(reg, cg->curr_size), CG_register_to_str(R_RAX, cg->curr_size));
				return reg;
			}

		case AST_NUM:
			{
				int reg = rstack_pop(cg->stack);
				char* s = AST_get_arg(tree); 
				fprintf(cg->output_file, "        mov     %s, %s\n", CG_register_to_str(reg, cg->curr_size), s);
				return reg;
			}

		case AST_STR:
			{
				int reg = rstack_pop(cg->stack);
				char* s = AST_get_arg(tree); 
				size_t id = 0; 
				for(int i = 0; i < cg->str_lits->size; i++){
					str_lit_T* curr = (str_lit_T*)list_get(cg->str_lits, i);
					if(!strcmp(curr->str, s)){
						id = curr->id;
						break;	
					}
				}
				if(!id){
					id = cg->str_lits->size;
					list_add(cg->str_lits, str_lit_init(s,id, strlen(s)));
				}
				fprintf(cg->output_file, "        lea     %s, [rel sl%ld]\n", CG_register_to_str(reg, 8), id);
				return reg;
			}

		case AST_WORD:
			{
				int reg = rstack_pop(cg->stack);
				symbol_T* sym = symtab_look_up(cg->table,AST_get_arg(tree));
				size_t size = var_type_size(sym->type);
				if(cg->is_value_needed){
					if(sym->scope == SCOPE_GLOBAL){ 
						if(size == cg->curr_size)
							fprintf(cg->output_file, "        mov     %s, %s [rel %s]\n", CG_register_to_str(reg, size), CG_size_to_extension(size), sym->name);
						else
							fprintf(cg->output_file, "        movsx   %s, %s [rel %s]\n", CG_register_to_str(reg, cg->curr_size), CG_size_to_extension(size), sym->name);
					}
					else{
						if(var_type_size(sym->type) == cg->curr_size)
							fprintf(cg->output_file, "        mov     %s, %s [rbp-%ld]\n", CG_register_to_str(reg, size), CG_size_to_extension(size), sym->offset);
						else
							fprintf(cg->output_file, "        movsx   %s, %s [rbp-%ld]\n", CG_register_to_str(reg, cg->curr_size), CG_size_to_extension(size), sym->offset);
					}
				}
				else{
					if(sym->scope == SCOPE_GLOBAL) 
						fprintf(cg->output_file, "        lea     %s, [rel %s]\n", CG_register_to_str(reg, 8), sym->name);
					else
						fprintf(cg->output_file, "        lea     %s, [rbp-%ld]\n", CG_register_to_str(reg, 8), sym->offset);
				}
				return reg;
			}
			
	}
}
