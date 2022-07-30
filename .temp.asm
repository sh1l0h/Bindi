section .bss
section .text
        global _start

_start:
        pop     rdi
        mov     rsi, rsp
        call    main
        movsx   rdi, eax
        mov     rax, 60
        syscall

main:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 8
        mov     QWORD [rbp-8], rdi
        sub     rsp, 8
        mov     QWORD [rbp-16], rsi
        sub     rsp, 4
        mov     ebx, 5
        mov     DWORD [rbp-20], ebx
        sub     rsp, 8
        lea     rbx, [rbp-20]
        mov     QWORD [rbp-28], rbx
        mov     rbx, QWORD [rbp-28]
        mov     ebx, DWORD [rbx]
        mov     edi, ebx
        call    print_int
.end:
        leave
        ret
print_char:
        dec     rsp
        mov     BYTE [rsp], dil
        mov     rax, 1
        mov     rdi, 1
        mov     rdx, 1
        mov     rsi, rsp
        syscall
        inc     rsp
        ret
print_int:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 64
        mov     DWORD  [rbp-52], edi
        mov     BYTE  [rbp-17], 0
        mov     BYTE  [rbp-18], 10
        mov     DWORD  [rbp-4], 0
        cmp     DWORD  [rbp-52], 0
        jns     .L2
        mov     DWORD  [rbp-4], 1
        neg     DWORD  [rbp-52]
.L2:
        mov     DWORD  [rbp-8], 0
        cmp     DWORD  [rbp-52], 0
        jne     .L5
        add     DWORD  [rbp-8], 1
        mov     eax, 30
        sub     eax, DWORD  [rbp-8]
        cdqe
        mov     BYTE  [rbp-48+rax], 48
        jmp     .L4
.L6:
        mov     ecx, DWORD  [rbp-52]
        movsx   rax, ecx
        imul    rax, rax, 1717986919
        shr     rax, 32
        mov     edx, eax
        sar     edx, 2
        mov     eax, ecx
        sar     eax, 31
        sub     edx, eax
        mov     eax, edx
        sal     eax, 2
        add     eax, edx
        add     eax, eax
        sub     ecx, eax
        mov     edx, ecx
        mov     eax, edx
        lea     edx, [rax+48]
        add     DWORD  [rbp-8], 1
        mov     eax, 30
        sub     eax, DWORD  [rbp-8]
        cdqe
        mov     BYTE  [rbp-48+rax], dl
        mov     eax, DWORD  [rbp-52]
        movsx   rdx, eax
        imul    rdx, rdx, 1717986919
        shr     rdx, 32
        mov     ecx, edx
        sar     ecx, 2
        cdq
        mov     eax, ecx
        sub     eax, edx
        mov     DWORD  [rbp-52], eax
.L5:
        cmp     DWORD  [rbp-52], 0
        jg      .L6
        cmp     DWORD  [rbp-4], 0
        je      .L4
        add     DWORD  [rbp-8], 1
        mov     eax, 30
        sub     eax, DWORD  [rbp-8]
        cdqe
        mov     BYTE  [rbp-48+rax], 45
.L4:
        mov     eax, DWORD  [rbp-8]
        lea     edx, [rax+2]
        mov     eax, 30
        sub     eax, DWORD  [rbp-8]
        lea     rcx, [rbp-48]
        cdqe
        add     rax, rcx
        mov     rsi, rax
        mov     edi, 1
        mov     eax, 0
        mov     rax, 1
        syscall
        nop
        leave
        ret

print_bool:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 32
        mov     eax, edi
        mov     BYTE  [rbp-20], al
        mov     DWORD  [rbp-6], 1702195828
        mov     WORD  [rbp-2], 10
        mov     DWORD  [rbp-13], 1936482662
        mov     DWORD  [rbp-10], 681331
        cmp     BYTE  [rbp-20], 0
        jne     .L8
        lea     rax, [rbp-13]
        mov     edx, 6
        mov     rsi, rax
        mov     edi, 1
        mov     rax, 1
        syscall
        jmp     .L10
.L8:
        lea     rax, [rbp-6]
        mov     edx, 5
        mov     rsi, rax
        mov     edi, 1
        mov     rax, 1
        syscall
.L10:
        nop
        leave
        ret
print_byte:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 64
        mov     eax, edi
        mov     BYTE  [rbp-52], al
        mov     BYTE  [rbp-17], 0
        mov     BYTE  [rbp-18], 10
        mov     BYTE  [rbp-1], 0
        cmp     BYTE  [rbp-52], 0
        jns     .L11
        mov     BYTE  [rbp-1], 1
        movzx   eax, BYTE  [rbp-52]
        neg     eax
        mov     BYTE  [rbp-52], al
