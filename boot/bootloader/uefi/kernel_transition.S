    .text
    .globl uefi_enter_kernel32
    .globl uefi_enter_kernel64
    .globl uefi_runtime_bridge_entry32
    .globl uefi_runtime_bridge_entry64
    .globl uefi_runtime_bridge_return32
    .def uefi_enter_kernel32; .scl 2; .type 32; .endef
    .def uefi_enter_kernel64; .scl 2; .type 32; .endef
/* Microsoft x64 ABI: RCX=entry, RDX=BIB, R8D=stack top. This trampoline is intentionally
   non-returning and is used only after successful ExitBootServices(). */
uefi_enter_kernel32:
    cli
    movb $'T', %al
    outb %al, $0xe9
    movl %ecx, %edi
    movl %edx, %ebx
    movl %r8d, %esi
    lgdt uefi_transition_gdtr(%rip)
    pushq $0x08
    leaq uefi_transition_compat(%rip), %rax
    pushq %rax
    lretq

    .code32
uefi_transition_compat:
    movb $'C', %al
    outb %al, $0xe9
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    movw %ax, %fs
    movw %ax, %gs
    movl %cr0, %eax
    andl $0x7fffffff, %eax
    movl %eax, %cr0
    movb $'P', %al
    outb %al, $0xe9
    movl $0xc0000080, %ecx
    rdmsr
    andl $0xfffffeff, %eax
    wrmsr
    movl %cr4, %eax
    andl $0xffffffdf, %eax
    movl %eax, %cr4
    movl %esi, %esp
    xorl %ebp, %ebp
    movl $0x4e4f5641, %eax
    movb $'J', %al
    outb %al, $0xe9
    movl $0x4e4f5641, %eax
    jmp *%edi

    .code64
/* Microsoft x64 ABI: RCX=entry, EDX=BIB, R8D=stack top. UEFI x64 already
   runs in long mode; after ExitBootServices the loader only establishes the
   kernel register contract and transfers control without returning. */
uefi_enter_kernel64:
    cli
    movq %rcx, %r10
    movl %edx, %ebx
    movl %r8d, %esp
    xorl %ebp, %ebp
    movl $0x4e4f5641, %eax
    jmp *%r10

    .code32
/* IA32 kernel provider entry: EDI=context, ESI=64-byte health evidence.
   The bridge restores the firmware's x64 page-table context only for the
   bounded SetVariable call and returns to paging-off IA32 afterwards. */
uefi_runtime_bridge_entry32:
    pushfl
    cli
    pushal
    movl %esp, 56(%edi)
    movl %edi, %ebx
    leal 64(%edi), %edi
    movl $16, %ecx
    cld
    rep movsl
    movl %ebx, %edi
    lgdt 160(%edi)
    movl 16(%edi), %eax
    movl %eax, %cr3
    movl %cr4, %eax
    orl $0x20, %eax
    movl %eax, %cr4
    movl $0xc0000080, %ecx
    rdmsr
    orl $0x100, %eax
    wrmsr
    movl %cr0, %eax
    orl $0x80000000, %eax
    movl %eax, %cr0
    ljmp *166(%edi)

    .code64
uefi_runtime_bridge_entry64:
    movl %edi, %r12d
    movq 56(%r12), %rsp
    andq $-16, %rsp
    subq $48, %rsp
    movq 32(%r12), %rcx
    movq 40(%r12), %rdx
    movl $7, %r8d
    movl $64, %r9d
    leaq 64(%r12), %rax
    movq %rax, 32(%rsp)
    movq 24(%r12), %rax
    call *%rax
    movq %rax, 48(%r12)
    pushq $0x18
    leaq uefi_runtime_bridge_return32(%rip), %rax
    pushq %rax
    lretq

    .code32
uefi_runtime_bridge_return32:
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
    movl 56(%edi), %esp
    popal
    movl 48(%edi), %eax
    orl 52(%edi), %eax
    sete %al
    movzbl %al, %eax
    popfl
    ret

    .p2align 3
uefi_transition_gdt:
    .quad 0
    .quad 0x00cf9a000000ffff
    .quad 0x00cf92000000ffff
uefi_transition_gdtr:
    .word (uefi_transition_gdtr-uefi_transition_gdt)-1
    .quad uefi_transition_gdt
