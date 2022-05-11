#include <stdlib.h>
#include "./include/cg.h"
#include "./include/token.h"

int is_value_needed = 0;
int lable_count = 0;
size_t curr_size = 0;

CG_T* CG_init(char* file_name)
{
	CG_T* result = (CG_T*)malloc(sizeof(CG_T));
	result->output_file = fopen(file_name, "w");
	return result;
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

const char* CG_size_register(size_t size, size_t reg)
{
	if(reg == 0){
		switch(size){
			case 1: return "al";
			case 2: return "ax";
			case 4: return "eax";
			default: return "rax";
		}
	}
	else{
		switch(size){
			case 1: return "bl";
			case 2: return "bx";
			case 4: return "ebx";
			default: return "rbx";
		}
	}
}

void CG_generate_code(CG_T* cg, AST_T* tree, symtab_T* table)
{
	switch(tree->type){
		case AST_PROG:
			symtab_enter_scope(table);
			fprintf(cg->output_file, "section .data\n");
			fprintf(cg->output_file, "    true: db \"true\", 10\n");
			fprintf(cg->output_file, "    false: db \"false\", 10\n");
			fprintf(cg->output_file, "section .bss\n");
			for(int i = 0; i < tree->children->size; i++){
				AST_T* curr = AST_get_child(tree, i);
				if(curr->type != AST_VADEC) continue;
				CG_generate_code(cg, curr, table);
				symtab_add_symbol(table, symbol_init(((token_T*)*(AST_get_child(curr,1)->children->list))->arg->_str, SYM_INT, SCOPE_GLOBAL, 4, 0));
			}
			fprintf(cg->output_file, "section .text\n");
			fprintf(cg->output_file, "    global _start\n\n");
			fprintf(cg->output_file, "_start:\n");
			for(int i = 0; i < tree->children->size; i++){
				AST_T* curr = AST_get_child(tree, i);
				if(curr->type == AST_VADEC) continue;
				CG_generate_code(cg, curr, table);
			}
			fprintf(cg->output_file, "    mov    rax, 60\n");
			fprintf(cg->output_file, "    mov    rdi, 0\n");
			fprintf(cg->output_file, "    syscall\n");

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
			fprintf(cg->output_file, "        ret\n");
			//symtab_exit_scope(table);
			break;

		case AST_VADEC:
			fprintf(cg->output_file, "    %s: resd 1\n", ((token_T*)*(AST_get_child(tree,1)->children->list))->arg->_str);
			break;

		case AST_BLOCK:
			symtab_enter_scope(table);
			for(int i = 1; i < tree->children->size -1; i++) CG_generate_code(cg, AST_get_child(tree, i), table);
			symtab_exit_scope(table);
			break;

		case AST_ST:
			if(AST_get_child(tree, 0)->type == AST_PRINT){
				CG_generate_code(cg, AST_get_child(tree,1), table);
				if(AST_get_child(tree,1)->type == AST_BE){
					fprintf(cg->output_file, "    cmp    al, 0\n");
					fprintf(cg->output_file, "    je     .l%d\n", lable_count);
					fprintf(cg->output_file, "    mov    rax, 1\n");
					fprintf(cg->output_file, "    mov    rdi, 1\n");
					fprintf(cg->output_file, "    mov    rsi, true\n");
					fprintf(cg->output_file, "    mov    rdx, 5\n");
					fprintf(cg->output_file, "    jmp    .l%d\n", ++lable_count);
					fprintf(cg->output_file, ".l%d:\n", lable_count - 1);
					fprintf(cg->output_file, "    mov    rax, 1\n");
					fprintf(cg->output_file, "    mov    rdi, 1\n");
					fprintf(cg->output_file, "    mov    rsi, false\n");
					fprintf(cg->output_file, "    mov    rdx, 6\n");
					fprintf(cg->output_file, ".l%d:\n",lable_count++);
					fprintf(cg->output_file, "    syscall\n");
				}
				else{
					fprintf(cg->output_file, "    mov    rdi, rax\n");
					fprintf(cg->output_file, "    call   print_int\n");

				}
			}
			else if(AST_get_child(tree, 0)->type == AST_IF){
				CG_generate_code(cg, AST_get_child(tree,1), table);
				fprintf(cg->output_file, "    cmp    al, 0\n");
				fprintf(cg->output_file, "    je     .l%d\n",lable_count);
				CG_generate_code(cg, AST_get_child(tree, 2), table);
				if(tree->children->size > 3) 
					fprintf(cg->output_file, "    jmp     .l%d\n",lable_count+1);
				fprintf(cg->output_file, ".l%d:\n",lable_count++);
				if(tree->children->size > 3){
					CG_generate_code(cg, AST_get_child(tree, 4), table);
					fprintf(cg->output_file, ".l%d:\n",lable_count++);
				}
			}
			else if(AST_get_child(tree, 0)->type == AST_WHILE){
				fprintf(cg->output_file, ".l%d:\n",lable_count);
				CG_generate_code(cg, AST_get_child(tree,1), table);
				fprintf(cg->output_file, "    cmp    al, 0\n");
				fprintf(cg->output_file, "    je     .l%d\n",++lable_count);
				CG_generate_code(cg, AST_get_child(tree,2), table);
				fprintf(cg->output_file, "    jmp    .l%d\n",lable_count-1);
				fprintf(cg->output_file, ".l%d:\n",lable_count++);
			}
			else if(AST_get_child(tree, 0)->type == AST_BLOCK){
				CG_generate_code(cg, AST_get_child(tree,0), table);
			}
			else{
				curr_size = symtab_look_up(table, AST_get_name(AST_get_child(tree, 0)))->size;
				CG_generate_code(cg, AST_get_child(tree,2), table);
				curr_size = 0;
				fprintf(cg->output_file, "    push   rax\n");
				is_value_needed = 0;
				CG_generate_code(cg, AST_get_child(tree,0), table);
				fprintf(cg->output_file, "    pop    rbx\n");
				fprintf(cg->output_file, "    mov    %s [rax], %s\n", CG_size_to_extension(curr_size), CG_size_register(curr_size, 1));
			}
			break;

		case AST_BE: 
			CG_generate_code(cg, AST_get_child(tree,0), table);
			if(tree->children->size == 1) break;
			break;

		case AST_BT: 
			CG_generate_code(cg, AST_get_child(tree,0), table);
			if(tree->children->size == 1) break;
			break;

		case AST_BU:
			if(tree->children->size == 1) CG_generate_code(cg, AST_get_child(tree,0), table);
			else{
				CG_generate_code(cg, AST_get_child(tree,1), table);
				fprintf(cg->output_file, "    cmp    al, 0\n");
				fprintf(cg->output_file, "    sete   al\n");
			}
			break;

		case AST_BP:
			if(AST_get_child(tree, 0)->type == AST_E){
				CG_generate_code(cg, AST_get_child(tree,0),table);
				fprintf(cg->output_file, "    push   rax\n");
				CG_generate_code(cg, AST_get_child(tree,2), table);
				fprintf(cg->output_file, "    pop    rbx\n");
				fprintf(cg->output_file, "    cmp    rbx, rax\n");
				if(AST_get_child(tree, 1)->type == AST_GT){
					fprintf(cg->output_file, "    setg   al\n");
				}
				else if (AST_get_child(tree, 1)->type == AST_LT){
					fprintf(cg->output_file, "    setl   al\n");
				}
			}
			break;

		case AST_E:
			CG_generate_code(cg, AST_get_child(tree,0), table);
			if(tree->children->size == 1) break;
			fprintf(cg->output_file, "    push    rax\n");
			CG_generate_code(cg, AST_get_child(tree,2), table);
			fprintf(cg->output_file, "    pop    rbx\n");
			if(AST_get_child(tree, 1)->type == AST_PLUS){
				fprintf(cg->output_file, "    add    rax, rbx\n");
			}
			else{
				fprintf(cg->output_file, "    sub    rbx, rax\n");
				fprintf(cg->output_file, "    mov    rax, rbx\n");
			}
			curr_size = 0;
			break;

		case AST_TERM:
			CG_generate_code(cg, AST_get_child(tree,0), table);
			if(tree->children->size == 1) break;
			fprintf(cg->output_file, "    push    rax\n");
			CG_generate_code(cg, AST_get_child(tree,2), table);
			if(AST_get_child(tree, 1)->type == AST_STAR){
				fprintf(cg->output_file, "    pop    rbx\n");
				fprintf(cg->output_file, "    imul   rbx\n");
			}
			else{
				fprintf(cg->output_file, "    mov    rbx, rax\n");
				fprintf(cg->output_file, "    pop    rax\n");
				fprintf(cg->output_file, "    imul   rbx\n");
			}
			break;

		case AST_U:
			if(tree->children->size == 1) CG_generate_code(cg, AST_get_child(tree,0), table);
			else if(AST_get_child(tree, 0)->type == AST_MINUS){
				CG_generate_code(cg, AST_get_child(tree, 1), table);
				fprintf(cg->output_file, "    neg    %s\n", CG_size_register(curr_size, 0));
			}
			break;

		case AST_P:
			if(tree->children->size == 1){
				is_value_needed = 1;
				CG_generate_code(cg, AST_get_child(tree,0), table);
			}
			else CG_generate_code(cg, AST_get_child(tree,1), table);
			break;

		case AST_NUM:
			if(curr_size == 0) curr_size = 4;
			fprintf(cg->output_file, "    mov    %s, %s\n", CG_size_register(curr_size, 0), AST_get_name(tree));
			break;

		case AST_WORD:
			{
				char* word = AST_get_name(tree);
				symbol_T* sym = symtab_look_up(table, word);
				if(curr_size == 0) curr_size = sym->size;
				if(sym){
					if(is_value_needed){
						if(sym->scope == SCOPE_GLOBAL) fprintf(cg->output_file, "    mov    %s, %s [rel %s]\n",CG_size_register(sym->size, 0), CG_size_to_extension(sym->size), word);
					}
					else{
						if(sym->scope == SCOPE_GLOBAL) fprintf(cg->output_file, "    lea    rax, [rel %s]\n", word);
					}
				}
			}
			break;


	}
}

void CG_free(CG_T* cg)
{
	fclose(cg->output_file);
	free(cg);
}
