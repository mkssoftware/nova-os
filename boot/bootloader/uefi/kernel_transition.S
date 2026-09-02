    .text
    .globl uefi_enter_kernel32
    .def uefi_enter_kernel32; .scl 2; .type 32; .endef
/* Microsoft x64 ABI: RCX=entry, RDX=BIB. This trampoline is intentionally
   non-returning and is used only after successful ExitBootServices(). */
uefi_enter_kernel32:
    cli
    movl %ecx, %edi
    movl %edx, %ebx
    lgdt uefi_transition_gdtr(%rip)
    pushq $0x08
    leaq uefi_transition_compat(%rip), %rax
    pushq %rax
    lretq

    .code32
uefi_transition_compat:
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    movl %cr0, %eax
    andl $0x7fffffff, %eax
    movl %eax, %cr0
    movl $0xc0000080, %ecx
    rdmsr
    andl $0xfffffeff, %eax
    wrmsr
    movl %cr4, %eax
    andl $0xffffffdf, %eax
    movl %eax, %cr4
    movl $0x00090000, %esp
    xorl %ebp, %ebp
    movl $0x4e4f5641, %eax
    jmp *%edi

    .code64
    .p2align 3
uefi_transition_gdt:
    .quad 0
    .quad 0x00cf9a000000ffff
    .quad 0x00cf92000000ffff
uefi_transition_gdtr:
    .word (uefi_transition_gdtr-uefi_transition_gdt)-1
    .quad uefi_transition_gdt