.L11:
        mov     BYTE  [rbp-2], 0
        cmp     BYTE  [rbp-52], 0
        jne     .L13
        movzx   eax, BYTE  [rbp-2]
        add     eax, 1
        mov     BYTE  [rbp-2], al
        movsx   eax, BYTE  [rbp-2]
        mov     edx, 30
        sub     edx, eax
        movsx   rax, edx
        mov     BYTE  [rbp-48+rax], 48
        jmp     .L14
.L12:
        movzx   edx, BYTE  [rbp-52]
        mov     ecx, 103
        mov     eax, ecx
        imul    dl
        shr     ax, 8
        sar     al, 2
        mov     esi, edx
        sar     sil, 7
        sub     eax, esi
        mov     ecx, eax
        mov     eax, ecx
        sal     eax, 2
        add     eax, ecx
        add     eax, eax
        mov     ecx, edx
        sub     ecx, eax
        mov     eax, ecx
        lea     ecx, [rax+48]
        movzx   eax, BYTE  [rbp-2]
        add     eax, 1
        mov     BYTE  [rbp-2], al
        movsx   eax, BYTE  [rbp-2]
        mov     edx, 30
        sub     edx, eax
        movsx   rax, edx
        mov     BYTE  [rbp-48+rax], cl
        movzx   edx, BYTE  [rbp-52]
        mov     ecx, 103
        mov     eax, ecx
        imul    dl
        shr     ax, 8
        sar     al, 2
        sar     dl, 7
        sub     eax, edx
        mov     BYTE  [rbp-52], al
.L13:
        cmp     BYTE  [rbp-52], 0
        jg      .L12
        cmp     BYTE  [rbp-1], 0
        je      .L14
        movzx   eax, BYTE  [rbp-2]
        add     eax, 1
        mov     BYTE  [rbp-2], al
        movsx   eax, BYTE  [rbp-2]
        mov     edx, 30
        sub     edx, eax
        movsx   rax, edx
        mov     BYTE  [rbp-48+rax], 45
.L14:
        movsx   eax, BYTE  [rbp-2]
        lea     edx, [rax+2]
        movsx   eax, BYTE  [rbp-2]
        mov     ecx, 30
        mov     esi, ecx
        sub     esi, eax
        lea     rcx, [rbp-48]
        movsx   rax, esi
        add     rax, rcx
        mov     rsi, rax
        mov     edi, 1
        mov     rax, 1
        syscall
        nop
        leave
        ret
print_long:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 64
        mov     QWORD  [rbp-56], rdi
        mov     BYTE  [rbp-17], 0
        mov     BYTE  [rbp-18], 10
        mov     BYTE  [rbp-1], 0
        cmp     QWORD  [rbp-56], 0
        jns     .L15
        mov     BYTE  [rbp-1], 1
        neg     QWORD  [rbp-56]
.L15:
        mov     DWORD  [rbp-8], 0
        cmp     QWORD  [rbp-56], 0
        jne     .L17
        add     DWORD  [rbp-8], 1
        mov     eax, 30
        sub     eax, DWORD  [rbp-8]
        cdqe
        mov     BYTE  [rbp-48+rax], 48
        jmp     .L18
.L16:
        mov     rcx, QWORD  [rbp-56]
        mov     rdx, 7378697629483820647
        mov     rax, rcx
        imul    rdx
        sar     rdx, 2
        mov     rax, rcx
        sar     rax, 63
        sub     rdx, rax
        mov     rax, rdx
        sal     rax, 2
        add     rax, rdx
        add     rax, rax
        sub     rcx, rax
        mov     rdx, rcx
        mov     eax, edx
        lea     edx, [rax+48]
        add     DWORD  [rbp-8], 1
        mov     eax, 30
        sub     eax, DWORD  [rbp-8]
        cdqe
        mov     BYTE  [rbp-48+rax], dl
        mov     rcx, QWORD  [rbp-56]
        mov     rdx, 7378697629483820647
        mov     rax, rcx
        imul    rdx
        mov     rax, rdx
        sar     rax, 2
        sar     rcx, 63
        mov     rdx, rcx
        sub     rax, rdx
        mov     QWORD  [rbp-56], rax
.L17:
        cmp     QWORD  [rbp-56], 0
        jg      .L16
        cmp     BYTE  [rbp-1], 0
        je      .L18
        add     DWORD  [rbp-8], 1
        mov     eax, 30
        sub     eax, DWORD  [rbp-8]
        cdqe
        mov     BYTE  [rbp-48+rax], 45
.L18:
        mov     eax, DWORD  [rbp-8]
        lea     edx, [rax+2]
        mov     eax, 30
        sub     eax, DWORD  [rbp-8]
        lea     rcx, [rbp-48]
        cdqe
        add     rax, rcx
        mov     rsi, rax
        mov     edi, 1
        mov     rax, 1
        syscall
        nop
        leave
        ret
print_string:        mov    rax, 1
        mov     rsi, rdi
        mov     DWORD edx, [rdi-4]
        mov     rdi, 1
        syscall
        ret
section .data
