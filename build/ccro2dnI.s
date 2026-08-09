	.file	"controls.c"
	.text
	.p2align 4
	.def	sync_scrollbars;	.scl	3;	.type	32;	.endef
	.seh_proc	sync_scrollbars
sync_scrollbars:
	.seh_endprologue
	leaq	2+controls(%rip), %rax
	leaq	used(%rip), %rdx
	movq	%rcx, %r8
	leaq	68096(%rax), %rcx
	jmp	.L5
	.p2align 5
	.p2align 4,,10
	.p2align 3
.L2:
	addq	$532, %rax
	addq	$1, %rdx
	cmpq	%rax, %rcx
	je	.L8
.L5:
	cmpb	$0, (%rdx)
	je	.L2
	movzwl	(%r8), %r11d
	cmpw	%r11w, (%rax)
	jne	.L2
	cmpl	$14, 6(%rax)
	jne	.L2
	movl	$0, 82(%rax)
	cmpw	$1, 122(%rax)
	je	.L9
	movl	104(%r8), %r9d
	movl	$0, %r10d
	subl	112(%r8), %r9d
	cmovs	%r10d, %r9d
	movl	%r9d, 86(%rax)
	movl	96(%r8), %r9d
.L4:
	movl	%r9d, 78(%rax)
	movl	112(%r8), %r9d
	addq	$1, %rdx
	movl	%r9d, 110(%rax)
	movl	116(%r8), %r9d
	movl	%r9d, 114(%rax)
	movl	104(%r8), %r9d
	movl	%r9d, 102(%rax)
	movl	108(%r8), %r9d
	orl	$8, 14(%rax)
	addq	$532, %rax
	movl	%r9d, -426(%rax)
	cmpq	%rax, %rcx
	jne	.L5
.L8:
	ret
	.p2align 4,,10
	.p2align 3
.L9:
	movl	108(%r8), %r9d
	movl	$0, %r10d
	subl	116(%r8), %r9d
	cmovs	%r10d, %r9d
	movl	%r9d, 86(%rax)
	movl	100(%r8), %r9d
	jmp	.L4
	.seh_endproc
	.p2align 4
	.def	control_rounded_rect;	.scl	3;	.type	32;	.endef
	.seh_proc	control_rounded_rect
control_rounded_rect:
	pushq	%r15
	.seh_pushreg	%r15
	pushq	%r14
	.seh_pushreg	%r14
	pushq	%r13
	.seh_pushreg	%r13
	pushq	%r12
	.seh_pushreg	%r12
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$88, %rsp
	.seh_stackalloc	88
	.seh_endprologue
	movq	8(%rdx), %r12
	movq	(%rdx), %r14
	movl	%r8d, %edi
	movl	%r8d, %esi
	movq	%r12, %r8
	movq	%rcx, %r13
	sarq	$32, %r8
	testl	%r12d, %r12d
	movl	%r9d, 184(%rsp)
	setle	%al
	testl	%r8d, %r8d
	setle	%dl
	orb	%dl, %al
	jne	.L22
	testw	%di, %di
	je	.L22
	movzwl	%di, %edi
	leal	(%rdi,%rdi), %eax
	cmpl	%r8d, %eax
	jle	.L14
	movl	%r8d, %edi
	sarl	%edi
	movl	%edi, %esi
	leal	(%rdi,%rdi), %eax
.L14:
	movl	%r12d, %edx
	leal	-1(%r8), %ebp
	movq	%r14, 40(%rsp)
	movq	%r8, %rbx
	sarl	%edx
	cmpl	%r12d, %eax
	movq	%r14, %rax
	movabsq	$-4294967296, %r15
	cmovg	%edx, %esi
	cmovg	%edx, %edi
	xorl	%r10d, %r10d
	xorl	%r11d, %r11d
	shrq	$32, %rax
	movq	%r13, 160(%rsp)
	movl	%ebp, %r13d
	movl	%eax, %ecx
	movzwl	%si, %esi
	leaq	64(%rsp), %rax
	movq	%r12, 48(%rsp)
	imull	%esi, %esi
	movq	%rax, 32(%rsp)
	leal	-1(%rdi), %eax
	xorl	%ebp, %ebp
	movl	%eax, 60(%rsp)
	movq	%r10, %rax
	movl	%ecx, %r14d
	andq	%r15, %rax
	movl	%esi, %r12d
	movq	%rax, %rsi
	jmp	.L19
	.p2align 4,,10
	.p2align 3
.L25:
	movl	48(%rsp), %edx
	movl	40(%rsp), %eax
	movl	%r13d, %r8d
	cmpl	%edi, %r13d
	jl	.L16
.L17:
	leal	(%r14,%rbp), %ecx
	orq	%rsi, %rax
	addl	$1, %ebp
	subl	$1, %r13d
	salq	$32, %rcx
	movl	%eax, %eax
	movl	184(%rsp), %r8d
	orq	%rcx, %rax
	movq	160(%rsp), %rcx
	movq	%rax, %r10
	movq	%r11, %rax
	andq	%r15, %rax
	movq	%r10, 64(%rsp)
	orq	%rdx, %rax
	movq	32(%rsp), %rdx
	movl	%eax, %eax
	btsq	$32, %rax
	movq	%rax, 72(%rsp)
	call	nova_surface_rect
	cmpl	%ebp, %ebx
	jle	.L10
.L19:
	cmpl	%edi, %ebp
	jge	.L25
	movl	%ebp, %r8d
.L16:
	movl	60(%rsp), %ecx
	xorl	%eax, %eax
	subl	%r8d, %ecx
	imull	%ecx, %ecx
	.p2align 5
	.p2align 4
	.p2align 3
.L18:
	movl	%eax, %r8d
	addl	$1, %eax
	movl	%eax, %edx
	imull	%eax, %edx
	addl	%ecx, %edx
	cmpl	%r12d, %edx
	jle	.L18
	movl	40(%rsp), %eax
	movl	%edi, %edx
	subl	%r8d, %edx
	leal	(%rdx,%rdx), %ecx
	addl	%edx, %eax
	movl	48(%rsp), %edx
	subl	%ecx, %edx
	jmp	.L17
	.p2align 4,,10
	.p2align 3
.L22:
	movl	184(%rsp), %r8d
	leaq	64(%rsp), %rdx
	movq	%r13, %rcx
	movq	%r14, 64(%rsp)
	movq	%r12, 72(%rsp)
	call	nova_surface_rect
	nop
.L10:
	addq	$88, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%rbp
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	ret
	.seh_endproc
	.p2align 4
	.def	text_equal;	.scl	3;	.type	32;	.endef
	.seh_proc	text_equal
text_equal:
	.seh_endprologue
	xorl	%eax, %eax
	testq	%rcx, %rcx
	je	.L26
	xorl	%eax, %eax
	jmp	.L28
	.p2align 4,,10
	.p2align 3
.L37:
	testb	%r9b, %r9b
	je	.L29
	addq	$1, %rax
	cmpb	%r9b, %r8b
	jne	.L32
.L28:
	movzbl	(%rcx,%rax), %r8d
	movzbl	(%rdx,%rax), %r9d
	testb	%r8b, %r8b
	jne	.L37
.L29:
	cmpb	%r9b, %r8b
	sete	%al
.L26:
	ret
	.p2align 4,,10
	.p2align 3
.L32:
	xorl	%eax, %eax
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_control_set_state
	.def	nova_control_set_state;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_control_set_state
nova_control_set_state:
	.seh_endprologue
	movq	%rcx, %rax
	testq	%rcx, %rcx
	je	.L39
	movzwl	(%rcx), %ecx
	leaq	used(%rip), %r8
	movzbl	(%r8,%rcx), %r8d
	testb	%r8b, %r8b
	je	.L39
	movl	12(%rax), %ecx
	cmpl	$5, %edx
	je	.L77
	cmpl	$4, %ecx
	ja	.L39
	leaq	.L43(%rip), %r9
	movslq	(%r9,%rcx,4), %rcx
	addq	%r9, %rcx
	jmp	*%rcx
	.section .rdata,"dr"
	.align 4
.L43:
	.long	.L46-.L43
	.long	.L44-.L43
	.long	.L45-.L43
	.long	.L44-.L43
	.long	.L42-.L43
	.text
	.p2align 4,,10
	.p2align 3
.L45:
	leal	-3(%rdx), %ecx
	cmpl	$1, %ecx
	setbe	%cl
.L49:
	leal	-2(%rdx), %r9d
	testb	%cl, %cl
	jne	.L48
.L39:
	addl	$1, 8+diagnostics(%rip)
	xorl	%r8d, %r8d
	movl	%r8d, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L44:
	leal	-2(%rdx), %r9d
	testl	$-3, %r9d
	jne	.L39
.L48:
	movl	%edx, 12(%rax)
	movl	16(%rax), %ecx
	cmpl	$1, %r9d
	ja	.L51
	orl	$3, %ecx
.L41:
	movl	8(%rax), %edx
	leal	-8(%rdx), %r9d
	cmpl	$2, %r9d
	jbe	.L57
	subl	$19, %edx
	cmpl	$1, %edx
	ja	.L54
.L57:
	andl	$-3, %ecx
.L54:
	orl	$8, %ecx
	movl	%ecx, 16(%rax)
	movl	%r8d, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L77:
	cmpl	$5, %ecx
	je	.L39
	movl	$5, 12(%rax)
	movl	16(%rax), %ecx
	jmp	.L41
	.p2align 4,,10
	.p2align 3
.L46:
	cmpl	$1, %edx
	jne	.L39
	movl	$1, 12(%rax)
	movl	16(%rax), %ecx
.L51:
	movl	8(%rax), %r9d
	leal	-8(%r9), %r10d
	cmpl	$2, %r10d
	jbe	.L56
	subl	$19, %r9d
	cmpl	$1, %r9d
	ja	.L52
.L56:
	andl	$-3, %ecx
.L52:
	movl	%ecx, %r9d
	andl	$-3, %r9d
	cmpl	$4, %edx
	cmove	%r9d, %ecx
	jmp	.L54
	.p2align 4,,10
	.p2align 3
.L42:
	cmpl	$2, %edx
	sete	%cl
	jmp	.L49
	.seh_endproc
	.p2align 4
	.globl	nova_control_destroy
	.def	nova_control_destroy;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_control_destroy
nova_control_destroy:
	subq	$56, %rsp
	.seh_stackalloc	56
	.seh_endprologue
	movq	%rcx, %r11
	testq	%rcx, %rcx
	je	.L81
	movzwl	(%rcx), %r8d
	leaq	used(%rip), %r10
	cmpb	$0, (%r10,%r8)
	jne	.L92
.L81:
	xorl	%eax, %eax
	addq	$56, %rsp
	ret
	.p2align 4,,10
	.p2align 3
.L92:
	movl	$5, %edx
	movq	%r8, 32(%rsp)
	movl	%r8d, 44(%rsp)
	call	nova_control_set_state
	testb	%al, %al
	je	.L81
	cmpl	$7, 8(%r11)
	movq	32(%rsp), %r8
	leaq	used(%rip), %r10
	movl	44(%rsp), %r9d
	je	.L93
.L82:
	cmpw	%r9w, focused_id(%rip)
	je	.L94
.L86:
	subw	$1, diagnostics(%rip)
	addw	$1, 6+diagnostics(%rip)
	movb	$0, (%r10,%r8)
	addq	$56, %rsp
	ret
	.p2align 4,,10
	.p2align 3
.L94:
	movl	$-1, %edx
	movw	%dx, focused_id(%rip)
	jmp	.L86
	.p2align 4,,10
	.p2align 3
.L93:
	leaq	133(%r11), %rdx
	leaq	229(%r11), %rcx
	.p2align 4
	.p2align 4
	.p2align 3
.L83:
	movb	$0, (%rdx)
	addq	$2, %rdx
	movb	$0, -1(%rdx)
	cmpq	%rdx, %rcx
	jne	.L83
	movq	$0, 518(%r11)
	movzwl	(%r11), %r8d
	movq	%r8, %r9
	cmpb	$0, (%r10,%r8)
	je	.L82
	cmpl	$5, 12(%r11)
	je	.L82
	orl	$8, 16(%r11)
	jmp	.L82
	.seh_endproc
	.p2align 4
	.globl	nova_control_set_parent
	.def	nova_control_set_parent;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_control_set_parent
nova_control_set_parent:
	.seh_endprologue
	testq	%rcx, %rcx
	sete	%al
	cmpq	%rdx, %rcx
	sete	%r8b
	orl	%r8d, %eax
	testq	%rdx, %rdx
	sete	%r9b
	xorl	%r8d, %r8d
	orb	%r9b, %al
	jne	.L95
	movzwl	(%rcx), %r8d
	leaq	used(%rip), %rax
	movq	%r8, %r9
	movzbl	(%rax,%r8), %r8d
	testb	%r8b, %r8b
	je	.L95
	movzwl	(%rdx), %r8d
	movq	%r8, %r11
	movzbl	(%rax,%r8), %r8d
	testb	%r8b, %r8b
	je	.L95
	cmpw	$-1, 2(%rcx)
	jne	.L101
	movzwl	2(%rdx), %eax
	cmpw	$-1, %ax
	je	.L97
	leaq	controls(%rip), %r10
	jmp	.L98
	.p2align 5
	.p2align 4,,10
	.p2align 3
.L112:
	imulq	$532, %rax, %rax
	movzwl	2(%r10,%rax), %eax
	cmpw	$-1, %ax
	je	.L97
.L98:
	cmpw	%ax, %r9w
	jne	.L112
.L101:
	xorl	%r8d, %r8d
.L95:
	movl	%r8d, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L97:
	movw	%r11w, 2(%rcx)
	movzwl	4(%rdx), %eax
	movw	%ax, 6(%rcx)
	movw	%r9w, 4(%rdx)
	jmp	.L95
	.seh_endproc
	.p2align 4
	.globl	nova_control_set_bounds
	.def	nova_control_set_bounds;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_control_set_bounds
nova_control_set_bounds:
	.seh_endprologue
	movq	(%rdx), %r8
	movq	8(%rdx), %rax
	xorl	%edx, %edx
	testq	%rcx, %rcx
	je	.L113
	testl	%eax, %eax
	jle	.L113
	movq	%rax, %r9
	sarq	$32, %r9
	testl	%r9d, %r9d
	jle	.L113
	orl	$8, 16(%rcx)
	movl	$1, %edx
	movq	%r8, 20(%rcx)
	movq	%rax, 28(%rcx)
.L113:
	movl	%edx, %eax
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_control_set_text
	.def	nova_control_set_text;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_control_set_text
nova_control_set_text:
	pushq	%r12
	.seh_pushreg	%r12
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	movq	%rcx, %rbp
	movq	%rdx, %rbx
	testq	%rcx, %rcx
	je	.L119
	testq	%rdx, %rdx
	je	.L119
	movzbl	(%rdx), %edx
	testb	%dl, %dl
	je	.L120
	movl	$1, %eax
	jmp	.L122
	.p2align 4
	.p2align 4,,10
	.p2align 3
.L137:
	addq	$1, %rax
	cmpq	$97, %rax
	je	.L119
.L122:
	cmpb	$0, (%rbx,%rax)
	jne	.L137
	movl	8(%rbp), %esi
	movl	%eax, %r12d
	leal	-6(%rsi), %ecx
	cmpl	$1, %ecx
	jbe	.L138
.L124:
	movd	%r12d, %xmm1
	xorl	%eax, %eax
	pshuflw	$0, %xmm1, %xmm0
	jmp	.L129
	.p2align 5
	.p2align 4,,10
	.p2align 3
.L131:
	movb	%dl, 133(%rbp,%rax)
	addq	$1, %rax
	cmpq	$95, %rax
	je	.L130
.L129:
	movzbl	(%rbx,%rax), %edx
	testb	%dl, %dl
	jne	.L131
.L130:
	movb	$0, 133(%rbp,%rax)
	movl	$1, %eax
	orl	$8, 16(%rbp)
	movq	%xmm0, 518(%rbp)
	addq	$48, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%rbp
	popq	%r12
	ret
	.p2align 4,,10
	.p2align 3
.L138:
	cmpw	%ax, 526(%rbp)
	jb	.L119
.L123:
	movq	%rbx, 40(%rsp)
	jmp	.L125
	.p2align 4,,10
	.p2align 3
.L126:
	movq	40(%rsp), %rax
	movzbl	(%rax), %edx
.L125:
	testb	%dl, %dl
	je	.L124
	leaq	36(%rsp), %rdx
	leaq	40(%rsp), %rcx
	call	nova_unicode_next
	testb	%al, %al
	jne	.L126
.L119:
	xorl	%eax, %eax
	addq	$48, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%rbp
	popq	%r12
	ret
.L120:
	movl	8(%rcx), %eax
	xorl	%r12d, %r12d
	subl	$6, %eax
	cmpl	$1, %eax
	ja	.L124
	jmp	.L123
	.seh_endproc
	.p2align 4
	.globl	nova_text_field_set_placeholder
	.def	nova_text_field_set_placeholder;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_text_field_set_placeholder
nova_text_field_set_placeholder:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L142
	movl	8(%rcx), %eax
	subl	$6, %eax
	cmpl	$1, %eax
	ja	.L142
	xorl	%eax, %eax
	testq	%rdx, %rdx
	jne	.L143
.L142:
	xorl	%eax, %eax
	ret
	.p2align 5
	.p2align 4,,10
	.p2align 3
.L145:
	movb	%r8b, 325(%rcx,%rax)
	addq	$1, %rax
	cmpq	$95, %rax
	je	.L144
.L143:
	movzbl	(%rdx,%rax), %r8d
	testb	%r8b, %r8b
	jne	.L145
.L144:
	movb	$0, 325(%rcx,%rax)
	movzwl	(%rcx), %eax
	leaq	used(%rip), %rdx
	movzbl	(%rdx,%rax), %eax
	testb	%al, %al
	je	.L142
	cmpl	$5, 12(%rcx)
	je	.L142
	orl	$8, 16(%rcx)
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_text_field_set_mode
	.def	nova_text_field_set_mode;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_text_field_set_mode
nova_text_field_set_mode:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L155
	movl	8(%rcx), %eax
	leal	-6(%rax), %r8d
	cmpl	$1, %r8d
	ja	.L155
	cmpl	$5, %edx
	jbe	.L164
.L155:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L164:
	cmpl	$7, %eax
	jne	.L157
	cmpl	$3, %edx
	jne	.L155
.L157:
	movzwl	(%rcx), %eax
	movl	%edx, 528(%rcx)
	leaq	used(%rip), %rdx
	movzbl	(%rdx,%rax), %eax
	testb	%al, %al
	je	.L155
	cmpl	$5, 12(%rcx)
	je	.L155
	orl	$8, 16(%rcx)
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_text_field_set_maximum
	.def	nova_text_field_set_maximum;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_text_field_set_maximum
nova_text_field_set_maximum:
	.seh_endprologue
	movl	%edx, %r8d
	xorl	%edx, %edx
	testq	%rcx, %rcx
	je	.L165
	movl	8(%rcx), %eax
	subl	$6, %eax
	cmpl	$1, %eax
	jbe	.L172
.L165:
	movl	%edx, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L172:
	leal	-1(%r8), %eax
	cmpw	$94, %ax
	ja	.L165
	cmpw	518(%rcx), %r8w
	jb	.L165
	movl	$1, %edx
	movw	%r8w, 526(%rcx)
	movl	%edx, %eax
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_text_field_insert
	.def	nova_text_field_insert;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_text_field_insert
nova_text_field_insert:
	pushq	%r15
	.seh_pushreg	%r15
	pushq	%r14
	.seh_pushreg	%r14
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$16, %rsp
	.seh_stackalloc	16
	.seh_endprologue
	movq	%rcx, %rax
	movl	%edx, %ecx
	testq	%rax, %rax
	je	.L175
	movl	8(%rax), %edx
	subl	$6, %edx
	cmpl	$1, %edx
	jbe	.L203
.L175:
	xorl	%edx, %edx
.L173:
	movl	%edx, %eax
	addq	$16, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%r14
	popq	%r15
	ret
	.p2align 4,,10
	.p2align 3
.L203:
	testb	$-48, 17(%rax)
	jne	.L175
	cmpl	$31, %ecx
	jbe	.L175
	cmpl	$127, %ecx
	je	.L175
	movl	528(%rax), %edx
	cmpl	$1, %edx
	je	.L204
	cmpl	$2, %edx
	je	.L205
	cmpl	$4, %edx
	je	.L206
.L181:
	cmpl	$127, %ecx
	jbe	.L178
	cmpl	$2047, %ecx
	jbe	.L207
	leal	-55296(%rcx), %edx
	cmpl	$2047, %edx
	jbe	.L175
	cmpl	$65535, %ecx
	jbe	.L208
	cmpl	$1114111, %ecx
	ja	.L175
	movl	%ecx, %r10d
	movl	%ecx, %r8d
	movl	%ecx, %r9d
	andl	$63, %ecx
	shrl	$6, %r10d
	movl	%ecx, %edx
	shrl	$12, %r9d
	movl	.LC0(%rip), %ebx
	movl	%r10d, %ecx
	sall	$8, %edx
	movl	$4, %esi
	movl	$4, %edi
	andl	$63, %ecx
	shrl	$18, %r8d
	orl	%ecx, %edx
	movl	%r9d, %ecx
	andl	$63, %ecx
	sall	$8, %edx
	orl	%ecx, %edx
	movzbl	%r8b, %ecx
	sall	$8, %edx
	orl	%ecx, %edx
	orl	%ebx, %edx
	movl	%edx, 12(%rsp)
	.p2align 4
	.p2align 3
.L185:
	movzwl	522(%rax), %edx
	movzwl	524(%rax), %ecx
	movzwl	518(%rax), %r9d
	cmpw	%cx, %dx
	je	.L188
	cmpw	%dx, %cx
	movl	%edx, %r8d
	movzwl	%r9w, %r11d
	cmovnb	%ecx, %edx
	cmovb	%ecx, %r8d
	movl	%edx, %ebx
	cmpw	%r9w, %dx
	movzwl	%r8w, %ecx
	movl	%r8d, %r15d
	cmova	%r9d, %ebx
	movl	%ebx, %r14d
	subl	%r8d, %ebx
	movzwl	%bx, %r10d
	leal	(%rcx,%r10), %edx
	cmpl	%r11d, %edx
	jg	.L191
	.p2align 6
	.p2align 4
	.p2align 3
.L190:
	movslq	%edx, %rdx
	addl	$1, %r8d
	movzbl	133(%rax,%rdx), %edx
	movb	%dl, 133(%rax,%rcx)
	movzwl	%r8w, %ecx
	leal	(%r10,%rcx), %edx
	cmpl	%r11d, %edx
	jle	.L190
.L191:
	movzwl	%r15w, %ecx
	addl	%r15d, %r9d
	movq	%rcx, %rdx
	subl	%r14d, %r9d
	salq	$16, %rdx
	orq	%rcx, %rdx
	salq	$16, %rdx
	orq	%rcx, %rdx
	movzwl	%r9w, %ecx
	salq	$16, %rdx
	orq	%rcx, %rdx
	movq	%rdx, 518(%rax)
.L188:
	movzwl	%r9w, %edx
	movzwl	526(%rax), %ecx
	addl	%esi, %edx
	cmpl	%ecx, %edx
	jg	.L175
	movzwl	520(%rax), %r10d
	movzwl	%r9w, %r8d
	leal	1(%r9), %ecx
	movq	%r10, %rdx
	salq	$16, %rdx
	orq	%r10, %rdx
	salq	$16, %rdx
	orq	%r10, %rdx
	salq	$16, %rdx
	orq	%r8, %rdx
	movq	%rdx, %r14
	cmpw	%cx, %r10w
	jnb	.L197
	subl	%r10d, %r9d
	movzwl	%cx, %ebx
	leaq	-1(%rax), %rcx
	movzwl	%r9w, %r9d
	leal	-1(%rbx), %r8d
	leal	-1(%rbx,%rsi), %r11d
	subq	%r9, %rcx
	leaq	(%rax,%r8), %rdx
	leaq	(%rcx,%r8), %r9
	.p2align 5
	.p2align 4
	.p2align 3
.L196:
	movzbl	133(%rdx), %ebx
	movq	%rdx, %rcx
	subq	$1, %rdx
	subq	%r8, %rcx
	movb	%bl, 133(%rcx,%r11)
	cmpq	%rdx, %r9
	jne	.L196
.L197:
	movzbl	%dil, %ecx
	leaq	133(%rax,%r10), %r9
	testl	%ecx, %ecx
	je	.L194
	xorl	%edx, %edx
.L193:
	movl	%edx, %r8d
	addl	$1, %edx
	movzbl	12(%rsp,%r8), %r11d
	movb	%r11b, (%r9,%r8)
	cmpl	%ecx, %edx
	jb	.L193
.L194:
	movzbl	%dil, %ebx
	movzwl	(%rax), %edx
	movq	%r14, %xmm1
	leaq	used(%rip), %rcx
	movd	%ebx, %xmm0
	addl	$1, 20+diagnostics(%rip)
	pshuflw	$0, %xmm0, %xmm0
	movzbl	(%rcx,%rdx), %edx
	paddw	%xmm1, %xmm0
	movq	%xmm0, 518(%rax)
	testb	%dl, %dl
	je	.L175
	cmpl	$5, 12(%rax)
	je	.L175
	orl	$8, 16(%rax)
	jmp	.L173
	.p2align 4,,10
	.p2align 3
.L207:
	movl	%ecx, %edx
	movl	$2, %esi
	movl	$2, %edi
	shrl	$6, %edx
	orl	$-64, %edx
	movb	%dl, 12(%rsp)
	movl	%ecx, %edx
	andl	$63, %edx
	orl	$-128, %edx
	movb	%dl, 13(%rsp)
	jmp	.L185
	.p2align 4,,10
	.p2align 3
.L204:
	leal	-48(%rcx), %edx
	cmpl	$9, %edx
	ja	.L175
.L178:
	movb	%cl, 12(%rsp)
	movl	$1, %esi
	movl	$1, %edi
	jmp	.L185
	.p2align 4,,10
	.p2align 3
.L205:
	leal	-48(%rcx), %edx
	cmpl	$54, %edx
	ja	.L175
	movabsq	$35465847073801215, %r8
	btq	%rdx, %r8
	jc	.L178
	jmp	.L175
	.p2align 4,,10
	.p2align 3
.L206:
	leal	-34(%rcx), %edx
	cmpl	$58, %edx
	jbe	.L182
	movl	%ecx, %edx
	andl	$-65, %edx
	cmpl	$60, %edx
	je	.L175
	jmp	.L181
.L208:
	movl	%ecx, %edx
	movl	$3, %esi
	movl	$3, %edi
	shrl	$12, %edx
	orl	$-32, %edx
	movb	%dl, 12(%rsp)
	movl	%ecx, %edx
	shrl	$6, %edx
	andl	$63, %edx
	orl	$-128, %edx
	movb	%dl, 13(%rsp)
	movl	%ecx, %edx
	andl	$63, %edx
	orl	$-128, %edx
	movb	%dl, 14(%rsp)
	jmp	.L185
.L182:
	movabsq	$-288230376973803778, %r8
	btq	%rdx, %r8
	jnc	.L175
	movl	%ecx, %edx
	andl	$-65, %edx
	cmpl	$60, %edx
	je	.L175
	jmp	.L178
	.seh_endproc
	.p2align 4
	.globl	nova_text_field_backspace
	.def	nova_text_field_backspace;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_text_field_backspace
nova_text_field_backspace:
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	.seh_endprologue
	movq	%rcx, %rax
	testq	%rcx, %rcx
	je	.L212
	movl	8(%rcx), %ecx
	leal	-6(%rcx), %edx
	cmpl	$1, %edx
	jbe	.L241
.L212:
	xorl	%edx, %edx
.L209:
	movl	%edx, %eax
	popq	%rbx
	popq	%rsi
	popq	%rdi
	ret
	.p2align 4,,10
	.p2align 3
.L241:
	testb	$-48, 17(%rax)
	jne	.L212
	movzwl	522(%rax), %edx
	movzwl	524(%rax), %r9d
	cmpw	%r9w, %dx
	je	.L213
	cmpw	%dx, %r9w
	movzwl	518(%rax), %r10d
	movl	%edx, %ecx
	cmovnb	%r9d, %edx
	cmovb	%r9d, %ecx
	movl	%r10d, %ebx
	cmpw	%dx, %r10w
	movl	%ecx, %r8d
	movl	%ecx, %edi
	movzwl	%cx, %r9d
	cmovbe	%r10d, %edx
	movzwl	%cx, %ecx
	movl	%edx, %esi
	subl	%r8d, %edx
	movzwl	%dx, %r11d
	leal	(%r11,%rcx), %edx
	cmpl	%r10d, %edx
	jg	.L217
	.p2align 6
	.p2align 4
	.p2align 3
.L215:
	movslq	%edx, %rdx
	addl	$1, %r8d
	movzbl	133(%rax,%rdx), %edx
	movb	%dl, 133(%rax,%rcx)
	movzwl	%r8w, %ecx
	leal	(%rcx,%r11), %edx
	cmpl	%r10d, %edx
	jle	.L215
.L217:
	leal	(%rbx,%rdi), %r10d
	subl	%esi, %r10d
.L216:
	movq	%r9, %rdx
	movzwl	%r10w, %r10d
	leaq	used(%rip), %rcx
	addl	$1, 20+diagnostics(%rip)
	salq	$16, %rdx
	orq	%r9, %rdx
	salq	$16, %rdx
	orq	%r9, %rdx
	salq	$16, %rdx
	orq	%r10, %rdx
	movq	%rdx, 518(%rax)
	movzwl	(%rax), %edx
	movzbl	(%rcx,%rdx), %edx
	testb	%dl, %dl
	je	.L212
	cmpl	$5, 12(%rax)
	je	.L212
	orl	$8, 16(%rax)
	jmp	.L209
	.p2align 4,,10
	.p2align 3
.L213:
	movzwl	520(%rax), %r9d
	testw	%r9w, %r9w
	je	.L212
	movl	%r9d, %r8d
	subw	$1, %r8w
	je	.L226
	movzwl	%r8w, %edx
	leaq	133(%rax,%rdx), %rcx
	jmp	.L219
	.p2align 5
	.p2align 4,,10
	.p2align 3
.L221:
	subq	$1, %rcx
	subw	$1, %r8w
	je	.L220
.L219:
	movsbl	(%rcx), %edx
	andl	$192, %edx
	cmpl	$128, %edx
	je	.L221
	cmpw	%r8w, %r9w
	jnb	.L220
	movzwl	%r9w, %ecx
.L218:
	movzwl	518(%rax), %r11d
	cmpw	%r8w, %r11w
	movl	%r11d, %ebx
	cmovbe	%r11d, %r8d
	movl	%r8d, %edx
	movl	%r8d, %esi
	movzwl	%r11w, %r8d
	movl	%r9d, %r11d
	subl	%r9d, %edx
	movzwl	%dx, %r10d
	leal	(%r10,%rcx), %edx
	cmpl	%r8d, %edx
	jg	.L224
	.p2align 6
	.p2align 4
	.p2align 3
.L223:
	movslq	%edx, %rdx
	addl	$1, %r11d
	movzbl	133(%rax,%rdx), %edx
	movb	%dl, 133(%rax,%rcx)
	movzwl	%r11w, %ecx
	leal	(%r10,%rcx), %edx
	cmpl	%edx, %r8d
	jge	.L223
.L224:
	leal	(%rbx,%r9), %r10d
	subl	%esi, %r10d
	jmp	.L216
	.p2align 4,,10
	.p2align 3
.L220:
	movl	%r9d, %edx
	movzwl	%r8w, %ecx
	movzwl	%r8w, %r9d
	movl	%edx, %r8d
	jmp	.L218
.L226:
	xorl	%r9d, %r9d
	xorl	%ecx, %ecx
	movl	$1, %r8d
	jmp	.L218
	.seh_endproc
	.p2align 4
	.globl	nova_text_field_delete
	.def	nova_text_field_delete;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_text_field_delete
nova_text_field_delete:
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	.seh_endprologue
	movq	%rcx, %rax
	testq	%rcx, %rcx
	je	.L245
	movl	8(%rcx), %ecx
	leal	-6(%rcx), %edx
	cmpl	$1, %edx
	jbe	.L267
.L245:
	xorl	%edx, %edx
.L242:
	movl	%edx, %eax
	popq	%rbx
	popq	%rsi
	popq	%rdi
	ret
	.p2align 4,,10
	.p2align 3
.L267:
	testb	$-48, 17(%rax)
	jne	.L245
	movzwl	518(%rax), %ecx
	movzwl	522(%rax), %r11d
	movzwl	524(%rax), %r8d
	movl	%ecx, %ebx
	cmpw	%r8w, %r11w
	je	.L246
	cmpw	%r11w, %r8w
	movl	%r11d, %r9d
	cmovnb	%r8d, %r11d
	cmovb	%r8d, %r9d
	movl	%r11d, %edx
	cmpw	%cx, %r11w
	movzwl	%cx, %r11d
	movl	%r9d, %edi
	cmova	%ecx, %edx
	movzwl	%r9w, %ecx
	movl	%r9d, %r8d
	movl	%edx, %esi
	subl	%r9d, %edx
	movzwl	%dx, %r10d
	leal	(%r10,%rcx), %edx
	cmpl	%r11d, %edx
	jg	.L250
	.p2align 6
	.p2align 4
	.p2align 3
.L248:
	movslq	%edx, %rdx
	addl	$1, %r9d
	movzbl	133(%rax,%rdx), %edx
	movb	%dl, 133(%rax,%rcx)
	movzwl	%r9w, %ecx
	leal	(%rcx,%r10), %edx
	cmpl	%r11d, %edx
	jle	.L248
.L250:
	leal	(%rdi,%rbx), %r10d
	subl	%esi, %r10d
.L249:
	movzwl	%r8w, %r8d
	movzwl	%r10w, %r10d
	leaq	used(%rip), %rcx
	addl	$1, 20+diagnostics(%rip)
	movq	%r8, %rdx
	salq	$16, %rdx
	orq	%r8, %rdx
	salq	$16, %rdx
	orq	%r8, %rdx
	salq	$16, %rdx
	orq	%r10, %rdx
	movq	%rdx, 518(%rax)
	movzwl	(%rax), %edx
	movzbl	(%rcx,%rdx), %edx
	testb	%dl, %dl
	je	.L245
	cmpl	$5, 12(%rax)
	je	.L245
	orl	$8, 16(%rax)
	jmp	.L242
	.p2align 4,,10
	.p2align 3
.L246:
	movzwl	520(%rax), %r9d
	cmpw	%cx, %r9w
	jnb	.L245
	leal	1(%r9), %r8d
	cmpw	%cx, %r8w
	jnb	.L258
	movzwl	%r8w, %edx
	leaq	133(%rax,%rdx), %rcx
	jmp	.L252
	.p2align 5
	.p2align 4,,10
	.p2align 3
.L254:
	addl	$1, %r8d
	addq	$1, %rcx
	cmpw	%bx, %r8w
	jnb	.L253
.L252:
	movsbl	(%rcx), %edx
	andl	$192, %edx
	cmpl	$128, %edx
	je	.L254
.L253:
	cmpw	%r9w, %r8w
	movl	%r9d, %edx
	cmovnb	%r8d, %r9d
	cmovnb	%edx, %r8d
.L251:
	cmpw	%bx, %r9w
	movzwl	%r8w, %ecx
	cmova	%ebx, %r9d
	movzwl	%bx, %r11d
	movl	%r9d, %r10d
	movl	%r9d, %esi
	movl	%r8d, %r9d
	subl	%r8d, %r10d
	movzwl	%r10w, %r10d
	leal	(%r10,%rcx), %edx
	cmpl	%edx, %r11d
	jl	.L256
	.p2align 6
	.p2align 4
	.p2align 3
.L255:
	movslq	%edx, %rdx
	addl	$1, %r9d
	movzbl	133(%rax,%rdx), %edx
	movb	%dl, 133(%rax,%rcx)
	movzwl	%r9w, %ecx
	leal	(%r10,%rcx), %edx
	cmpl	%edx, %r11d
	jge	.L255
.L256:
	leal	(%rbx,%r8), %r10d
	subl	%esi, %r10d
	jmp	.L249
.L258:
	movl	%r8d, %edx
	movl	%r9d, %r8d
	movl	%edx, %r9d
	jmp	.L251
	.seh_endproc
	.p2align 4
	.globl	nova_text_field_move
	.def	nova_text_field_move;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_text_field_move
nova_text_field_move:
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	.seh_endprologue
	movq	%rcx, %r11
	movl	%edx, %r10d
	testq	%rcx, %rcx
	je	.L271
	movl	8(%rcx), %eax
	subl	$6, %eax
	cmpl	$1, %eax
	jbe	.L307
.L271:
	xorl	%eax, %eax
	popq	%rbx
	popq	%rsi
	ret
	.p2align 4,,10
	.p2align 3
.L307:
	movzwl	520(%rcx), %eax
	leaq	133(%rcx), %r9
	movl	%eax, %esi
	testw	%dx, %dx
	jns	.L308
	.p2align 4
	.p2align 3
.L272:
	testw	%ax, %ax
	je	.L275
	subw	$1, %ax
	je	.L309
	movzwl	%ax, %ecx
	addq	%r9, %rcx
	jmp	.L276
	.p2align 5
	.p2align 4,,10
	.p2align 3
.L278:
	subq	$1, %rcx
	subw	$1, %ax
	je	.L277
.L276:
	movsbl	(%rcx), %edx
	andl	$192, %edx
	cmpl	$128, %edx
	je	.L278
.L277:
	movw	%ax, 520(%r11)
	addw	$1, %r10w
	jne	.L272
.L281:
	testb	%r8b, %r8b
	jne	.L310
.L287:
	movd	%eax, %xmm1
	pshuflw	$0, %xmm1, %xmm0
	movd	%xmm0, 522(%r11)
.L289:
	movzwl	(%r11), %eax
	leaq	used(%rip), %rdx
	movzbl	(%rdx,%rax), %eax
	testb	%al, %al
	je	.L271
	cmpl	$5, 12(%r11)
	je	.L271
	orl	$8, 16(%r11)
	popq	%rbx
	popq	%rsi
	ret
.L308:
	je	.L281
	movzwl	518(%rcx), %r9d
	leaq	133(%rcx), %rbx
	.p2align 4
	.p2align 3
.L282:
	cmpw	%r9w, %ax
	jnb	.L281
	addl	$1, %eax
	cmpw	%r9w, %ax
	jnb	.L283
	movzwl	%ax, %ecx
	addq	%rbx, %rcx
	jmp	.L284
	.p2align 5
	.p2align 4,,10
	.p2align 3
.L285:
	addl	$1, %eax
	addq	$1, %rcx
	cmpw	%r9w, %ax
	jnb	.L283
.L284:
	movsbl	(%rcx), %edx
	andl	$192, %edx
	cmpl	$128, %edx
	je	.L285
.L283:
	movw	%ax, 520(%r11)
	subw	$1, %r10w
	jne	.L282
	testb	%r8b, %r8b
	je	.L287
.L310:
	movzwl	524(%r11), %ebx
	cmpw	%bx, 522(%r11)
	jne	.L288
	movw	%si, 522(%r11)
.L288:
	movw	%ax, 524(%r11)
	jmp	.L289
.L309:
	xorl	%eax, %eax
	movw	%ax, 520(%r11)
.L275:
	xorl	%eax, %eax
	testb	%r8b, %r8b
	je	.L287
	jmp	.L310
	.seh_endproc
	.p2align 4
	.globl	nova_text_field_select_all
	.def	nova_text_field_select_all;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_text_field_select_all
nova_text_field_select_all:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L314
	movl	8(%rcx), %eax
	subl	$6, %eax
	cmpl	$1, %eax
	jbe	.L318
.L314:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L318:
	xorl	%eax, %eax
	leaq	used(%rip), %rdx
	movw	%ax, 522(%rcx)
	movzwl	518(%rcx), %eax
	movw	%ax, 524(%rcx)
	movw	%ax, 520(%rcx)
	movzwl	(%rcx), %eax
	movzbl	(%rdx,%rax), %eax
	testb	%al, %al
	je	.L314
	cmpl	$5, 12(%rcx)
	je	.L314
	orl	$8, 16(%rcx)
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_text_field_clear
	.def	nova_text_field_clear;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_text_field_clear
nova_text_field_clear:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L319
	movl	8(%rcx), %eax
	subl	$6, %eax
	cmpl	$1, %eax
	jbe	.L326
.L319:
	ret
	.p2align 4,,10
	.p2align 3
.L326:
	leaq	133(%rcx), %rax
	leaq	229(%rcx), %rdx
	.p2align 4
	.p2align 4
	.p2align 3
.L323:
	movb	$0, (%rax)
	addq	$2, %rax
	movb	$0, -1(%rax)
	cmpq	%rdx, %rax
	jne	.L323
	movzwl	(%rcx), %eax
	leaq	used(%rip), %rdx
	movq	$0, 518(%rcx)
	cmpb	$0, (%rdx,%rax)
	je	.L319
	cmpl	$5, 12(%rcx)
	je	.L319
	orl	$8, 16(%rcx)
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_control_set_accessibility
	.def	nova_control_set_accessibility;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_control_set_accessibility
nova_control_set_accessibility:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L338
	testb	%r9b, %r9b
	jne	.L329
	xorl	%eax, %eax
	testq	%r8, %r8
	je	.L327
	cmpb	$0, (%r8)
	je	.L327
	movw	%dx, 130(%rcx)
	leaq	229(%rcx), %r10
.L330:
	xorl	%eax, %eax
	jmp	.L333
	.p2align 5
	.p2align 4,,10
	.p2align 3
.L335:
	movb	%dl, (%r10,%rax)
	addq	$1, %rax
	cmpq	$95, %rax
	je	.L334
.L333:
	movzbl	(%r8,%rax), %edx
	testb	%dl, %dl
	jne	.L335
.L334:
	movb	$0, (%r10,%rax)
	testb	%r9b, %r9b
	jne	.L332
	movl	16(%rcx), %eax
	andb	$-3, %ah
.L337:
	movl	%eax, 16(%rcx)
	movl	$1, %eax
.L327:
	ret
	.p2align 4,,10
	.p2align 3
.L329:
	movw	%dx, 130(%rcx)
	testq	%r8, %r8
	jne	.L331
	movb	$0, 229(%rcx)
.L332:
	movl	16(%rcx), %eax
	orb	$2, %ah
	jmp	.L337
	.p2align 4,,10
	.p2align 3
.L331:
	leaq	229(%rcx), %r10
	jmp	.L330
	.p2align 4,,10
	.p2align 3
.L338:
	xorl	%eax, %eax
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_control_set_range
	.def	nova_control_set_range;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_control_set_range
nova_control_set_range:
	.seh_endprologue
	testq	%rcx, %rcx
	sete	%al
	cmpl	%r8d, %edx
	setge	%r10b
	orb	%r10b, %al
	jne	.L346
	cmpl	%r9d, %edx
	jg	.L344
	cmpl	%r9d, %r8d
	jl	.L344
	orl	$8, 16(%rcx)
	movl	$1, %eax
	movl	%edx, 84(%rcx)
	movl	%r8d, 88(%rcx)
	movl	%r9d, 80(%rcx)
.L344:
	ret
	.p2align 4,,10
	.p2align 3
.L346:
	xorl	%eax, %eax
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_control_set_step
	.def	nova_control_set_step;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_control_set_step
nova_control_set_step:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L351
	testl	%edx, %edx
	jle	.L351
	movl	88(%rcx), %eax
	subl	84(%rcx), %eax
	cmpl	%edx, %eax
	jge	.L355
.L351:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L355:
	movzwl	(%rcx), %eax
	movl	%edx, 92(%rcx)
	leaq	used(%rip), %rdx
	movzbl	(%rdx,%rax), %eax
	testb	%al, %al
	je	.L351
	cmpl	$5, 12(%rcx)
	je	.L351
	orl	$8, 16(%rcx)
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_control_set_value
	.def	nova_control_set_value;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_control_set_value
nova_control_set_value:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L359
	cmpl	%edx, 84(%rcx)
	jg	.L359
	cmpl	%edx, 88(%rcx)
	jge	.L363
.L359:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L363:
	movzwl	(%rcx), %eax
	movl	%edx, 80(%rcx)
	leaq	used(%rip), %rdx
	addl	$1, 20+diagnostics(%rip)
	movzbl	(%rdx,%rax), %eax
	testb	%al, %al
	je	.L359
	cmpl	$5, 12(%rcx)
	je	.L359
	orl	$8, 16(%rcx)
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_control_adjust
	.def	nova_control_adjust;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_control_adjust
nova_control_adjust:
	.seh_endprologue
	movq	%rcx, %rax
	movslq	%edx, %rcx
	testq	%rax, %rax
	je	.L367
	testb	$-64, 17(%rax)
	jne	.L367
	movl	8(%rax), %edx
	leal	-14(%rdx), %r8d
	cmpl	$1, %r8d
	jbe	.L369
	cmpl	$5, %edx
	je	.L369
	.p2align 4
	.p2align 3
.L367:
	xorl	%edx, %edx
.L364:
	movl	%edx, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L369:
	movslq	92(%rax), %rdx
	movl	84(%rax), %r8d
	movl	88(%rax), %r9d
	imulq	%rcx, %rdx
	movslq	80(%rax), %rcx
	addq	%rcx, %rdx
	movslq	%r8d, %rcx
	cmpq	%rcx, %rdx
	cmovl	%rcx, %rdx
	movslq	%r9d, %rcx
	cmpq	%rcx, %rdx
	cmovg	%rcx, %rdx
	cmpl	%edx, %r8d
	jg	.L367
	cmpl	%edx, %r9d
	jl	.L367
	movl	%edx, 80(%rax)
	movzwl	(%rax), %edx
	leaq	used(%rip), %rcx
	addl	$1, 20+diagnostics(%rip)
	movzbl	(%rcx,%rdx), %edx
	testb	%dl, %dl
	je	.L367
	cmpl	$5, 12(%rax)
	je	.L367
	orl	$8, 16(%rax)
	jmp	.L364
	.seh_endproc
	.p2align 4
	.globl	nova_control_set_flags
	.def	nova_control_set_flags;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_control_set_flags
nova_control_set_flags:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L379
	movzwl	(%rcx), %eax
	leaq	used(%rip), %r8
	movzbl	(%r8,%rax), %eax
	testb	%al, %al
	je	.L376
	cmpl	$5, 12(%rcx)
	je	.L379
	orl	$8, %edx
	movl	%edx, 16(%rcx)
.L376:
	ret
	.p2align 4,,10
	.p2align 3
.L379:
	xorl	%eax, %eax
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_control_set_checked
	.def	nova_control_set_checked;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_control_set_checked
nova_control_set_checked:
	.seh_endprologue
	movl	%edx, %r8d
	testq	%rcx, %rcx
	je	.L387
	movl	8(%rcx), %eax
	leal	-3(%rax), %edx
	cmpl	$1, %edx
	jbe	.L386
	testl	%eax, %eax
	jne	.L387
	cmpw	$6, 124(%rcx)
	jne	.L387
.L386:
	movl	16(%rcx), %eax
	movl	%eax, %edx
	shrl	$10, %edx
	andl	$1, %edx
	cmpb	%dl, %r8b
	je	.L395
.L388:
	movl	%eax, %edx
	andb	$-5, %ah
	orb	$4, %dh
	testb	%r8b, %r8b
	cmovne	%edx, %eax
	leaq	used(%rip), %rdx
	andb	$-9, %ah
	movl	%eax, 16(%rcx)
	movzwl	(%rcx), %eax
	addl	$1, 24+diagnostics(%rip)
	cmpb	$0, (%rdx,%rax)
	je	.L387
	cmpl	$5, 12(%rcx)
	je	.L387
	orl	$8, 16(%rcx)
.L389:
	movl	$1, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L387:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L395:
	testb	$8, %ah
	jne	.L388
	jmp	.L389
	.seh_endproc
	.p2align 4
	.globl	nova_control_toggle
	.def	nova_control_toggle;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_control_toggle
nova_control_toggle:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L396
	movl	16(%rcx), %edx
	testb	$-112, %dh
	je	.L401
.L396:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L401:
	shrl	$10, %edx
	andl	$1, %edx
	xorl	$1, %edx
	jmp	nova_control_set_checked
	.seh_endproc
	.p2align 4
	.globl	nova_activity_start
	.def	nova_activity_start;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_activity_start
nova_activity_start:
	.seh_endprologue
	xorl	%eax, %eax
	testq	%rcx, %rcx
	je	.L402
	cmpl	$16, 8(%rcx)
	jne	.L402
	movl	16(%rcx), %edx
	testb	$-96, %dh
	jne	.L402
	orl	$4104, %edx
	movl	$1, %eax
	movl	%edx, 16(%rcx)
.L402:
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_activity_stop
	.def	nova_activity_stop;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_activity_stop
nova_activity_stop:
	.seh_endprologue
	xorl	%eax, %eax
	testq	%rcx, %rcx
	je	.L407
	cmpl	$16, 8(%rcx)
	je	.L411
.L407:
	ret
	.p2align 4,,10
	.p2align 3
.L411:
	movl	16(%rcx), %eax
	andb	$-17, %ah
	orl	$8, %eax
	movl	%eax, 16(%rcx)
	movl	$1, %eax
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_activity_set_style
	.def	nova_activity_set_style;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_activity_set_style
nova_activity_set_style:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L415
	cmpl	$16, 8(%rcx)
	jne	.L415
	cmpl	$3, %edx
	jbe	.L416
.L415:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L416:
	orl	$8, 16(%rcx)
	movl	$1, %eax
	movw	%dx, 124(%rcx)
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_activity_set_phase
	.def	nova_activity_set_phase;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_activity_set_phase
nova_activity_set_phase:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L420
	cmpl	$16, 8(%rcx)
	jne	.L420
	cmpw	$1000, %dx
	jbe	.L421
.L420:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L421:
	orl	$8, 16(%rcx)
	movzwl	%dx, %edx
	movl	$1, %eax
	movl	%edx, 80(%rcx)
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_activity_running
	.def	nova_activity_running;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_activity_running
nova_activity_running:
	.seh_endprologue
	xorl	%eax, %eax
	testq	%rcx, %rcx
	je	.L422
	cmpl	$16, 8(%rcx)
	je	.L427
.L422:
	ret
	.p2align 4,,10
	.p2align 3
.L427:
	movzbl	17(%rcx), %eax
	shrb	$4, %al
	andl	$1, %eax
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_scroll_view_configure
	.def	nova_scroll_view_configure;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_scroll_view_configure
nova_scroll_view_configure:
	subq	$40, %rsp
	.seh_stackalloc	40
	.seh_endprologue
	xorl	%eax, %eax
	testq	%rcx, %rcx
	je	.L428
	cmpl	$13, 8(%rcx)
	jne	.L428
	testl	%edx, %edx
	setle	%al
	testl	%r8d, %r8d
	setle	%r10b
	orb	%r10b, %al
	jne	.L434
	cmpl	%r9d, %edx
	jg	.L428
	cmpl	80(%rsp), %r8d
	jg	.L428
	movd	80(%rsp), %xmm3
	movd	%r9d, %xmm0
	movd	%edx, %xmm1
	subl	%edx, %r9d
	movd	%r8d, %xmm2
	punpckldq	%xmm2, %xmm1
	punpckldq	%xmm3, %xmm0
	punpcklqdq	%xmm1, %xmm0
	movups	%xmm0, 104(%rcx)
	cmpl	%r9d, 96(%rcx)
	jle	.L430
	movl	%r9d, 96(%rcx)
.L430:
	movl	80(%rsp), %eax
	subl	%r8d, %eax
	cmpl	%eax, 100(%rcx)
	jle	.L431
	movl	%eax, 100(%rcx)
.L431:
	orl	$8, 16(%rcx)
	call	sync_scrollbars
	movl	$1, %eax
.L428:
	addq	$40, %rsp
	ret
	.p2align 4,,10
	.p2align 3
.L434:
	xorl	%eax, %eax
	addq	$40, %rsp
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_scroll_view_scroll_to
	.def	nova_scroll_view_scroll_to;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_scroll_view_scroll_to
nova_scroll_view_scroll_to:
	subq	$56, %rsp
	.seh_stackalloc	56
	.seh_endprologue
	xorl	%r9d, %r9d
	testq	%rcx, %rcx
	je	.L435
	cmpl	$13, 8(%rcx)
	jne	.L435
	movl	112(%rcx), %r10d
	testl	%r10d, %r10d
	jle	.L435
	movl	104(%rcx), %eax
	xorl	%r9d, %r9d
	subl	%r10d, %eax
	testl	%edx, %edx
	cmovs	%r9d, %edx
	cmpl	%edx, %eax
	cmovg	%edx, %eax
	movl	108(%rcx), %edx
	subl	116(%rcx), %edx
	testl	%r8d, %r8d
	cmovs	%r9d, %r8d
	movl	$1, %r9d
	cmpl	%r8d, %edx
	cmovg	%r8d, %edx
	cmpl	%eax, 96(%rcx)
	je	.L442
.L437:
	orl	$8, 16(%rcx)
	movl	%eax, 96(%rcx)
	movl	%edx, 100(%rcx)
	movb	%r9b, 47(%rsp)
	call	sync_scrollbars
	movzbl	47(%rsp), %r9d
.L435:
	movl	%r9d, %eax
	addq	$56, %rsp
	ret
	.p2align 4,,10
	.p2align 3
.L442:
	cmpl	%edx, 100(%rcx)
	setne	%r9b
	jmp	.L437
	.seh_endproc
	.p2align 4
	.globl	nova_scroll_view_scroll_by
	.def	nova_scroll_view_scroll_by;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_scroll_view_scroll_by
nova_scroll_view_scroll_by:
	subq	$56, %rsp
	.seh_stackalloc	56
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L444
	cmpl	$13, 8(%rcx)
	je	.L445
.L444:
	xorl	%eax, %eax
	addq	$56, %rsp
	ret
	.p2align 4,,10
	.p2align 3
.L445:
	movl	112(%rcx), %r9d
	testl	%r9d, %r9d
	jle	.L444
	movl	96(%rcx), %r11d
	movl	104(%rcx), %eax
	movl	100(%rcx), %r10d
	subl	%r9d, %eax
	movl	$0, %r9d
	addl	%r11d, %edx
	cmovs	%r9d, %edx
	cmpl	%edx, %eax
	cmovle	%eax, %edx
	movl	108(%rcx), %eax
	subl	116(%rcx), %eax
	addl	%r10d, %r8d
	cmovs	%r9d, %r8d
	movl	16(%rcx), %r9d
	cmpl	%r8d, %eax
	cmovle	%eax, %r8d
	cmpl	%edx, %r11d
	setne	%al
	cmpl	%r8d, %r10d
	setne	%r10b
	orl	$8, %r9d
	orb	%r10b, %al
	je	.L451
	movd	%edx, %xmm0
	movd	%r8d, %xmm1
	movl	%r9d, 16(%rcx)
	punpckldq	%xmm1, %xmm0
	movb	%al, 47(%rsp)
	movq	%xmm0, 96(%rcx)
	call	sync_scrollbars
	movzbl	47(%rsp), %eax
	addq	$56, %rsp
	ret
	.p2align 4,,10
	.p2align 3
.L451:
	movl	%edx, 96(%rcx)
	movl	%r8d, 100(%rcx)
	movl	%r9d, 16(%rcx)
	call	sync_scrollbars
	jmp	.L444
	.seh_endproc
	.p2align 4
	.globl	nova_scroll_view_scroll_into_view
	.def	nova_scroll_view_scroll_into_view;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_scroll_view_scroll_into_view
nova_scroll_view_scroll_into_view:
	pushq	%rbx
	.seh_pushreg	%rbx
	.seh_endprologue
	testq	%rcx, %rcx
	sete	%al
	testq	%rdx, %rdx
	sete	%r8b
	orb	%r8b, %al
	jne	.L452
	movl	8(%rdx), %eax
	testl	%eax, %eax
	jle	.L452
	movl	12(%rdx), %r10d
	testl	%r10d, %r10d
	jle	.L452
	movl	96(%rcx), %r8d
	movl	(%rdx), %r9d
	movl	100(%rcx), %r11d
	cmpl	%r8d, %r9d
	jge	.L459
	movl	4(%rdx), %r8d
	cmpl	%r11d, %r8d
	jge	.L460
.L455:
	movl	%r9d, %edx
	popq	%rbx
	jmp	nova_scroll_view_scroll_to
	.p2align 4,,10
	.p2align 3
.L452:
	xorl	%eax, %eax
	popq	%rbx
	ret
	.p2align 4,,10
	.p2align 3
.L460:
	movl	116(%rcx), %eax
	addl	%r8d, %r10d
	movl	%r10d, %r8d
	leal	(%rax,%r11), %edx
	subl	%eax, %r8d
	cmpl	%edx, %r10d
	movl	%r9d, %edx
	cmovle	%r11d, %r8d
	popq	%rbx
	jmp	nova_scroll_view_scroll_to
	.p2align 4,,10
	.p2align 3
.L459:
	addl	%r9d, %eax
	movl	112(%rcx), %r9d
	movl	%eax, %ebx
	subl	112(%rcx), %ebx
	addl	%r8d, %r9d
	cmpl	%r9d, %eax
	movl	%ebx, %r9d
	cmovle	%r8d, %r9d
	movl	4(%rdx), %r8d
	cmpl	%r11d, %r8d
	jl	.L455
	jmp	.L460
	.seh_endproc
	.p2align 4
	.globl	nova_scrollbar_attach
	.def	nova_scrollbar_attach;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_scrollbar_attach
nova_scrollbar_attach:
	subq	$56, %rsp
	.seh_stackalloc	56
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L464
	cmpl	$14, 8(%rcx)
	jne	.L464
	testq	%rdx, %rdx
	je	.L464
	cmpl	$13, 8(%rdx)
	jne	.L464
	cmpl	$1, %r8d
	jbe	.L468
	.p2align 4
	.p2align 3
.L464:
	xorl	%eax, %eax
.L461:
	addq	$56, %rsp
	ret
	.p2align 4,,10
	.p2align 3
.L468:
	cmpw	$-1, 2(%rcx)
	jne	.L464
	movl	%r8d, 80(%rsp)
	call	nova_control_set_parent
	testb	%al, %al
	je	.L464
	movl	80(%rsp), %r8d
	movb	%al, 47(%rsp)
	movw	%r8w, 124(%rcx)
	movq	%rdx, %rcx
	call	sync_scrollbars
	movzbl	47(%rsp), %eax
	jmp	.L461
	.seh_endproc
	.p2align 4
	.globl	nova_card_set_type
	.def	nova_card_set_type;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_card_set_type
nova_card_set_type:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L472
	cmpl	$17, 8(%rcx)
	jne	.L472
	cmpl	$6, %edx
	jbe	.L473
.L472:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L473:
	orl	$8, 16(%rcx)
	movl	$1, %eax
	movw	%dx, 124(%rcx)
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_card_add_child
	.def	nova_card_add_child;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_card_add_child
nova_card_add_child:
	.seh_endprologue
	movq	%rdx, %rax
	testq	%rcx, %rcx
	je	.L474
	cmpl	$17, 8(%rcx)
	je	.L479
.L474:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L479:
	movq	%rcx, %rdx
	movq	%rax, %rcx
	jmp	nova_control_set_parent
	.seh_endproc
	.p2align 4
	.globl	nova_tile_set_type
	.def	nova_tile_set_type;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_tile_set_type
nova_tile_set_type:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L483
	cmpl	$18, 8(%rcx)
	jne	.L483
	cmpl	$6, %edx
	jbe	.L484
.L483:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L484:
	orl	$8, 16(%rcx)
	movl	$1, %eax
	movw	%dx, 124(%rcx)
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_tile_set_description
	.def	nova_tile_set_description;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_tile_set_description
nova_tile_set_description:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L491
	cmpl	$18, 8(%rcx)
	jne	.L491
	xorl	%eax, %eax
	testq	%rdx, %rdx
	jne	.L487
.L491:
	xorl	%eax, %eax
	ret
	.p2align 5
	.p2align 4,,10
	.p2align 3
.L489:
	movb	%r8b, 325(%rcx,%rax)
	addq	$1, %rax
	cmpq	$95, %rax
	je	.L488
.L487:
	movzbl	(%rdx,%rax), %r8d
	testb	%r8b, %r8b
	jne	.L489
.L488:
	movb	$0, 325(%rcx,%rax)
	movl	$1, %eax
	orl	$8, 16(%rcx)
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_tile_set_status
	.def	nova_tile_set_status;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_tile_set_status
nova_tile_set_status:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L501
	cmpl	$18, 8(%rcx)
	jne	.L501
	xorl	%eax, %eax
	testq	%rdx, %rdx
	jne	.L497
.L501:
	xorl	%eax, %eax
	ret
	.p2align 5
	.p2align 4,,10
	.p2align 3
.L499:
	movb	%r8b, 421(%rcx,%rax)
	addq	$1, %rax
	cmpq	$95, %rax
	je	.L498
.L497:
	movzbl	(%rdx,%rax), %r8d
	testb	%r8b, %r8b
	jne	.L499
.L498:
	movb	$0, 421(%rcx,%rax)
	movl	$1, %eax
	orl	$8, 16(%rcx)
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_icon_button_set_icon
	.def	nova_icon_button_set_icon;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_icon_button_set_icon
nova_icon_button_set_icon:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L508
	cmpl	$1, 8(%rcx)
	jne	.L508
	cmpw	$14, %dx
	jbe	.L509
.L508:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L509:
	orl	$8, 16(%rcx)
	movzwl	%dx, %edx
	movl	$1, %eax
	movl	%edx, 80(%rcx)
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_icon_button_set_tooltip
	.def	nova_icon_button_set_tooltip;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_icon_button_set_tooltip
nova_icon_button_set_tooltip:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L516
	cmpl	$1, 8(%rcx)
	setne	%al
	testq	%rdx, %rdx
	sete	%r8b
	orb	%r8b, %al
	je	.L520
.L516:
	xorl	%eax, %eax
.L510:
	ret
	.p2align 4,,10
	.p2align 3
.L520:
	cmpb	$0, (%rdx)
	je	.L510
	xorl	%eax, %eax
	jmp	.L512
	.p2align 5
	.p2align 4,,10
	.p2align 3
.L514:
	movb	%r8b, 325(%rcx,%rax)
	addq	$1, %rax
	cmpq	$95, %rax
	je	.L513
.L512:
	movzbl	(%rdx,%rax), %r8d
	testb	%r8b, %r8b
	jne	.L514
.L513:
	movb	$0, 325(%rcx,%rax)
	movl	$1, %eax
	orl	$8, 16(%rcx)
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_icon_button_set_action
	.def	nova_icon_button_set_action;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_icon_button_set_action
nova_icon_button_set_action:
	.seh_endprologue
	xorl	%eax, %eax
	testq	%rcx, %rcx
	je	.L521
	cmpl	$1, 8(%rcx)
	je	.L525
.L521:
	ret
	.p2align 4,,10
	.p2align 3
.L525:
	orl	$8, 16(%rcx)
	movl	$1, %eax
	movl	%edx, 120(%rcx)
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_button_set_type
	.def	nova_button_set_type;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_button_set_type
nova_button_set_type:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L529
	movl	8(%rcx), %eax
	testl	%eax, %eax
	jne	.L529
	cmpl	$6, %edx
	ja	.L529
	orl	$8, 16(%rcx)
	movl	$1, %eax
	movw	%dx, 124(%rcx)
	ret
	.p2align 4,,10
	.p2align 3
.L529:
	xorl	%eax, %eax
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_button_set_icon
	.def	nova_button_set_icon;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_button_set_icon
nova_button_set_icon:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L533
	movl	8(%rcx), %eax
	testl	%eax, %eax
	jne	.L533
	cmpw	$14, %dx
	ja	.L533
	orl	$8, 16(%rcx)
	movzwl	%dx, %edx
	movl	$1, %eax
	movl	%edx, 84(%rcx)
	ret
	.p2align 4,,10
	.p2align 3
.L533:
	xorl	%eax, %eax
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_button_set_action
	.def	nova_button_set_action;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_button_set_action
nova_button_set_action:
	.seh_endprologue
	xorl	%eax, %eax
	testq	%rcx, %rcx
	je	.L534
	movl	8(%rcx), %r8d
	testl	%r8d, %r8d
	jne	.L534
	orl	$8, 16(%rcx)
	movl	$1, %eax
	movl	%edx, 120(%rcx)
.L534:
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_button_pressed
	.def	nova_button_pressed;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_button_pressed
nova_button_pressed:
	.seh_endprologue
	xorl	%eax, %eax
	testq	%rcx, %rcx
	je	.L538
	movl	8(%rcx), %edx
	testl	%edx, %edx
	jne	.L538
	movzbl	16(%rcx), %eax
	shrb	$5, %al
	andl	$1, %eax
.L538:
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_menu_button_bind
	.def	nova_menu_button_bind;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_menu_button_bind
nova_menu_button_bind:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L546
	cmpl	$2, 8(%rcx)
	setne	%al
	testq	%rdx, %rdx
	sete	%r8b
	orb	%r8b, %al
	je	.L550
.L546:
	xorl	%eax, %eax
.L543:
	ret
	.p2align 4,,10
	.p2align 3
.L550:
	cmpl	$22, 8(%rdx)
	jne	.L543
	movzwl	(%rdx), %eax
	leaq	used(%rip), %r8
	movq	%rax, %rdx
	movzbl	(%r8,%rax), %eax
	testb	%al, %al
	je	.L543
	movl	%edx, 80(%rcx)
	movl	16(%rcx), %edx
	andl	$-65537, %edx
	orl	$8, %edx
	movl	%edx, 16(%rcx)
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_menu_button_open
	.def	nova_menu_button_open;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_menu_button_open
nova_menu_button_open:
	.seh_endprologue
	xorl	%eax, %eax
	testq	%rcx, %rcx
	je	.L551
	cmpl	$2, 8(%rcx)
	je	.L561
.L551:
	ret
	.p2align 4,,10
	.p2align 3
.L561:
	movl	16(%rcx), %edx
	movl	%edx, %r8d
	andl	$65538, %r8d
	cmpl	$2, %r8d
	jne	.L551
	movslq	80(%rcx), %r8
	cmpl	$127, %r8d
	ja	.L551
	leaq	used(%rip), %rax
	movzbl	(%rax,%r8), %eax
	testb	%al, %al
	je	.L551
	imulq	$532, %r8, %r8
	leaq	controls(%rip), %r9
	cmpl	$22, 8(%r9,%r8)
	je	.L562
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L562:
	orl	$65544, %edx
	movl	%edx, 16(%rcx)
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_menu_button_close
	.def	nova_menu_button_close;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_menu_button_close
nova_menu_button_close:
	.seh_endprologue
	xorl	%eax, %eax
	testq	%rcx, %rcx
	je	.L563
	cmpl	$2, 8(%rcx)
	je	.L567
.L563:
	ret
	.p2align 4,,10
	.p2align 3
.L567:
	movl	16(%rcx), %edx
	movl	%edx, %eax
	andl	$-65537, %edx
	shrl	$16, %eax
	orl	$8, %edx
	andl	$1, %eax
	movl	%edx, 16(%rcx)
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_menu_button_expanded
	.def	nova_menu_button_expanded;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_menu_button_expanded
nova_menu_button_expanded:
	.seh_endprologue
	xorl	%eax, %eax
	testq	%rcx, %rcx
	je	.L568
	cmpl	$2, 8(%rcx)
	je	.L573
.L568:
	ret
	.p2align 4,,10
	.p2align 3
.L573:
	movzbl	18(%rcx), %eax
	andl	$1, %eax
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_label_set_type
	.def	nova_label_set_type;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_label_set_type
nova_label_set_type:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L577
	cmpl	$8, 8(%rcx)
	jne	.L577
	cmpl	$8, %edx
	jbe	.L578
.L577:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L578:
	orl	$8, 16(%rcx)
	movl	$1, %eax
	movw	%dx, 124(%rcx)
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_label_set_alignment
	.def	nova_label_set_alignment;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_label_set_alignment
nova_label_set_alignment:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L582
	cmpl	$8, 8(%rcx)
	jne	.L582
	cmpl	$8, %edx
	jbe	.L583
.L582:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L583:
	orl	$8, 16(%rcx)
	movl	$1, %eax
	movl	%edx, 80(%rcx)
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_label_set_scale
	.def	nova_label_set_scale;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_label_set_scale
nova_label_set_scale:
	.seh_endprologue
	xorl	%eax, %eax
	testq	%rcx, %rcx
	je	.L584
	cmpl	$8, 8(%rcx)
	jne	.L584
	leal	-500(%rdx), %r8d
	cmpw	$2500, %r8w
	ja	.L584
	movzwl	%dx, %edx
	orl	$8, 16(%rcx)
	movl	$1, %eax
	movl	%edx, 92(%rcx)
.L584:
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_label_get_text
	.def	nova_label_get_text;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_label_get_text
nova_label_get_text:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L591
	xorl	%edx, %edx
	leaq	133(%rcx), %rax
	cmpl	$8, 8(%rcx)
	cmovne	%rdx, %rax
	ret
	.p2align 4,,10
	.p2align 3
.L591:
	xorl	%eax, %eax
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_icon_control_set
	.def	nova_icon_control_set;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_icon_control_set
nova_icon_control_set:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L596
	cmpl	$9, 8(%rcx)
	jne	.L596
	cmpw	$14, %dx
	jbe	.L597
.L596:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L597:
	orl	$8, 16(%rcx)
	movzwl	%dx, %edx
	movl	$1, %eax
	movl	%edx, 80(%rcx)
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_icon_control_get
	.def	nova_icon_control_get;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_icon_control_get
nova_icon_control_get:
	.seh_endprologue
	movl	$-1, %eax
	testq	%rcx, %rcx
	je	.L598
	cmpl	$9, 8(%rcx)
	je	.L602
.L598:
	ret
	.p2align 4,,10
	.p2align 3
.L602:
	movzwl	80(%rcx), %eax
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_image_set_resource
	.def	nova_image_set_resource;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_image_set_resource
nova_image_set_resource:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L606
	cmpl	$10, 8(%rcx)
	jne	.L606
	testl	%edx, %edx
	je	.L607
.L606:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L607:
	orl	$8, 16(%rcx)
	movl	$1, %eax
	movl	$0, 80(%rcx)
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_image_set_scaling
	.def	nova_image_set_scaling;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_image_set_scaling
nova_image_set_scaling:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L611
	cmpl	$10, 8(%rcx)
	jne	.L611
	cmpl	$4, %edx
	jbe	.L612
.L611:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L612:
	orl	$8, 16(%rcx)
	movl	$1, %eax
	movw	%dx, 124(%rcx)
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_image_set_tint
	.def	nova_image_set_tint;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_image_set_tint
nova_image_set_tint:
	.seh_endprologue
	xorl	%eax, %eax
	testq	%rcx, %rcx
	je	.L613
	cmpl	$10, 8(%rcx)
	je	.L617
.L613:
	ret
	.p2align 4,,10
	.p2align 3
.L617:
	orl	$8, 16(%rcx)
	movl	$1, %eax
	movl	%edx, 40(%rcx)
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_separator_set_orientation
	.def	nova_separator_set_orientation;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_separator_set_orientation
nova_separator_set_orientation:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L621
	cmpl	$20, 8(%rcx)
	jne	.L621
	cmpl	$1, %edx
	jbe	.L622
.L621:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L622:
	movl	16(%rcx), %eax
	movw	%dx, 124(%rcx)
	andl	$-3, %eax
	orl	$520, %eax
	movl	%eax, 16(%rcx)
	movl	$1, %eax
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_separator_set_title
	.def	nova_separator_set_title;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_separator_set_title
nova_separator_set_title:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L623
	cmpl	$20, 8(%rcx)
	jne	.L623
	testq	%rdx, %rdx
	je	.L623
	jmp	nova_control_set_text
	.p2align 4,,10
	.p2align 3
.L623:
	xorl	%eax, %eax
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_list_add_item
	.def	nova_list_add_item;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_list_add_item
nova_list_add_item:
	subq	$56, %rsp
	.seh_stackalloc	56
	.seh_endprologue
	movq	%rcx, %r8
	movq	%rdx, %rcx
	testq	%r8, %r8
	je	.L631
	cmpl	$11, 8(%r8)
	jne	.L631
	testq	%rdx, %rdx
	je	.L631
	cmpl	$12, 8(%rdx)
	je	.L635
	.p2align 4
	.p2align 3
.L631:
	xorl	%eax, %eax
.L628:
	addq	$56, %rsp
	ret
	.p2align 4,,10
	.p2align 3
.L635:
	cmpw	$-1, 2(%rdx)
	jne	.L631
	movzwl	(%r8), %eax
	leaq	list_entry_count(%rip), %r10
	movzwl	(%r10,%rax,2), %r9d
	cmpw	$63, %r9w
	ja	.L631
	movq	%r8, %rdx
	movq	%r8, 64(%rsp)
	movl	%r9d, 44(%rsp)
	call	nova_control_set_parent
	testb	%al, %al
	je	.L631
	movq	64(%rsp), %r8
	movl	44(%rsp), %r9d
	movzwl	(%r8), %r11d
	movzwl	%r9w, %edx
	addl	$1, %r9d
	movq	%r11, %r10
	salq	$6, %r10
	addq	%rdx, %r10
	movzwl	(%rcx), %edx
	leaq	list_entries(%rip), %rcx
	orl	$8, 16(%r8)
	movw	%dx, (%rcx,%r10,2)
	leaq	list_entry_count(%rip), %r10
	movw	%r9w, (%r10,%r11,2)
	movzwl	%r9w, %r9d
	movl	%r9d, 108(%r8)
	jmp	.L628
	.seh_endproc
	.p2align 4
	.globl	nova_list_remove_item
	.def	nova_list_remove_item;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_list_remove_item
nova_list_remove_item:
	pushq	%r15
	.seh_pushreg	%r15
	pushq	%r14
	.seh_pushreg	%r14
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	.seh_endprologue
	xorl	%eax, %eax
	movq	%rcx, %r8
	movl	%edx, %r9d
	testq	%rcx, %rcx
	je	.L636
	cmpl	$11, 8(%rcx)
	jne	.L636
	movzwl	(%rcx), %edx
	leaq	list_entry_count(%rip), %r11
	cmpw	(%r11,%rdx,2), %r9w
	jnb	.L636
	movzwl	%r9w, %eax
	salq	$6, %rdx
	leaq	list_entries(%rip), %rcx
	movzwl	%r9w, %r10d
	addq	%rax, %rdx
	movzwl	(%rcx,%rdx,2), %edi
	movzwl	4(%r8), %edx
	cmpw	$-1, %dx
	setne	%al
	cmpw	%dx, %di
	setne	%bl
	andb	%bl, %al
	je	.L638
	leaq	controls(%rip), %rbp
	.p2align 6
	.p2align 4
	.p2align 3
.L639:
	imulq	$532, %rdx, %rdx
	leaq	0(%rbp,%rdx), %rsi
	movzwl	6(%rsi), %edx
	cmpw	$-1, %dx
	setne	%al
	cmpw	%dx, %di
	setne	%bl
	andb	%bl, %al
	jne	.L639
	cmpw	$-1, %dx
	je	.L636
	imulq	$532, %rdi, %rax
	movzwl	6(%rbp,%rax), %eax
	movw	%ax, 6(%rsi)
.L643:
	imulq	$532, %rdi, %rdi
	movl	$-1, %eax
	movl	$-1, %edx
	leal	1(%r10), %esi
	movl	%esi, %r14d
	addq	%rdi, %rbp
	movw	%ax, 2(%rbp)
	movw	%dx, 6(%rbp)
	movzwl	(%r8), %ebp
	movzwl	(%r11,%rbp,2), %edi
	movl	%edi, %r15d
	cmpl	%edi, %esi
	jge	.L648
	leal	1(%r9), %ebx
	movslq	%esi, %rdx
	movq	%rbp, %rsi
	movslq	%r10d, %rax
	movzwl	%bx, %ebx
	salq	$6, %rsi
	.p2align 5
	.p2align 4
	.p2align 3
.L647:
	addq	%rsi, %rdx
	addq	%rsi, %rax
	movzwl	(%rcx,%rdx,2), %edx
	movw	%dx, (%rcx,%rax,2)
	movslq	%ebx, %rax
	addl	$1, %ebx
	movslq	%ebx, %rdx
	cmpl	%edi, %ebx
	jl	.L647
.L648:
	leal	-1(%r15), %eax
	subl	$1, %edi
	movw	%ax, (%r11,%rbp,2)
	leaq	list_selection_mask(%rip), %r11
	xorl	%eax, %eax
	movl	%edi, 108(%r8)
	movq	(%r11,%rbp,8), %rdx
	testw	%r9w, %r9w
	je	.L646
	movq	$-1, %rax
	movl	%r10d, %ecx
	salq	%cl, %rax
	notq	%rax
	andq	%rdx, %rax
	cmpw	$62, %r9w
	ja	.L649
.L646:
	movl	%r14d, %ecx
	shrq	%cl, %rdx
	movl	%r10d, %ecx
	salq	%cl, %rdx
	orq	%rdx, %rax
.L649:
	movq	%rax, (%r11,%rbp,8)
	movl	80(%r8), %eax
	cmpl	%eax, %r10d
	je	.L672
	jge	.L651
	cmpl	$65535, %eax
	jne	.L673
.L651:
	orl	$8, 16(%r8)
	movl	$1, %eax
.L636:
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%rbp
	popq	%r14
	popq	%r15
	ret
	.p2align 4,,10
	.p2align 3
.L672:
	movl	$65535, 80(%r8)
	jmp	.L651
	.p2align 4,,10
	.p2align 3
.L673:
	subl	$1, %eax
	movl	%eax, 80(%r8)
	jmp	.L651
	.p2align 4,,10
	.p2align 3
.L638:
	cmpw	$-1, %dx
	je	.L636
	imulq	$532, %rdi, %rax
	leaq	controls(%rip), %rbp
	movzwl	6(%rbp,%rax), %eax
	movw	%ax, 4(%r8)
	jmp	.L643
	.seh_endproc
	.p2align 4
	.globl	nova_list_set_selection_mode
	.def	nova_list_set_selection_mode;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_list_set_selection_mode
nova_list_set_selection_mode:
	.seh_endprologue
	movq	%rcx, %rax
	testq	%rcx, %rcx
	je	.L679
	cmpl	$11, 8(%rcx)
	jne	.L679
	cmpl	$2, %edx
	jbe	.L683
.L679:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L683:
	movw	%dx, 124(%rcx)
	testl	%edx, %edx
	je	.L684
	cmpl	$1, %edx
	je	.L685
.L677:
	orl	$8, 16(%rax)
	movl	$1, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L684:
	movzwl	(%rcx), %ecx
	leaq	list_selection_mask(%rip), %rdx
	movl	$65535, 80(%rax)
	movq	$0, (%rdx,%rcx,8)
	jmp	.L677
	.p2align 4,,10
	.p2align 3
.L685:
	movl	80(%rcx), %ecx
	cmpl	$65535, %ecx
	je	.L677
	movzwl	(%rax), %r8d
	movl	$1, %edx
	salq	%cl, %rdx
	leaq	list_selection_mask(%rip), %rcx
	movq	%rdx, (%rcx,%r8,8)
	jmp	.L677
	.seh_endproc
	.p2align 4
	.globl	nova_list_select
	.def	nova_list_select;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_list_select
nova_list_select:
	pushq	%r14
	.seh_pushreg	%r14
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	.seh_endprologue
	xorl	%r8d, %r8d
	movq	%rcx, %rax
	testq	%rcx, %rcx
	je	.L686
	cmpl	$11, 8(%rcx)
	jne	.L686
	movzwl	(%rcx), %r9d
	leaq	list_entry_count(%rip), %rcx
	cmpw	(%rcx,%r9,2), %dx
	jnb	.L686
	movzwl	124(%rax), %r10d
	testw	%r10w, %r10w
	je	.L686
	movq	%r9, %r11
	movzwl	%dx, %ecx
	leaq	list_entries(%rip), %rbx
	movzwl	%dx, %edx
	salq	$6, %r11
	addq	%r11, %rdx
	movq	%r11, %rdi
	movzwl	(%rbx,%rdx,2), %edx
	leaq	controls(%rip), %rbx
	movq	%rdx, %rsi
	imulq	$532, %rdx, %rdx
	addq	%rbx, %rdx
	movl	16(%rdx), %r11d
	movl	%r11d, %ebx
	notl	%ebx
	andl	$3, %ebx
	jne	.L686
	movl	$1, %r8d
	salq	%cl, %r8
	cmpw	$1, %r10w
	je	.L700
	leaq	list_selection_mask(%rip), %rbx
	movq	%rbx, %r14
	orq	(%rbx,%r9,8), %r8
.L690:
	imulq	$532, %rsi, %rdx
	orl	$72, %r11d
	leaq	controls(%rip), %rbx
	movq	%r8, (%r14,%r9,8)
	movl	$1, %r8d
	movl	%r11d, 16(%rbx,%rdx)
	orl	$8, 16(%rax)
	movl	%ecx, 80(%rax)
.L686:
	movl	%r8d, %eax
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%r14
	ret
	.p2align 4,,10
	.p2align 3
.L700:
	movl	80(%rax), %r10d
	leaq	list_selection_mask(%rip), %r14
	cmpl	$65535, %r10d
	je	.L690
	movzwl	%r10w, %r10d
	leaq	list_entries(%rip), %rbx
	addq	%rdi, %r10
	movzwl	(%rbx,%r10,2), %r10d
	leaq	controls(%rip), %rbx
	imulq	$532, %r10, %r10
	movl	16(%rbx,%r10), %r11d
	andl	$-65, %r11d
	orl	$8, %r11d
	movl	%r11d, 16(%rbx,%r10)
	movl	16(%rdx), %r11d
	jmp	.L690
	.seh_endproc
	.p2align 4
	.globl	nova_list_selected_index
	.def	nova_list_selected_index;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_list_selected_index
nova_list_selected_index:
	.seh_endprologue
	movl	$-1, %eax
	testq	%rcx, %rcx
	je	.L701
	cmpl	$11, 8(%rcx)
	je	.L705
.L701:
	ret
	.p2align 4,,10
	.p2align 3
.L705:
	movzwl	80(%rcx), %eax
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_list_count
	.def	nova_list_count;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_list_count
nova_list_count:
	.seh_endprologue
	xorl	%eax, %eax
	testq	%rcx, %rcx
	je	.L706
	cmpl	$11, 8(%rcx)
	je	.L711
.L706:
	ret
	.p2align 4,,10
	.p2align 3
.L711:
	movzwl	(%rcx), %edx
	leaq	list_entry_count(%rip), %rax
	movzwl	(%rax,%rdx,2), %eax
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_list_set_virtual_window
	.def	nova_list_set_virtual_window;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_list_set_virtual_window
nova_list_set_virtual_window:
	.seh_endprologue
	xorl	%eax, %eax
	testq	%rcx, %rcx
	je	.L712
	cmpl	$11, 8(%rcx)
	jne	.L712
	movzwl	(%rcx), %r10d
	leaq	list_entry_count(%rip), %r9
	cmpw	%dx, (%r9,%r10,2)
	jb	.L712
	movzwl	%dx, %edx
	movl	$64, %r9d
	movzwl	%r8w, %r8d
	subl	%edx, %r9d
	cmpl	%r8d, %r9d
	jb	.L712
	sall	$16, %r8d
	orl	$8, 16(%rcx)
	pxor	%xmm1, %xmm1
	movl	$1, %eax
	orl	%edx, %r8d
	movd	%r8d, %xmm0
	punpcklwd	%xmm1, %xmm0
	movq	%xmm0, 84(%rcx)
.L712:
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_list_item_visible
	.def	nova_list_item_visible;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_list_item_visible
nova_list_item_visible:
	.seh_endprologue
	xorl	%eax, %eax
	testq	%rcx, %rcx
	je	.L718
	cmpl	$11, 8(%rcx)
	je	.L725
.L718:
	ret
	.p2align 4,,10
	.p2align 3
.L725:
	movzwl	(%rcx), %r9d
	leaq	list_entry_count(%rip), %r8
	cmpw	(%r8,%r9,2), %dx
	jnb	.L718
	movl	84(%rcx), %r8d
	cmpw	%r8w, %dx
	jb	.L718
	addw	88(%rcx), %r8w
	cmpw	%r8w, %dx
	setb	%al
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_list_item_set_subtitle
	.def	nova_list_item_set_subtitle;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_list_item_set_subtitle
nova_list_item_set_subtitle:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L732
	cmpl	$12, 8(%rcx)
	jne	.L732
	xorl	%eax, %eax
	testq	%rdx, %rdx
	jne	.L728
.L732:
	xorl	%eax, %eax
	ret
	.p2align 5
	.p2align 4,,10
	.p2align 3
.L730:
	movb	%r8b, 325(%rcx,%rax)
	addq	$1, %rax
	cmpq	$95, %rax
	je	.L729
.L728:
	movzbl	(%rdx,%rax), %r8d
	testb	%r8b, %r8b
	jne	.L730
.L729:
	movb	$0, 325(%rcx,%rax)
	movl	$1, %eax
	orl	$8, 16(%rcx)
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_list_item_set_status
	.def	nova_list_item_set_status;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_list_item_set_status
nova_list_item_set_status:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L742
	cmpl	$12, 8(%rcx)
	jne	.L742
	xorl	%eax, %eax
	testq	%rdx, %rdx
	jne	.L738
.L742:
	xorl	%eax, %eax
	ret
	.p2align 5
	.p2align 4,,10
	.p2align 3
.L740:
	movb	%r8b, 421(%rcx,%rax)
	addq	$1, %rax
	cmpq	$95, %rax
	je	.L739
.L738:
	movzbl	(%rdx,%rax), %r8d
	testb	%r8b, %r8b
	jne	.L740
.L739:
	movb	$0, 421(%rcx,%rax)
	movl	$1, %eax
	orl	$8, 16(%rcx)
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_list_item_set_state
	.def	nova_list_item_set_state;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_list_item_set_state
nova_list_item_set_state:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L755
	cmpl	$12, 8(%rcx)
	jne	.L755
	cmpl	$5, %edx
	jbe	.L756
.L755:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L756:
	movl	16(%rcx), %r8d
	movl	%r8d, %eax
	andl	$-8277, %eax
	cmpl	$1, %edx
	je	.L757
	cmpl	$2, %edx
	je	.L758
	cmpl	$3, %edx
	je	.L759
	cmpl	$5, %edx
	je	.L760
	cmpl	$4, %edx
	jne	.L749
	movl	%r8d, %eax
	andl	$-8279, %eax
	jmp	.L753
	.p2align 4,,10
	.p2align 3
.L757:
	orl	$16, %eax
.L749:
	orl	$2, %eax
.L753:
	orl	$8, %eax
	movw	%dx, 124(%rcx)
	movl	%eax, 16(%rcx)
	movl	$1, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L758:
	orl	$4, %eax
	jmp	.L749
	.p2align 4,,10
	.p2align 3
.L759:
	orl	$64, %eax
	jmp	.L749
	.p2align 4,,10
	.p2align 3
.L760:
	orb	$32, %ah
	jmp	.L749
	.seh_endproc
	.p2align 4
	.globl	nova_status_badge_set_type
	.def	nova_status_badge_set_type;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_status_badge_set_type
nova_status_badge_set_type:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L764
	cmpl	$19, 8(%rcx)
	jne	.L764
	cmpl	$6, %edx
	jbe	.L765
.L764:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L765:
	orl	$8, 16(%rcx)
	movl	$1, %eax
	movw	%dx, 124(%rcx)
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_status_badge_set_icon
	.def	nova_status_badge_set_icon;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_status_badge_set_icon
nova_status_badge_set_icon:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L769
	cmpl	$19, 8(%rcx)
	jne	.L769
	cmpw	$14, %dx
	jbe	.L770
.L769:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L770:
	orl	$8, 16(%rcx)
	movzwl	%dx, %edx
	movl	$1, %eax
	movl	%edx, 84(%rcx)
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_status_badge_set_visible
	.def	nova_status_badge_set_visible;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_status_badge_set_visible
nova_status_badge_set_visible:
	.seh_endprologue
	xorl	%eax, %eax
	testq	%rcx, %rcx
	je	.L771
	cmpl	$19, 8(%rcx)
	je	.L777
.L771:
	ret
	.p2align 4,,10
	.p2align 3
.L777:
	movl	16(%rcx), %eax
	movl	%eax, %r8d
	andl	$-2, %eax
	orl	$1, %r8d
	testb	%dl, %dl
	cmovne	%r8d, %eax
	orl	$8, %eax
	movl	%eax, 16(%rcx)
	movl	$1, %eax
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_style_define
	.def	nova_style_define;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_style_define
nova_style_define:
	pushq	%r15
	.seh_pushreg	%r15
	pushq	%r14
	.seh_pushreg	%r14
	pushq	%r12
	.seh_pushreg	%r12
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	.seh_endprologue
	cmpw	$31, %cx
	movl	%edx, %r10d
	seta	%al
	testq	%r8, %r8
	sete	%dl
	orb	%dl, %al
	jne	.L792
	cmpl	$1023, %r9d
	seta	%dl
	cmpl	$3, 96(%rsp)
	seta	%r11b
	orl	%r11d, %edx
	cmpw	%r10w, %cx
	sete	%r11b
	orb	%r11b, %dl
	jne	.L778
	cmpw	$-1, %r10w
	je	.L780
	cmpw	$31, %r10w
	ja	.L778
	movzwl	%r10w, %eax
	leaq	styles(%rip), %rdx
	imulq	$60, %rax, %rbx
	leaq	(%rdx,%rbx), %r11
	movzbl	56(%r11), %eax
	testb	%al, %al
	je	.L778
	movd	12(%r11), %xmm0
	movd	16(%r11), %xmm3
	leaq	16(%rdx), %r12
	movd	20(%r11), %xmm1
	movd	24(%r11), %xmm4
	movl	28(%r11), %esi
	movl	32(%r11), %edi
	movl	36(%r11), %r14d
	movl	40(%r11), %r15d
	movl	44(%r11), %ebp
	movd	48(%rdx,%rbx), %xmm2
	movzwl	52(%r11), %r11d
.L781:
	testb	$1, %r9b
	je	.L782
	movd	(%r8), %xmm0
.L782:
	testb	$2, %r9b
	je	.L783
	movd	4(%r8), %xmm3
.L783:
	testb	$4, %r9b
	je	.L784
	movd	8(%r8), %xmm1
.L784:
	testb	$8, %r9b
	je	.L785
	movd	12(%r8), %xmm4
.L785:
	punpckldq	%xmm4, %xmm1
	punpckldq	%xmm3, %xmm0
	punpcklqdq	%xmm1, %xmm0
	testb	$16, %r9b
	je	.L786
	movl	16(%r8), %esi
.L786:
	testb	$32, %r9b
	je	.L787
	movl	20(%r8), %edi
.L787:
	testb	$64, %r9b
	je	.L788
	movl	24(%r8), %r14d
.L788:
	testb	$-128, %r9b
	je	.L789
	movl	28(%r8), %r15d
.L789:
	testl	$256, %r9d
	je	.L790
	movl	32(%r8), %ebp
.L790:
	testl	$512, %r9d
	je	.L791
	movd	36(%r8), %xmm2
	movzwl	40(%r8), %r11d
.L791:
	movzwl	%cx, %r8d
	movl	$1, %eax
	imulq	$60, %r8, %r8
	movw	%cx, (%rdx,%r8)
	movl	96(%rsp), %ecx
	movw	%r10w, 2(%rdx,%r8)
	movl	%r9d, 4(%rdx,%r8)
	movl	%ecx, 8(%rdx,%r8)
	movl	%esi, 28(%rdx,%r8)
	movl	%edi, 32(%rdx,%r8)
	movl	%r14d, 36(%rdx,%r8)
	movl	%r15d, 40(%rdx,%r8)
	movl	%ebp, 44(%rdx,%r8)
	movups	%xmm0, 12(%rdx,%r8)
	movd	%xmm2, 32(%r12,%r8)
	movw	%r11w, 52(%rdx,%r8)
	movb	$1, 56(%rdx,%r8)
.L778:
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%rbp
	popq	%r12
	popq	%r14
	popq	%r15
	ret
	.p2align 4,,10
	.p2align 3
.L792:
	xorl	%eax, %eax
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%rbp
	popq	%r12
	popq	%r14
	popq	%r15
	ret
	.p2align 4,,10
	.p2align 3
.L780:
	leaq	styles(%rip), %rdx
	movd	default_style(%rip), %xmm0
	movd	4+default_style(%rip), %xmm3
	movl	16+default_style(%rip), %esi
	movl	20+default_style(%rip), %edi
	leaq	16(%rdx), %r12
	movd	8+default_style(%rip), %xmm1
	movd	12+default_style(%rip), %xmm4
	movl	24+default_style(%rip), %r14d
	movl	28+default_style(%rip), %r15d
	movl	32+default_style(%rip), %ebp
	movd	36+default_style(%rip), %xmm2
	movzwl	40+default_style(%rip), %r11d
	jmp	.L781
	.seh_endproc
	.p2align 4
	.globl	nova_style_get
	.def	nova_style_get;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_style_get
nova_style_get:
	.seh_endprologue
	xorl	%edx, %edx
	cmpw	$31, %cx
	ja	.L827
	movzwl	%cx, %eax
	leaq	styles(%rip), %r8
	imulq	$60, %rax, %rax
	cmpb	$0, 56(%r8,%rax)
	je	.L827
	leaq	(%r8,%rax), %rdx
.L827:
	movq	%rdx, %rax
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_style_apply
	.def	nova_style_apply;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_style_apply
nova_style_apply:
	.seh_endprologue
	movl	%edx, %r8d
	xorl	%edx, %edx
	cmpw	$31, %r8w
	ja	.L831
	movzwl	%r8w, %edx
	leaq	styles(%rip), %rax
	imulq	$60, %rdx, %rdx
	addq	%rdx, %rax
	movzbl	56(%rax), %edx
	testb	%dl, %dl
	je	.L831
	testq	%rcx, %rcx
	je	.L834
	movdqu	12(%rax), %xmm0
	movups	%xmm0, 36(%rcx)
	movdqu	28(%rax), %xmm1
	movups	%xmm1, 52(%rcx)
	movq	40(%rax), %r10
	movq	48(%rax), %r11
	orl	$8, 16(%rcx)
	movq	%r10, 64(%rcx)
	movq	%r11, 72(%rcx)
	movw	%r8w, 128(%rcx)
.L831:
	movl	%edx, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L834:
	xorl	%edx, %edx
	jmp	.L831
	.seh_endproc
	.p2align 4
	.globl	nova_style_theme_changed
	.def	nova_style_theme_changed;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_style_theme_changed
nova_style_theme_changed:
	.seh_endprologue
	leaq	16+controls(%rip), %rax
	leaq	used(%rip), %rcx
	leaq	68096(%rax), %r8
	leaq	styles(%rip), %r9
	.p2align 4
	.p2align 3
.L840:
	cmpb	$0, (%rcx)
	je	.L839
	movzwl	112(%rax), %edx
	cmpw	$31, %dx
	ja	.L839
	imulq	$60, %rdx, %rdx
	addq	%r9, %rdx
	cmpb	$0, 56(%rdx)
	je	.L839
	orl	$8, (%rax)
	movdqu	12(%rdx), %xmm0
	movdqu	28(%rdx), %xmm1
	movdqu	40(%rdx), %xmm2
	movups	%xmm0, 20(%rax)
	movups	%xmm1, 36(%rax)
	movups	%xmm2, 48(%rax)
	.p2align 4
	.p2align 3
.L839:
	addq	$532, %rax
	addq	$1, %rcx
	cmpq	%rax, %r8
	jne	.L840
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_control_template_define
	.def	nova_control_template_define;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_control_template_define
nova_control_template_define:
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	.seh_endprologue
	xorl	%eax, %eax
	movl	72(%rsp), %r11d
	movl	%edx, %r10d
	leal	-1(%rcx), %edx
	cmpw	$62, %dx
	ja	.L842
	cmpl	$23, %r8d
	seta	%al
	testw	%r11w, %r11w
	sete	%dl
	orl	%edx, %eax
	cmpw	%r10w, %cx
	sete	%dl
	orb	%dl, %al
	jne	.L848
	cmpw	$63, %r10w
	jbe	.L854
	movl	%r9d, %edx
	notl	%edx
	testl	%edx, 64(%rsp)
	jne	.L842
	cmpw	$-1, %r10w
	jne	.L842
	movl	%r9d, %edi
	leaq	templates(%rip), %rsi
.L846:
	movl	64(%rsp), %ebx
	movzwl	%cx, %eax
	leaq	(%rax,%rax,2), %rax
	movw	%cx, (%rsi,%rax,8)
	movw	%r10w, 2(%rsi,%rax,8)
	movl	%r8d, 4(%rsi,%rax,8)
	movl	%r9d, 8(%rsi,%rax,8)
	movl	%edi, 12(%rsi,%rax,8)
	movl	%ebx, 16(%rsi,%rax,8)
	movw	%r11w, 20(%rsi,%rax,8)
	movb	$1, 22(%rsi,%rax,8)
	movl	$1, %eax
.L842:
	popq	%rbx
	popq	%rsi
	popq	%rdi
	ret
	.p2align 4,,10
	.p2align 3
.L848:
	xorl	%eax, %eax
	popq	%rbx
	popq	%rsi
	popq	%rdi
	ret
	.p2align 4,,10
	.p2align 3
.L854:
	movzwl	%r10w, %edx
	leaq	templates(%rip), %rbx
	leaq	(%rdx,%rdx,2), %rdx
	movq	%rbx, %rsi
	leaq	(%rbx,%rdx,8), %rdx
	cmpb	$0, 22(%rdx)
	je	.L842
	movl	12(%rdx), %ebx
	orl	%r9d, %ebx
	movl	%ebx, %edi
	notl	%ebx
	testl	%ebx, 64(%rsp)
	jne	.L842
	cmpl	%r8d, 4(%rdx)
	je	.L846
	jmp	.L842
	.seh_endproc
	.p2align 4
	.globl	nova_controls_initialize
	.def	nova_controls_initialize;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_controls_initialize
nova_controls_initialize:
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	xorl	%eax, %eax
	leaq	used(%rip), %rdi
	movq	%rcx, %rdx
	movl	$16, %ecx
	rep stosq
	movl	$240, %ecx
	leaq	styles(%rip), %rdi
	rep stosq
	movl	$192, %ecx
	leaq	templates(%rip), %rdi
	rep stosq
	testq	%rdx, %rdx
	je	.L856
	movdqu	(%rdx), %xmm1
	movups	%xmm1, default_style(%rip)
	movdqu	16(%rdx), %xmm2
	movups	%xmm2, 16+default_style(%rip)
	movdqu	28(%rdx), %xmm3
	movups	%xmm3, 28+default_style(%rip)
.L857:
	movl	$0, 32(%rsp)
	pxor	%xmm0, %xmm0
	xorl	%ecx, %ecx
	xorl	%ebx, %ebx
	movl	$-1, %eax
	movl	$1023, %r9d
	leaq	default_style(%rip), %r8
	movl	$65535, %edx
	movw	%ax, focused_id(%rip)
	leaq	CSWTCH.45(%rip), %rsi
	movl	$0, 32+diagnostics(%rip)
	movups	%xmm0, diagnostics(%rip)
	movups	%xmm0, 16+diagnostics(%rip)
	call	nova_style_define
	.p2align 4
	.p2align 3
.L861:
	cmpq	$20, %rbx
	ja	.L858
.L862:
	movl	(%rsi,%rbx,4), %r9d
	leal	1(%rbx), %ecx
	movl	%ebx, %r8d
	movl	$65535, %edx
	movl	$4, 40(%rsp)
	addq	$1, %rbx
	movl	%r9d, 32(%rsp)
	call	nova_control_template_define
	cmpq	$20, %rbx
	jbe	.L862
.L858:
	movl	$4, 40(%rsp)
	leal	1(%rbx), %ecx
	movl	%ebx, %r8d
	movl	$17, %r9d
	movl	$17, 32(%rsp)
	movl	$65535, %edx
	addq	$1, %rbx
	call	nova_control_template_define
	cmpq	$24, %rbx
	jne	.L861
	addq	$48, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	ret
.L856:
	movdqu	.LC1(%rip), %xmm0
	movl	$8, %edx
	movabsq	$281505041436516, %rax
	movq	%rax, 32+default_style(%rip)
	movups	%xmm0, default_style(%rip)
	movdqu	.LC2(%rip), %xmm0
	movw	%dx, 40+default_style(%rip)
	movups	%xmm0, 16+default_style(%rip)
	jmp	.L857
	.seh_endproc
	.p2align 4
	.globl	nova_control_template_get
	.def	nova_control_template_get;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_control_template_get
nova_control_template_get:
	.seh_endprologue
	xorl	%eax, %eax
	cmpw	$63, %cx
	ja	.L863
	movzwl	%cx, %edx
	leaq	templates(%rip), %r8
	leaq	(%rdx,%rdx,2), %rdx
	cmpb	$0, 22(%r8,%rdx,8)
	je	.L863
	leaq	(%r8,%rdx,8), %rax
.L863:
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_control_template_apply
	.def	nova_control_template_apply;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_control_template_apply
nova_control_template_apply:
	.seh_endprologue
	xorl	%eax, %eax
	cmpw	$63, %dx
	ja	.L867
	movzwl	%dx, %eax
	leaq	(%rax,%rax,2), %r8
	leaq	templates(%rip), %rax
	leaq	(%rax,%r8,8), %r8
	movzbl	22(%r8), %eax
	testb	%al, %al
	je	.L867
	testq	%rcx, %rcx
	je	.L872
	movl	8(%rcx), %r10d
	cmpl	%r10d, 4(%r8)
	jne	.L872
	movl	12(%r8), %r9d
	notl	%r9d
	andl	16(%r8), %r9d
	jne	.L872
	orl	$8, 16(%rcx)
	movw	%dx, 126(%rcx)
.L867:
	ret
	.p2align 4,,10
	.p2align 3
.L872:
	xorl	%eax, %eax
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_control_create
	.def	nova_control_create;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_control_create
nova_control_create:
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movl	%ecx, %r10d
	cmpl	$23, %ecx
	ja	.L877
	xorl	%edx, %edx
	leaq	used(%rip), %r11
	jmp	.L889
	.p2align 5
	.p2align 4,,10
	.p2align 3
.L878:
	addq	$1, %rdx
	cmpq	$128, %rdx
	je	.L877
.L889:
	cmpb	$0, (%r11,%rdx)
	jne	.L878
	movslq	%edx, %rbx
	leaq	controls(%rip), %r9
	xorl	%eax, %eax
	movslq	%edx, %rsi
	movb	$1, (%r11,%rbx)
	imulq	$532, %rbx, %rbx
	movd	%r10d, %xmm0
	leaq	(%r9,%rbx), %r8
	leaq	8(%r8), %rdi
	movq	%r8, %rcx
	movq	$0, (%r8)
	movq	$0, 524(%r8)
	andq	$-8, %rdi
	subq	%rdi, %rcx
	addl	$532, %ecx
	shrl	$3, %ecx
	rep stosq
	movq	.LC7(%rip), %rax
	movl	$-1, %edi
	movw	%dx, (%r8)
	movl	$-1, 4(%r8)
	leal	1(%r10), %edx
	movw	%di, 2(%r8)
	movq	%xmm0, 8(%r9,%rbx)
	movdqu	default_style(%rip), %xmm1
	movdqu	16+default_style(%rip), %xmm2
	movdqu	28+default_style(%rip), %xmm3
	movl	$10, 16(%r8)
	movq	%rax, 88(%r9,%rbx)
	xorl	%eax, %eax
	movups	%xmm2, 52(%r9,%rbx)
	movw	%ax, 128(%r8)
	movw	%dx, 126(%r8)
	movups	%xmm1, 36(%r9,%rbx)
	movups	%xmm3, 64(%r9,%rbx)
	testl	%r10d, %r10d
	je	.L891
	cmpl	$8, %r10d
	jne	.L881
	xorl	%eax, %eax
	movw	%ax, 124(%r8)
	movl	$95, %eax
	movw	%ax, 526(%r8)
.L880:
	xorl	%eax, %eax
.L882:
	imulq	$532, %rsi, %rcx
	movzwl	%dx, %edx
	addq	%r9, %rcx
	movl	%eax, 528(%rcx)
	call	nova_control_template_apply
	testb	%al, %al
	je	.L892
	addw	$1, 4+diagnostics(%rip)
	movq	%rcx, %rax
	addw	$1, diagnostics(%rip)
	addq	$32, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	ret
	.p2align 4,,10
	.p2align 3
.L891:
	xorl	%ecx, %ecx
	movl	$95, %r10d
	movl	$-1, 84(%r8)
	movw	%cx, 124(%r8)
	movw	%r10w, 526(%r8)
	jmp	.L880
	.p2align 4,,10
	.p2align 3
.L892:
	movb	$0, (%r11,%rsi)
.L877:
	xorl	%ecx, %ecx
	movq	%rcx, %rax
	addq	$32, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	ret
.L881:
	cmpl	$10, %r10d
	jne	.L883
	movl	$2, %eax
	movl	$0, 80(%r8)
	movw	%ax, 124(%r8)
	movl	$95, %eax
	movw	%ax, 526(%r8)
	jmp	.L880
.L883:
	cmpl	$20, %r10d
	je	.L893
	cmpl	$11, %r10d
	je	.L894
	cmpl	$19, %r10d
	jne	.L886
	xorl	%eax, %eax
	movl	$95, %ecx
	movl	$-1, 84(%r8)
	movw	%ax, 124(%r8)
	movl	$16392, 16(%r8)
	movw	%cx, 526(%r8)
	jmp	.L880
.L893:
	xorl	%edi, %edi
	movl	$95, %eax
	movl	$520, 16(%r8)
	movw	%di, 124(%r8)
	movw	%ax, 526(%r8)
	jmp	.L880
.L894:
	movl	$1, %r10d
	movw	%r10w, 124(%r8)
	movq	$65535, 80(%r9,%rbx)
	movl	$95, %ebx
	movl	$64, 88(%r8)
	movw	%bx, 526(%r8)
	jmp	.L880
.L886:
	movw	%r10w, 124(%r8)
	movw	$95, 526(%r8)
	cmpl	$7, %r10d
	jne	.L880
	movl	$266, 16(%r8)
	movl	$3, %eax
	jmp	.L882
	.seh_endproc
	.section .rdata,"dr"
.LC9:
	.ascii "Background\0"
.LC10:
	.ascii "Border\0"
.LC11:
	.ascii "Icon\0"
.LC12:
	.ascii "Text\0"
.LC13:
	.ascii "ContentPresenter\0"
.LC14:
	.ascii "FocusIndicator\0"
.LC15:
	.ascii "Track\0"
.LC16:
	.ascii "Thumb\0"
.LC17:
	.ascii "Status\0"
.LC18:
	.ascii "Image\0"
	.text
	.p2align 4
	.globl	nova_control_template_has_part
	.def	nova_control_template_has_part;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_control_template_has_part
nova_control_template_has_part:
	subq	$40, %rsp
	.seh_stackalloc	40
	.seh_endprologue
	movq	%rcx, %r10
	movq	%rdx, %r11
	testq	%rcx, %rcx
	je	.L924
	cmpb	$1, 22(%rcx)
	jne	.L924
	testq	%rdx, %rdx
	je	.L924
	leaq	10(%rdx), %r9
	movq	%rdx, %rax
	movl	$66, %r8d
	leaq	.LC9(%rip), %rcx
	jmp	.L897
	.p2align 4,,10
	.p2align 3
.L956:
	cmpq	%r9, %rax
	je	.L899
	cmpb	%r8b, %dl
	jne	.L898
	movzbl	1(%rcx), %r8d
	addq	$1, %rcx
	addq	$1, %rax
.L897:
	movzbl	(%rax), %edx
	testb	%dl, %dl
	jne	.L956
.L899:
	movl	$1, %eax
	cmpb	%r8b, %dl
	jne	.L898
.L901:
	andl	12(%r10), %eax
	setne	%al
	addq	$40, %rsp
	ret
	.p2align 4,,10
	.p2align 3
.L924:
	xorl	%eax, %eax
.L895:
	addq	$40, %rsp
	ret
	.p2align 4,,10
	.p2align 3
.L898:
	leaq	6(%r11), %r9
	movq	%r11, %rax
	leaq	.LC10(%rip), %rdx
	jmp	.L903
	.p2align 4,,10
	.p2align 3
.L957:
	cmpq	%r9, %rax
	je	.L904
	addq	$1, %rax
	addq	$1, %rdx
	cmpb	%r8b, %cl
	jne	.L902
.L903:
	movzbl	(%rax), %ecx
	movzbl	(%rdx), %r8d
	testb	%cl, %cl
	jne	.L957
.L904:
	movl	$2, %eax
	cmpb	%cl, %r8b
	je	.L901
.L902:
	leaq	4(%r11), %r9
	movq	%r11, %rax
	movl	$73, %r8d
	leaq	.LC11(%rip), %rcx
	jmp	.L906
	.p2align 4,,10
	.p2align 3
.L958:
	cmpq	%r9, %rax
	je	.L908
	cmpb	%dl, %r8b
	jne	.L927
	movzbl	1(%rcx), %r8d
	addq	$1, %rcx
	addq	$1, %rax
.L906:
	movzbl	(%rax), %edx
	testb	%dl, %dl
	jne	.L958
.L908:
	movl	$4, %eax
	cmpb	%dl, %r8b
	je	.L901
.L927:
	movq	%r11, %rax
	movl	$84, %r8d
	leaq	.LC12(%rip), %rcx
	jmp	.L911
	.p2align 4,,10
	.p2align 3
.L959:
	cmpq	%r9, %rax
	je	.L912
	cmpb	%r8b, %dl
	jne	.L910
	movzbl	1(%rcx), %r8d
	addq	$1, %rcx
	addq	$1, %rax
.L911:
	movzbl	(%rax), %edx
	testb	%dl, %dl
	jne	.L959
.L912:
	movl	$8, %eax
	cmpb	%dl, %r8b
	je	.L901
.L910:
	leaq	16(%r11), %r9
	movq	%r11, %rax
	movl	$67, %r8d
	leaq	.LC13(%rip), %rcx
	jmp	.L914
	.p2align 4,,10
	.p2align 3
.L960:
	cmpq	%rax, %r9
	je	.L916
	cmpb	%dl, %r8b
	jne	.L915
	movzbl	1(%rcx), %r8d
	addq	$1, %rcx
	addq	$1, %rax
.L914:
	movzbl	(%rax), %edx
	testb	%dl, %dl
	jne	.L960
.L916:
	movl	$16, %eax
	cmpb	%dl, %r8b
	je	.L901
.L915:
	leaq	14(%r11), %r9
	movq	%r11, %rax
	movl	$70, %r8d
	leaq	.LC14(%rip), %rcx
	jmp	.L918
.L961:
	cmpq	%rax, %r9
	je	.L920
	cmpb	%dl, %r8b
	jne	.L919
	movzbl	1(%rcx), %r8d
	addq	$1, %rcx
	addq	$1, %rax
.L918:
	movzbl	(%rax), %edx
	testb	%dl, %dl
	jne	.L961
.L920:
	movl	$32, %eax
	cmpb	%r8b, %dl
	je	.L901
.L919:
	leaq	.LC15(%rip), %rdx
	movq	%r11, %rcx
	call	text_equal
	testb	%al, %al
	movl	$64, %eax
	jne	.L901
	leaq	.LC16(%rip), %rdx
	call	text_equal
	testb	%al, %al
	movl	$128, %eax
	jne	.L901
	leaq	.LC17(%rip), %rdx
	call	text_equal
	testb	%al, %al
	movl	$256, %eax
	jne	.L901
	leaq	.LC18(%rip), %rdx
	call	text_equal
	testb	%al, %al
	je	.L895
	movl	$512, %eax
	jmp	.L901
	.seh_endproc
	.p2align 4
	.globl	nova_control_set_style
	.def	nova_control_set_style;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_control_set_style
nova_control_set_style:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L966
	testq	%rdx, %rdx
	je	.L966
	movdqu	(%rdx), %xmm0
	movups	%xmm0, 36(%rcx)
	movdqu	16(%rdx), %xmm1
	movups	%xmm1, 52(%rcx)
	movq	28(%rdx), %rax
	movq	36(%rdx), %rdx
	movq	%rax, 64(%rcx)
	movl	$-1, %eax
	movw	%ax, 128(%rcx)
	movzwl	(%rcx), %eax
	movq	%rdx, 72(%rcx)
	leaq	used(%rip), %rdx
	movzbl	(%rdx,%rax), %eax
	testb	%al, %al
	je	.L966
	cmpl	$5, 12(%rcx)
	je	.L966
	orl	$8, 16(%rcx)
	ret
	.p2align 4,,10
	.p2align 3
.L966:
	xorl	%eax, %eax
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_control_invalidate
	.def	nova_control_invalidate;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_control_invalidate
nova_control_invalidate:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L973
	movzwl	(%rcx), %eax
	leaq	used(%rip), %rdx
	movzbl	(%rdx,%rax), %eax
	testb	%al, %al
	je	.L970
	cmpl	$5, 12(%rcx)
	je	.L973
	orl	$8, 16(%rcx)
.L970:
	ret
	.p2align 4,,10
	.p2align 3
.L973:
	xorl	%eax, %eax
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_control_focus
	.def	nova_control_focus;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_control_focus
nova_control_focus:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L980
	movl	16(%rcx), %eax
	xorl	%edx, %edx
	movl	%eax, %r8d
	notl	%r8d
	andl	$3, %r8d
	je	.L985
	movl	%edx, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L985:
	movzwl	focused_id(%rip), %edx
	cmpw	$-1, %dx
	je	.L979
	leaq	used(%rip), %r8
	cmpb	$0, (%r8,%rdx)
	je	.L979
	imulq	$532, %rdx, %rdx
	leaq	controls(%rip), %rax
	andl	$-5, 16(%rax,%rdx)
	movl	16(%rcx), %eax
.L979:
	movzwl	(%rcx), %edx
	orl	$12, %eax
	movl	%eax, 16(%rcx)
	movw	%dx, focused_id(%rip)
	movw	%dx, 2+diagnostics(%rip)
	movl	$1, %edx
	movl	%edx, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L980:
	xorl	%edx, %edx
	movl	%edx, %eax
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_control_hit_test
	.def	nova_control_hit_test;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_control_hit_test
nova_control_hit_test:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	.seh_endprologue
	leaq	used(%rip), %rbp
	leaq	controls(%rip), %rdi
	addl	$1, 16+diagnostics(%rip)
	leaq	-128(%rbp), %r9
	movq	%rbp, %rax
	movl	%ecx, %ebx
	movl	%edx, %esi
	movq	%rdi, %rcx
	jmp	.L989
	.p2align 5
	.p2align 4,,10
	.p2align 3
.L987:
	subq	$1, %rax
	subq	$532, %rcx
	cmpq	%r9, %rax
	je	.L998
.L989:
	cmpb	$0, 127(%rax)
	je	.L987
	movl	67580(%rcx), %r8d
	movl	67584(%rcx), %r10d
	movl	67588(%rcx), %r11d
	notl	%r8d
	andl	$3, %r8d
	jne	.L987
	cmpl	%r10d, %ebx
	setge	%dl
	cmpl	%esi, %r11d
	setle	%r8b
	testb	%r8b, %dl
	je	.L987
	addl	67592(%rcx), %r10d
	cmpl	%ebx, %r10d
	jle	.L987
	addl	67596(%rcx), %r11d
	cmpl	%esi, %r11d
	jle	.L987
	subl	%ebp, %eax
	addl	$127, %eax
	cltq
	imulq	$532, %rax, %rax
	addq	%rdi, %rax
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%rbp
	ret
.L998:
	xorl	%eax, %eax
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%rbp
	ret
	.seh_endproc
	.p2align 4
	.globl	nova_control_invoke
	.def	nova_control_invoke;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_control_invoke
nova_control_invoke:
	subq	$40, %rsp
	.seh_stackalloc	40
	.seh_endprologue
	movq	%rdx, %r9
	testq	%rcx, %rcx
	je	.L1000
	movl	16(%rcx), %edx
	movl	%edx, %eax
	andl	$53250, %eax
	cmpl	$2, %eax
	je	.L1010
.L1000:
	addl	$1, 12+diagnostics(%rip)
	xorl	%eax, %eax
	addq	$40, %rsp
	ret
	.p2align 4,,10
	.p2align 3
.L1010:
	cmpb	$0, 132(%rcx)
	jne	.L1000
	movl	8(%rcx), %eax
	leal	-3(%rax), %r8d
	cmpl	$1, %r8d
	jbe	.L1003
	testl	%eax, %eax
	jne	.L1004
	cmpw	$6, 124(%rcx)
	je	.L1003
.L1004:
	orl	$32, %edx
	movb	$1, 132(%rcx)
	movl	%edx, 16(%rcx)
	testq	%r9, %r9
	je	.L1006
	movl	120(%rcx), %eax
	movl	%eax, (%r9)
.L1006:
	movl	$1, %eax
	addq	$40, %rsp
	ret
	.p2align 4,,10
	.p2align 3
.L1003:
	shrl	$10, %edx
	andl	$1, %edx
	xorl	$1, %edx
	call	nova_control_set_checked
	movl	16(%rcx), %edx
	jmp	.L1004
	.seh_endproc
	.p2align 4
	.globl	nova_control_release
	.def	nova_control_release;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_control_release
nova_control_release:
	.seh_endprologue
	testq	%rcx, %rcx
	je	.L1011
	andl	$-33, 16(%rcx)
	movb	$0, 132(%rcx)
.L1011:
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC19:
	.ascii "Kritisch\0"
	.text
	.p2align 4
	.globl	nova_control_render
	.def	nova_control_render;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_control_render
nova_control_render:
	pushq	%r15
	.seh_pushreg	%r15
	pushq	%r14
	.seh_pushreg	%r14
	pushq	%r13
	.seh_pushreg	%r13
	pushq	%r12
	.seh_pushreg	%r12
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$312, %rsp
	.seh_stackalloc	312
	.seh_endprologue
	movq	%rcx, %rbx
	movq	%rdx, %rsi
	testq	%rcx, %rcx
	je	.L1016
	testq	%rdx, %rdx
	je	.L1016
	movl	16(%rcx), %ecx
	movl	%ecx, %eax
	andl	$1, %eax
	movl	%eax, %r13d
	je	.L1016
	testb	$2, %cl
	je	.L1018
	movl	36(%rbx), %eax
	movl	%eax, 88(%rsp)
.L1019:
	movl	24(%rbx), %eax
	movzwl	74(%rbx), %r11d
	movslq	88(%rbx), %r8
	movl	84(%rbx), %edx
	testw	%r11w, %r11w
	movl	20(%rbx), %r12d
	movl	28(%rbx), %edi
	movl	%eax, 80(%rsp)
	movzwl	72(%rbx), %eax
	movl	%r8d, %r9d
	movl	32(%rbx), %ebp
	movl	%eax, 96(%rsp)
	movl	%eax, %r10d
	movl	$1, %eax
	cmove	%eax, %r11d
	subl	%edx, %r9d
	movzwl	%r11w, %eax
	movl	%eax, 104(%rsp)
	xorl	%eax, %eax
	testl	%r9d, %r9d
	jle	.L1020
	movl	80(%rbx), %eax
	movslq	%r9d, %r9
	subl	%edx, %eax
	cltq
	imulq	$1000, %rax, %rax
	cqto
	idivq	%r9
.L1020:
	cmpl	$20, 8(%rbx)
	ja	.L1021
	movl	8(%rbx), %edx
	leaq	.L1023(%rip), %r9
	movslq	(%r9,%rdx,4), %rdx
	addq	%r9, %rdx
	jmp	*%rdx
	.section .rdata,"dr"
	.align 4
.L1023:
	.long	.L1039-.L1023
	.long	.L1038-.L1023
	.long	.L1037-.L1023
	.long	.L1036-.L1023
	.long	.L1035-.L1023
	.long	.L1028-.L1023
	.long	.L1034-.L1023
	.long	.L1034-.L1023
	.long	.L1033-.L1023
	.long	.L1032-.L1023
	.long	.L1031-.L1023
	.long	.L1021-.L1023
	.long	.L1030-.L1023
	.long	.L1021-.L1023
	.long	.L1029-.L1023
	.long	.L1028-.L1023
	.long	.L1027-.L1023
	.long	.L1026-.L1023
	.long	.L1025-.L1023
	.long	.L1024-.L1023
	.long	.L1022-.L1023
	.text
	.p2align 4,,10
	.p2align 3
.L1034:
	movl	80(%rsp), %r15d
	movl	%r12d, %eax
	xorl	%edx, %edx
	movl	88(%rsp), %r9d
	movl	%eax, %ecx
	movq	%rax, 128(%rsp)
	movq	%r15, %r8
	movq	%r15, 120(%rsp)
	movl	%edi, %r15d
	salq	$32, %r8
	movq	%r15, 144(%rsp)
	orq	%r8, %rcx
	movabsq	$-4294967296, %r8
	movq	%rcx, %rax
	movq	%rdx, %rcx
	andq	%r8, %rcx
	movq	%rbp, %r8
	movq	%rax, 176(%rsp)
	leaq	176(%rsp), %rax
	orq	%r15, %rcx
	salq	$32, %r8
	movq	%rax, %rdx
	movq	%rax, 112(%rsp)
	movl	%ecx, %ecx
	orq	%r8, %rcx
	movl	96(%rsp), %r8d
	movq	%rcx, 184(%rsp)
	movq	%rsi, %rcx
	call	control_rounded_rect
	movl	16(%rbx), %eax
	testb	$32, %ah
	je	.L1040
	movl	68(%rbx), %r8d
.L1041:
	movq	128(%rsp), %r15
	movq	120(%rsp), %r9
	xorl	%edx, %edx
	movabsq	$-4294967296, %r11
	movq	144(%rsp), %r14
	movl	104(%rsp), %r13d
	movl	%r8d, 88(%rsp)
	salq	$32, %r9
	movl	%r15d, %ecx
	orq	%r9, %rcx
	movq	%r13, %r9
	movq	%rcx, %rax
	movq	%rdx, %rcx
	salq	$32, %r9
	movq	112(%rsp), %rdx
	andq	%r11, %rcx
	movq	%r9, 96(%rsp)
	orq	%r14, %rcx
	movq	%rax, 176(%rsp)
	movl	%ecx, %ecx
	orq	%r9, %rcx
	movq	%rcx, 184(%rsp)
	movq	%rsi, %rcx
	call	nova_surface_rect
	movq	%r15, %rax
	movl	80(%rsp), %r15d
	xorl	%edx, %edx
	movabsq	$-4294967296, %r11
	movl	88(%rsp), %r8d
	leal	(%r15,%rbp), %ecx
	subl	%r13d, %ecx
	salq	$32, %rcx
	movq	%rcx, %r10
	movl	%eax, %ecx
	orq	%r10, %rcx
	movq	%rcx, %rax
	movq	%rdx, %rcx
	movq	112(%rsp), %rdx
	andq	%r11, %rcx
	movq	%rax, 176(%rsp)
	orq	%r14, %rcx
	movl	%ecx, %ecx
	orq	96(%rsp), %rcx
	movq	%rcx, 184(%rsp)
	movq	%rsi, %rcx
	call	nova_surface_rect
	movzwl	76(%rbx), %eax
	movl	$8, %edx
	movzwl	518(%rbx), %r9d
	testw	%ax, %ax
	cmove	%edx, %eax
	movzwl	%ax, %ecx
	leal	-20(%rbp), %eax
	shrl	$31, %eax
	movl	%ecx, %r14d
	leal	-20(%rax,%rbp), %eax
	sarl	%eax
	addl	%eax, %r15d
	leal	(%r12,%rcx), %eax
	movl	%eax, 96(%rsp)
	testw	%r9w, %r9w
	jne	.L1044
	cmpb	$0, 325(%rbx)
	jne	.L1301
.L1044:
	cmpl	$7, 8(%rbx)
	je	.L1046
	testb	$1, 17(%rbx)
	jne	.L1046
	movzwl	522(%rbx), %r11d
	movzwl	524(%rbx), %eax
	cmpw	%r11w, %ax
	jb	.L1053
	leaq	133(%rbx), %rdx
	movq	%rdx, 88(%rsp)
	cmpw	%ax, %r11w
	jne	.L1302
.L1054:
	movl	$1, 56(%rsp)
	leal	(%r14,%r14), %eax
	movl	$0, 48(%rsp)
	movl	40(%rbx), %edx
	movl	%edx, 40(%rsp)
	movq	88(%rsp), %rdx
.L1288:
	movq	%rdx, 32(%rsp)
	movl	%edi, %r9d
	movl	96(%rsp), %edx
	movl	%r15d, %r8d
	subl	%eax, %r9d
	movq	%rsi, %rcx
	call	nova_text_draw
.L1045:
	movl	16(%rbx), %ecx
	movl	%ecx, %eax
	andl	$16388, %eax
	cmpl	$4, %eax
	je	.L1303
	.p2align 4
	.p2align 3
.L1100:
	movl	8(%rbx), %eax
.L1065:
	andl	$-3, %eax
	jne	.L1073
.L1175:
	andl	$-9, %ecx
	movl	%ecx, 16(%rbx)
	addl	$1, 28+diagnostics(%rip)
.L1016:
	addq	$312, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%rbp
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	ret
	.p2align 4,,10
	.p2align 3
.L1018:
	movl	48(%rbx), %eax
	movl	%eax, 88(%rsp)
	jmp	.L1019
	.p2align 4,,10
	.p2align 3
.L1028:
	movl	$8, %r11d
	movl	%ebp, %ecx
	movl	%eax, 104(%rsp)
	movl	%r12d, %eax
	cmpl	%r11d, %ebp
	movq	%rax, 96(%rsp)
	movabsq	$-4294967296, %r15
	cmovle	%ebp, %r11d
	xorl	%r9d, %r9d
	subl	%r11d, %ecx
	movl	%ecx, %edx
	shrl	$31, %edx
	addl	%ecx, %edx
	movl	%r11d, %ecx
	sarl	%edx
	addl	80(%rsp), %edx
	shrl	$31, %ecx
	movq	%rdx, %r14
	movl	%eax, %edx
	addl	%r11d, %ecx
	salq	$32, %r11
	salq	$32, %r14
	sarl	%ecx
	movq	%r11, 120(%rsp)
	orq	%r14, %rdx
	movzwl	%cx, %r13d
	movl	%edi, %ecx
	movq	%rdx, %r8
	movq	%r9, %rdx
	movl	88(%rsp), %r9d
	andq	%r15, %rdx
	leaq	176(%rsp), %r15
	movq	%r8, 176(%rsp)
	movl	%r13d, %r8d
	orq	%rcx, %rdx
	movq	%rsi, %rcx
	movq	%r15, 112(%rsp)
	movl	%edx, %edx
	orq	%r11, %rdx
	movq	%rdx, 184(%rsp)
	movq	%r15, %rdx
	call	control_rounded_rect
	movslq	104(%rsp), %r10
	movslq	%edi, %r8
	movabsq	$2361183241434822607, %rax
	imulq	%r10, %r8
	imulq	%r8
	sarq	$63, %r8
	sarq	$7, %rdx
	subq	%r8, %rdx
	testl	%edx, %edx
	jle	.L1071
	movq	96(%rsp), %rax
	movl	%edx, %ecx
	xorl	%edx, %edx
	movq	%r10, 88(%rsp)
	movl	%eax, %r8d
	orq	%r14, %r8
	movabsq	$-4294967296, %r14
	movq	%r8, %rax
	movq	%rdx, %r8
	movq	%r15, %rdx
	andq	%r14, %r8
	movq	%rax, 176(%rsp)
	orq	%r8, %rcx
	movl	%r13d, %r8d
	movl	%ecx, %ecx
	orq	120(%rsp), %rcx
	movq	%rcx, 184(%rsp)
	movl	44(%rbx), %r9d
	movq	%rsi, %rcx
	call	control_rounded_rect
	movq	88(%rsp), %r10
.L1071:
	cmpl	$15, 8(%rbx)
	jne	.L1072
	movl	16(%rbx), %ecx
	.p2align 4
	.p2align 3
.L1073:
	testb	$68, %cl
	je	.L1175
	movzwl	74(%rbx), %eax
	movq	28(%rbx), %r9
	movabsq	$-4294967296, %rcx
	movq	20(%rbx), %r8
	movl	%eax, %edx
	sall	$2, %eax
	testw	%dx, %dx
	movl	$4, %edx
	movq	%r8, 176(%rsp)
	cmove	%edx, %eax
	movq	%r9, %rdx
	andq	%rcx, %rdx
	movq	%rsi, %rcx
	orq	%rdx, %rax
	leaq	176(%rsp), %rdx
	movq	%rax, 184(%rsp)
	movl	44(%rbx), %r8d
	call	nova_surface_rect
	movl	16(%rbx), %ecx
	jmp	.L1175
	.p2align 4,,10
	.p2align 3
.L1033:
	cmpl	$4, 12(%rbx)
	je	.L1150
	movl	40(%rbx), %r11d
.L1151:
	movzwl	124(%rbx), %eax
	cmpw	$5, %ax
	je	.L1304
	cmpw	$6, %ax
	je	.L1305
	cmpw	$7, %ax
	je	.L1306
	cmpw	$8, %ax
	jne	.L1153
	movl	44(%rbx), %r11d
	.p2align 4
	.p2align 3
.L1153:
	movl	36(%rbx), %r9d
	movl	%r9d, %eax
	shrl	$24, %eax
	jne	.L1307
.L1156:
	movslq	80(%rbx), %rcx
	imulq	$1431655766, %rcx, %rax
	movl	%ecx, %edx
	movq	%rcx, %r8
	sarl	$31, %edx
	shrq	$32, %rax
	subl	%edx, %eax
	leal	(%rax,%rax,2), %edx
	movl	%ecx, %eax
	subl	%edx, %eax
	cmpb	$1, %al
	je	.L1157
	cmpb	$2, %al
	sete	%al
	movzbl	%al, %eax
	leal	(%rax,%rax), %r13d
.L1157:
	movl	92(%rbx), %r9d
	cmpl	$499, %r9d
	jle	.L1192
	movzwl	%r9w, %r9d
	leal	(%r9,%r9,2), %eax
	leal	0(,%rax,8), %eax
	imulq	$274877907, %rax, %rax
	shrq	$38, %rax
	movq	%rax, %r15
	leal	(%r9,%r9,4), %eax
	leal	0(,%rax,4), %eax
	imulq	$274877907, %rax, %rax
	shrq	$38, %rax
	movq	%rax, %r14
.L1158:
	movl	%ebp, %eax
	cltd
	idivl	%r15d
	cmpw	$1, %ax
	ja	.L1159
	imulq	$1431655766, %rcx, %rcx
	sarl	$31, %r8d
	shrq	$32, %rcx
	subl	%r8d, %ecx
	cmpb	$1, %cl
	je	.L1308
	movl	80(%rsp), %edx
	leal	(%rdx,%rbp), %eax
	subl	%r14d, %eax
	cmpb	$2, %cl
	cmovne	%edx, %eax
	movl	%eax, 80(%rsp)
.L1161:
	movl	%r9d, 64(%rsp)
	movl	80(%rsp), %r8d
	movq	%rsi, %rcx
	movl	%edi, %r9d
	leaq	133(%rbx), %rax
	movl	$1, 56(%rsp)
	movl	%r12d, %edx
	movq	%rax, 32(%rsp)
	movl	%r13d, 48(%rsp)
	movl	%r11d, 40(%rsp)
	call	nova_text_draw_scaled
	movl	8(%rbx), %eax
	movl	16(%rbx), %ecx
	jmp	.L1065
	.p2align 4,,10
	.p2align 3
.L1022:
	cmpw	$1, 124(%rbx)
	je	.L1309
	cmpb	$0, 133(%rbx)
	jne	.L1136
	movl	%ebp, %ecx
	movl	80(%rsp), %r15d
	movl	%r12d, %eax
	xorl	%edx, %edx
	shrl	$31, %ecx
	addl	%ebp, %ecx
	sarl	%ecx
	leal	(%rcx,%r15), %r8d
	movl	%eax, %ecx
	salq	$32, %r8
	orq	%r8, %rcx
	movabsq	$-4294967296, %r8
	movq	%rcx, %rax
	movq	%rdx, %rcx
	andq	%r8, %rcx
	movq	104(%rsp), %r8
	movq	%rax, 176(%rsp)
	orq	%rdi, %rcx
	salq	$32, %r8
	movl	%ecx, %ecx
	orq	%r8, %rcx
	movq	%rcx, 184(%rsp)
.L1297:
	movl	48(%rbx), %r8d
	movq	%rsi, %rcx
	leaq	176(%rsp), %rdx
	call	nova_surface_rect
	movl	8(%rbx), %eax
	movl	16(%rbx), %ecx
	jmp	.L1065
	.p2align 4,,10
	.p2align 3
.L1039:
	movl	36(%rbx), %eax
	movl	%eax, 88(%rsp)
	cmpw	$6, 124(%rbx)
	je	.L1310
.L1103:
	andl	$68, %ecx
	je	.L1104
	movl	104(%rsp), %edx
	movl	80(%rsp), %r15d
	movl	$2, %eax
	movabsq	$-4294967296, %r14
	movl	%r10d, 112(%rsp)
	cmpl	%eax, %edx
	movl	%edx, %r11d
	movq	%r15, %r8
	cmovl	%eax, %r11d
	movl	%r12d, %eax
	salq	$32, %r8
	xorl	%edx, %edx
	movl	%eax, %ecx
	orq	%r8, %rcx
	movl	%edi, %r8d
	movl	%r11d, 104(%rsp)
	movq	%rcx, %rax
	movq	%rdx, %rcx
	leaq	176(%rsp), %rdx
	andq	%r14, %rcx
	movq	%rax, 176(%rsp)
	orq	%r8, %rcx
	movq	%rbp, %r8
	salq	$32, %r8
	movl	%ecx, %ecx
	orq	%r8, %rcx
	movzwl	%r10w, %r8d
	movq	%rcx, 184(%rsp)
	movl	44(%rbx), %r9d
	movq	%rsi, %rcx
	call	control_rounded_rect
	movl	104(%rsp), %r11d
	movl	%ebp, %r9d
	xorl	%edx, %edx
	leal	(%r12,%r11), %eax
	leal	(%r15,%r11), %r8d
	salq	$32, %r8
	movl	%eax, %ecx
	orq	%r8, %rcx
	leal	(%r11,%r11), %r8d
	movq	%rcx, %rax
	movl	%edi, %ecx
	subl	%r8d, %r9d
	subl	%r8d, %ecx
	testl	%ecx, %ecx
	jle	.L1107
	testl	%r9d, %r9d
	jle	.L1107
	movl	112(%rsp), %r10d
	movq	%rax, 176(%rsp)
	subl	%r11d, %r10d
	cmpl	%r11d, 96(%rsp)
	movl	$0, %r11d
	movzwl	%r10w, %r8d
	cmovle	%r11d, %r8d
	movl	%ecx, %r11d
	movq	%rdx, %rcx
	salq	$32, %r9
	andq	%r14, %rcx
	leaq	176(%rsp), %rdx
	orq	%r11, %rcx
	movl	%ecx, %ecx
	orq	%r9, %rcx
	movl	88(%rsp), %r9d
	movq	%rcx, 184(%rsp)
	movq	%rsi, %rcx
	call	control_rounded_rect
	.p2align 4
	.p2align 3
.L1107:
	movzwl	76(%rbx), %eax
	testw	%ax, %ax
	je	.L1183
	leal	(%rax,%rax), %edx
.L1108:
	subl	%edx, %edi
	movl	84(%rbx), %edx
	leal	(%r12,%rax), %r10d
	movl	%edi, %r11d
	cmpl	$14, %edx
	jbe	.L1311
.L1109:
	cmpb	$0, 133(%rbx)
	jne	.L1110
	.p2align 4
	.p2align 3
.L1296:
	movl	8(%rbx), %eax
.L1293:
	movl	16(%rbx), %ecx
	jmp	.L1065
	.p2align 4,,10
	.p2align 3
.L1038:
	testb	$84, %cl
	je	.L1100
	movl	36(%rbx), %r9d
	testl	%r9d, %r9d
	jne	.L1312
.L1101:
	testb	$4, %cl
	je	.L1100
	movl	80(%rsp), %r15d
	movl	%r12d, %eax
	xorl	%r9d, %r9d
	movl	%edi, %r10d
	movl	%eax, %edx
	movq	%rax, 96(%rsp)
	movabsq	$-4294967296, %r11
	movabsq	$8589934592, %rdi
	movq	%r15, %rcx
	movq	%r10, 88(%rsp)
	salq	$32, %rcx
	orq	%rcx, %rdx
	movq	%rsi, %rcx
	movq	%rdx, %r8
	movq	%r9, %rdx
	leaq	176(%rsp), %r9
	andq	%r11, %rdx
	movq	%r8, 176(%rsp)
	orq	%r10, %rdx
	movq	%r9, 80(%rsp)
	movl	%edx, %edx
	orq	%rdi, %rdx
	movq	%rdx, 184(%rsp)
	movl	44(%rbx), %r8d
	movq	%r9, %rdx
	call	nova_surface_rect
	movq	96(%rsp), %rax
	leal	-2(%r15,%rbp), %r8d
	xorl	%edx, %edx
	movabsq	$-4294967296, %r11
	salq	$32, %r8
	movl	%eax, %ecx
	orq	%r8, %rcx
	movq	%rcx, %rax
	movq	%rdx, %rcx
	movq	80(%rsp), %rdx
	andq	%r11, %rcx
	orq	88(%rsp), %rcx
	movq	%rax, 176(%rsp)
	movl	%ecx, %ecx
	orq	%rdi, %rcx
	movq	%rcx, 184(%rsp)
	movl	44(%rbx), %r8d
	movq	%rsi, %rcx
	call	nova_surface_rect
	movl	8(%rbx), %eax
	movl	16(%rbx), %ecx
	jmp	.L1065
	.p2align 4,,10
	.p2align 3
.L1032:
	cmpl	$4, 12(%rbx)
	je	.L1162
	movl	40(%rbx), %edx
.L1163:
	testb	$32, %ch
	je	.L1164
	movl	68(%rbx), %edx
.L1165:
	cmpl	%ebp, %edi
	movl	%ebp, %eax
	cmovle	%edi, %eax
	testl	%eax, %eax
	jle	.L1073
	movl	80(%rbx), %r11d
	cmpl	$14, %r11d
	ja	.L1073
	subl	%eax, %ebp
	subl	%eax, %edi
	movzwl	%ax, %eax
	movl	%edx, 40(%rsp)
	movl	%ebp, %r8d
	movl	%edi, %ecx
	movl	%eax, 32(%rsp)
	movl	80(%rsp), %eax
	shrl	$31, %r8d
	shrl	$31, %ecx
	movl	%r11d, %edx
	addl	%ebp, %r8d
	addl	%edi, %ecx
	sarl	%r8d
	sarl	%ecx
	leal	(%r8,%rax), %r9d
	leal	(%rcx,%r12), %r8d
	movq	%rsi, %rcx
	call	nova_icon_draw
	movl	8(%rbx), %eax
	movl	16(%rbx), %ecx
	jmp	.L1065
	.p2align 4,,10
	.p2align 3
.L1021:
	movq	80(%rsp), %r8
	movl	%r12d, %eax
	xorl	%edx, %edx
	movl	88(%rsp), %r9d
	movl	%eax, %ecx
	salq	$32, %rbp
	salq	$32, %r8
	orq	%r8, %rcx
	movabsq	$-4294967296, %r8
	movq	%rcx, %rax
	movq	%rdx, %rcx
	leaq	176(%rsp), %rdx
	andq	%r8, %rcx
	movl	96(%rsp), %r8d
	movq	%rax, 176(%rsp)
	orq	%rdi, %rcx
	movl	%ecx, %ecx
	orq	%rcx, %rbp
	movq	%rsi, %rcx
	movq	%rbp, 184(%rsp)
	call	control_rounded_rect
	movl	8(%rbx), %eax
	cmpl	$23, %eax
	jbe	.L1293
	addl	$1, 32+diagnostics(%rip)
	jmp	.L1296
	.p2align 4,,10
	.p2align 3
.L1037:
	testl	$65556, %ecx
	je	.L1111
	movl	36(%rbx), %eax
	andl	$4, %ecx
	movl	%eax, 88(%rsp)
	je	.L1113
	movl	104(%rsp), %edx
	movl	80(%rsp), %r15d
	movl	$2, %eax
	movabsq	$-4294967296, %r13
	leaq	176(%rsp), %r14
	movl	%r10d, 120(%rsp)
	cmpl	%eax, %edx
	movl	%edx, %r11d
	movq	%r15, %r8
	movq	%r14, 112(%rsp)
	cmovl	%eax, %r11d
	movl	%r12d, %eax
	salq	$32, %r8
	xorl	%edx, %edx
	movl	%eax, %ecx
	orq	%r8, %rcx
	movl	%edi, %r8d
	movl	%r11d, 104(%rsp)
	movq	%rcx, %rax
	movq	%rdx, %rcx
	movq	%r14, %rdx
	andq	%r13, %rcx
	movq	%rax, 176(%rsp)
	orq	%r8, %rcx
	movq	%rbp, %r8
	salq	$32, %r8
	movl	%ecx, %ecx
	orq	%r8, %rcx
	movl	96(%rsp), %r8d
	movq	%rcx, 184(%rsp)
	movl	44(%rbx), %r9d
	movq	%rsi, %rcx
	call	control_rounded_rect
	movl	104(%rsp), %r11d
	movl	%edi, %r9d
	xorl	%edx, %edx
	leal	(%r12,%r11), %eax
	leal	(%r15,%r11), %r8d
	salq	$32, %r8
	movl	%eax, %ecx
	orq	%r8, %rcx
	leal	(%r11,%r11), %r8d
	movq	%rcx, %rax
	movl	%ebp, %ecx
	subl	%r8d, %r9d
	subl	%r8d, %ecx
	testl	%ecx, %ecx
	jle	.L1116
	testl	%r9d, %r9d
	jle	.L1116
	movl	120(%rsp), %r10d
	movq	%rax, 176(%rsp)
	subl	%r11d, %r10d
	cmpl	%r11d, 96(%rsp)
	movl	$0, %r11d
	movzwl	%r10w, %r8d
	cmovle	%r11d, %r8d
	movl	%r9d, %r11d
	movq	%rdx, %r9
	salq	$32, %rcx
	andq	%r13, %r9
	movq	%r14, %rdx
	orq	%r11, %r9
	movl	%r9d, %r9d
	orq	%r9, %rcx
	movl	88(%rsp), %r9d
	movq	%rcx, 184(%rsp)
	movq	%rsi, %rcx
	call	control_rounded_rect
	.p2align 4
	.p2align 3
.L1116:
	movzwl	76(%rbx), %edx
	testw	%dx, %dx
	je	.L1185
	leal	(%rdx,%rdx,2), %ecx
	negl	%ecx
.L1117:
	leal	(%rdi,%r12), %eax
	movl	80(%rsp), %r15d
	leal	-10(%rdi,%rcx), %r9d
	movl	$1, 56(%rsp)
	subl	%edx, %eax
	movl	$0, 48(%rsp)
	movl	40(%rbx), %r8d
	xorl	%edi, %edi
	leal	-10(%rax), %r14d
	leal	-20(%rbp), %eax
	addl	%r12d, %edx
	movq	%rsi, %rcx
	movl	%r8d, 40(%rsp)
	shrl	$31, %eax
	leaq	133(%rbx), %r8
	movq	%r8, 32(%rsp)
	leal	-20(%rax,%rbp), %eax
	sarl	%eax
	leal	(%rax,%r15), %r8d
	call	nova_text_draw
	movl	%ebp, %eax
	movl	16(%rbx), %r12d
	xorl	%edx, %edx
	shrl	$31, %eax
	addl	%ebp, %eax
	andl	$65536, %r12d
	movabsq	$-4294967296, %rbp
	sarl	%eax
	leal	-2(%r15,%rax), %r13d
	xorl	%eax, %eax
	movq	%rax, %rcx
	andq	%rbp, %rcx
	movq	%rcx, %r15
.L1119:
	movl	$4, %ecx
	leal	0(%r13,%rdi), %r10d
	subl	%edi, %ecx
	testl	%r12d, %r12d
	cmove	%edi, %ecx
	salq	$32, %r10
	addl	$1, %edi
	leal	(%rcx,%r14), %r8d
	addl	%ecx, %ecx
	orq	%r15, %r8
	movl	%r8d, %r8d
	orq	%r10, %r8
	movq	%r8, %rax
	movl	$10, %r8d
	subl	%ecx, %r8d
	movq	%rdx, %rcx
	movq	%rax, 176(%rsp)
	movq	112(%rsp), %rdx
	andq	%rbp, %rcx
	orq	%r8, %rcx
	movl	%ecx, %ecx
	btsq	$32, %rcx
	movq	%rcx, 184(%rsp)
	movl	40(%rbx), %r8d
	movq	%rsi, %rcx
	call	nova_surface_rect
	cmpl	$5, %edi
	jne	.L1119
	jmp	.L1296
	.p2align 4,,10
	.p2align 3
.L1035:
	leal	-4(%rbp), %r11d
	andb	$4, %ch
	je	.L1068
	movl	44(%rbx), %eax
	movl	%eax, 88(%rsp)
.L1068:
	movl	80(%rsp), %r14d
	movl	%r12d, %eax
	xorl	%edx, %edx
	movl	88(%rsp), %r9d
	movl	%eax, %ecx
	movl	%r11d, 96(%rsp)
	movabsq	$-4294967296, %r10
	movq	%r14, %r8
	salq	$32, %r8
	orq	%r8, %rcx
	movl	%edi, %r8d
	addl	%r12d, %edi
	movq	%rcx, %rax
	movq	%rdx, %rcx
	leaq	176(%rsp), %rdx
	andq	%r10, %rcx
	movq	%rax, 176(%rsp)
	orq	%r8, %rcx
	movq	%rbp, %r8
	salq	$32, %r8
	movl	%ecx, %ecx
	orq	%r8, %rcx
	movl	%ebp, %r8d
	shrl	$31, %r8d
	movq	%rcx, 184(%rsp)
	movq	%rsi, %rcx
	addl	%ebp, %r8d
	sarl	%r8d
	movzwl	%r8w, %r8d
	call	control_rounded_rect
	movl	96(%rsp), %r11d
	leal	2(%r12), %eax
	leal	2(%r14), %r8d
	movabsq	$-4294967296, %r10
	subl	%r11d, %edi
	testb	$4, 17(%rbx)
	leal	-2(%rdi), %edx
	cmovne	%edx, %eax
	salq	$32, %r8
	xorl	%edx, %edx
	movl	%eax, %ecx
	orq	%r8, %rcx
	movl	%r11d, %r8d
	movq	%rcx, %rax
	movq	%rdx, %rcx
	leaq	176(%rsp), %rdx
	andq	%r10, %rcx
	movq	%rax, 176(%rsp)
	orq	%r8, %rcx
	salq	$32, %r8
	movl	%ecx, %ecx
	orq	%r8, %rcx
	movl	%r11d, %r8d
	shrl	$31, %r8d
	movq	%rcx, 184(%rsp)
	movl	40(%rbx), %r9d
	movq	%rsi, %rcx
	addl	%r11d, %r8d
	sarl	%r8d
	movzwl	%r8w, %r8d
	call	control_rounded_rect
	movl	8(%rbx), %eax
	movl	16(%rbx), %ecx
	jmp	.L1065
	.p2align 4,,10
	.p2align 3
.L1036:
	movq	80(%rsp), %r8
	cmpl	%ebp, %edi
	movl	%r12d, %eax
	movl	88(%rsp), %r9d
	cmovle	%edi, %ebp
	movl	%eax, %ecx
	xorl	%edx, %edx
	movabsq	$-4294967296, %rdi
	salq	$32, %r8
	orq	%r8, %rcx
	movl	%ebp, %r8d
	movq	%rcx, %rax
	movq	%rdx, %rcx
	andq	%rdi, %rcx
	movq	%rax, 176(%rsp)
	leaq	176(%rsp), %rax
	orq	%r8, %rcx
	salq	$32, %r8
	movq	%rax, %rdx
	movq	%rax, 112(%rsp)
	movl	%ecx, %ecx
	orq	%r8, %rcx
	movl	96(%rsp), %r8d
	movq	%rcx, 184(%rsp)
	movq	%rsi, %rcx
	call	control_rounded_rect
	movl	16(%rbx), %ecx
	testb	$4, %ch
	jne	.L1313
	testb	$8, %ch
	je	.L1100
	testl	%ebp, %ebp
	leal	3(%rbp), %eax
	movl	%ebp, %ecx
	movl	80(%rsp), %r14d
	cmovns	%ebp, %eax
	shrl	$31, %ecx
	xorl	%edx, %edx
	addl	%ebp, %ecx
	sarl	$2, %eax
	sarl	%ecx
	addl	%r12d, %eax
	leal	(%rcx,%r14), %r9d
	movl	%eax, %eax
	salq	$32, %r9
	movl	%eax, %r8d
	orq	%r9, %r8
	movq	%r8, %rax
	movq	%rdx, %r8
	andq	%rdi, %r8
	orq	%r8, %rcx
	movl	104(%rsp), %r8d
	movq	%rcx, %rdx
	addl	$1, %r8d
.L1298:
	salq	$32, %r8
	movl	%edx, %ecx
	movq	%rax, 176(%rsp)
	movq	112(%rsp), %rdx
	orq	%r8, %rcx
	movq	%rcx, 184(%rsp)
	movl	44(%rbx), %r8d
	movq	%rsi, %rcx
	call	nova_surface_rect
.L1289:
	movl	16(%rbx), %ecx
	jmp	.L1100
	.p2align 4,,10
	.p2align 3
.L1027:
	testb	$16, %ch
	je	.L1073
	cmpl	%ebp, %edi
	movl	%ebp, %eax
	cmovle	%edi, %eax
	cmpl	$13, %eax
	jle	.L1188
	movslq	%eax, %rdx
	movl	%eax, %r11d
	imulq	$-1840700269, %rdx, %rdx
	sarl	$31, %r11d
	shrq	$32, %rdx
	addl	%eax, %edx
	movl	%edx, %ecx
	sarl	$3, %edx
	sarl	$2, %ecx
	subl	%r11d, %ecx
	subl	%edx, %r11d
	movl	%ecx, %r15d
.L1140:
	movl	%edi, %edx
	shrl	$31, %edx
	addl	%edi, %edx
	movl	80(%rsp), %edi
	sarl	%edx
	leal	(%rdx,%r12), %r14d
	movl	%ebp, %edx
	shrl	$31, %edx
	addl	%edx, %ebp
	sarl	%ebp
	leal	(%rdi,%rbp), %r13d
	movl	80(%rbx), %edi
	leal	0(,%rdi,8), %r8d
	xorl	%edi, %edi
	movq	%r8, %rdx
	imulq	$98685563, %r8, %r8
	shrq	$32, %r8
	subl	%r8d, %edx
	shrl	%edx
	addl	%r8d, %edx
	movzwl	124(%rbx), %r8d
	shrl	$9, %edx
	movl	%edx, %ebp
	andl	$7, %ebp
	cmpw	$2, %r8w
	je	.L1285
	movl	%r11d, %edx
	movq	%rsi, 392(%rsp)
	movl	%eax, %r12d
	movl	%r11d, %esi
	negl	%edx
	movl	%edx, 80(%rsp)
	jmp	.L1141
	.p2align 4,,10
	.p2align 3
.L1148:
	salq	$32, %rdx
	movl	%r8d, %r10d
	xorl	%r9d, %r9d
	movabsq	$-4294967296, %rax
	orq	%rdx, %r10
	movl	%r15d, %edx
	movq	%r10, %r8
	movq	%r9, %r10
	movl	%ecx, %r9d
	movq	392(%rsp), %rcx
	andq	%rax, %r10
	movq	%r8, 176(%rsp)
	movzwl	80(%rsp), %r8d
	orq	%rdx, %r10
	salq	$32, %rdx
	movl	%r10d, %r10d
	orq	%rdx, %r10
	leaq	176(%rsp), %rdx
	movq	%r10, 184(%rsp)
	call	control_rounded_rect
.L1147:
	addq	$1, %rdi
	cmpq	$8, %rdi
	je	.L1314
	movzwl	124(%rbx), %r8d
.L1141:
	cmpw	$1, %r8w
	je	.L1144
	movl	%edi, %edx
	subl	%ebp, %edx
	addl	$8, %edx
	cmpw	$3, %r8w
	je	.L1315
	andl	$7, %edx
	cmpb	$5, %dl
	ja	.L1147
.L1144:
	leaq	px.1(%rip), %rax
	movsbl	(%rax,%rdi), %edx
	leaq	py.0(%rip), %rax
	movsbl	(%rax,%rdi), %ecx
	imull	%r12d, %edx
	imull	%r12d, %ecx
	movslq	%edx, %r8
	sarl	$31, %edx
	imulq	$954437177, %r8, %r8
	sarq	$34, %r8
	subl	%edx, %r8d
	movslq	%ecx, %rdx
	sarl	$31, %ecx
	imulq	$954437177, %rdx, %rdx
	addl	%r14d, %r8d
	addl	%esi, %r8d
	sarq	$34, %rdx
	subl	%ecx, %edx
	movl	88(%rsp), %ecx
	addl	%r13d, %edx
	addl	%esi, %edx
	cmpb	%dil, %bpl
	jne	.L1148
.L1177:
	movl	44(%rbx), %ecx
	jmp	.L1148
	.p2align 4,,10
	.p2align 3
.L1030:
	andl	$32, %ecx
	jne	.L1090
	movl	36(%rbx), %r9d
.L1091:
	movq	80(%rsp), %r14
	movl	%r12d, %r10d
	xorl	%edx, %edx
	salq	$32, %rbp
	movl	%r10d, %ecx
	movl	96(%rsp), %r8d
	movq	%r10, 88(%rsp)
	movabsq	$-4294967296, %r11
	salq	$32, %r14
	orq	%r14, %rcx
	movq	%rcx, %rax
	movq	%rdx, %rcx
	leaq	176(%rsp), %rdx
	andq	%r11, %rcx
	movq	%rax, 176(%rsp)
	orq	%rdi, %rcx
	movl	%ecx, %ecx
	orq	%rbp, %rcx
	movq	%rcx, 184(%rsp)
	movq	%rsi, %rcx
	call	control_rounded_rect
	movl	16(%rbx), %ecx
	movabsq	$-4294967296, %r11
	testb	$32, %ch
	je	.L1100
	movq	88(%rsp), %rax
	xorl	%edx, %edx
	movl	%eax, %ecx
	orq	%r14, %rcx
	movq	%rcx, %rax
	movq	%rdx, %rcx
	leaq	176(%rsp), %rdx
	andq	%r11, %rcx
	movq	%rax, 176(%rsp)
	orq	$4, %rcx
	movl	%ecx, %ecx
	orq	%rbp, %rcx
	movq	%rcx, 184(%rsp)
	movl	68(%rbx), %r8d
	movq	%rsi, %rcx
	call	nova_surface_rect
	movl	8(%rbx), %eax
	movl	16(%rbx), %ecx
	jmp	.L1065
	.p2align 4,,10
	.p2align 3
.L1025:
	andl	$64, %ecx
	je	.L1093
	movl	48(%rbx), %eax
	movl	%eax, 88(%rsp)
.L1093:
	movl	80(%rsp), %r15d
	movl	%r12d, %eax
	xorl	%edx, %edx
	movl	88(%rsp), %r9d
	movl	%eax, %ecx
	movq	%rax, 128(%rsp)
	movq	%r15, %r8
	movq	%r15, 120(%rsp)
	movl	%edi, %r15d
	salq	$32, %r8
	movq	%r15, 144(%rsp)
	orq	%r8, %rcx
	movabsq	$-4294967296, %r8
	movq	%rcx, %rax
	movq	%rdx, %rcx
	andq	%r8, %rcx
	movq	%rbp, %r8
	movq	%rax, 176(%rsp)
	leaq	176(%rsp), %rax
	orq	%r15, %rcx
	salq	$32, %r8
	movq	%rax, 112(%rsp)
	movq	%rcx, %rdx
	movl	%ebp, %ecx
	movq	%rcx, 136(%rsp)
	movl	%edx, %ecx
	movq	%rax, %rdx
	orq	%r8, %rcx
	movl	96(%rsp), %r8d
	movq	%rcx, 184(%rsp)
	movq	%rsi, %rcx
	call	control_rounded_rect
	movzwl	124(%rbx), %eax
	cmpw	$2, %ax
	je	.L1316
	cmpw	$4, %ax
	je	.L1317
	cmpw	$5, %ax
	je	.L1318
	cmpw	$3, %ax
	je	.L1098
	movl	44(%rbx), %r8d
	.p2align 4
	.p2align 3
.L1095:
	movq	128(%rsp), %rax
	movq	120(%rsp), %r14
	xorl	%edx, %edx
	movabsq	$-4294967296, %r15
	salq	$32, %r14
	movl	%eax, %ecx
	orq	%r14, %rcx
	movq	%r14, 88(%rsp)
	movq	136(%rsp), %r14
	movq	%rcx, %rax
	movq	%rdx, %rcx
	movq	112(%rsp), %rdx
	andq	%r15, %rcx
	salq	$32, %r14
	movq	%rax, 176(%rsp)
	orq	$4, %rcx
	movq	%r14, 96(%rsp)
	movl	%ecx, %ecx
	orq	%r14, %rcx
	movq	%rcx, 184(%rsp)
	movq	%rsi, %rcx
	call	nova_surface_rect
	movl	16(%rbx), %ecx
	testb	$64, %cl
	je	.L1100
	movq	128(%rsp), %r13
	xorl	%r9d, %r9d
	movq	%r15, %r14
	movq	%rsi, %rcx
	movl	%r13d, %eax
	orq	88(%rsp), %rax
	movq	%rax, %r8
	movq	%r9, %rax
	andq	%r15, %rax
	orq	144(%rsp), %rax
	movq	112(%rsp), %r15
	movq	%r8, 176(%rsp)
	movl	%eax, %edx
	btsq	$33, %rdx
	movq	%rdx, 184(%rsp)
	movl	44(%rbx), %r8d
	movq	%r15, %rdx
	call	nova_surface_rect
	movl	80(%rsp), %eax
	movl	%r13d, %edx
	xorl	%r9d, %r9d
	leal	-2(%rax,%rbp), %ecx
	movabsq	$8589934592, %rax
	salq	$32, %rcx
	orq	%rcx, %rdx
	movq	%rsi, %rcx
	movq	%rdx, %r8
	movq	%r9, %rdx
	andq	%r14, %rdx
	orq	144(%rsp), %rdx
	movq	%r8, 176(%rsp)
	movl	%edx, %edx
	orq	%rdx, %rax
	movq	%r15, %rdx
	movq	%rax, 184(%rsp)
	movl	44(%rbx), %r8d
	call	nova_surface_rect
	leal	-2(%rdi,%r12), %eax
	xorl	%edx, %edx
	movl	%eax, %ecx
	orq	88(%rsp), %rcx
	movq	%rcx, %rax
	movq	%rdx, %rcx
	movq	%r15, %rdx
	andq	%r14, %rcx
	movq	%rax, 176(%rsp)
	orq	$2, %rcx
	movl	%ecx, %ecx
	orq	96(%rsp), %rcx
	movq	%rcx, 184(%rsp)
	movl	44(%rbx), %r8d
	movq	%rsi, %rcx
	call	nova_surface_rect
	movl	16(%rbx), %ecx
	jmp	.L1100
	.p2align 4,,10
	.p2align 3
.L1031:
	movl	80(%rbx), %eax
	testl	%eax, %eax
	jne	.L1073
	call	nova_branding_intrinsic_width
	movl	%eax, %r15d
	call	nova_branding_intrinsic_height
	movzwl	%ax, %edx
	movzwl	124(%rbx), %eax
	cmpw	$1, %ax
	je	.L1193
	movzwl	%r15w, %r9d
	movzwl	%dx, %ecx
	cmpw	$2, %ax
	je	.L1319
	cmpw	$3, %ax
	je	.L1320
	testw	%ax, %ax
	jne	.L1292
	cmpl	%edi, %r9d
	cmovg	%edi, %r9d
	cmpl	%ebp, %ecx
	cmovg	%ebp, %ecx
.L1292:
	movl	%edi, %eax
	subl	%r9d, %eax
	movl	%eax, %r11d
	shrl	$31, %r11d
	addl	%eax, %r11d
	movl	%ebp, %eax
	subl	%ecx, %eax
	sarl	%r11d
	movl	%eax, %r8d
	addl	%r12d, %r11d
	shrl	$31, %r8d
	addl	%eax, %r8d
	sarl	%r8d
	addl	80(%rsp), %r8d
.L1166:
	testl	%r9d, %r9d
	jle	.L1296
	testl	%ecx, %ecx
	jle	.L1296
	movq	80(%rsp), %r14
	movl	%r12d, %eax
	xorl	%edx, %edx
	salq	$32, %rbp
	movl	%eax, %r10d
	movzwl	%r9w, %r9d
	movabsq	$-4294967296, %r15
	salq	$32, %r14
	orq	%r14, %r10
	movq	%r10, %rax
	movq	%rdx, %r10
	movl	%r11d, %edx
	andq	%r15, %r10
	movq	%rax, 176(%rsp)
	leaq	176(%rsp), %rax
	orq	%rdi, %r10
	movq	%rax, 48(%rsp)
	movl	%r10d, %r10d
	orq	%rbp, %r10
	movq	%r10, 184(%rsp)
	movl	40(%rbx), %eax
	movl	%eax, 40(%rsp)
	movzwl	%cx, %eax
	movq	%rsi, %rcx
	movl	%eax, 32(%rsp)
	call	nova_branding_draw_clipped
	movl	8(%rbx), %eax
	movl	16(%rbx), %ecx
	jmp	.L1065
	.p2align 4,,10
	.p2align 3
.L1029:
	cmpw	$1, 124(%rbx)
	je	.L1321
	movl	104(%rbx), %edx
	movl	%edi, %eax
	testl	%edx, %edx
	jle	.L1077
	movslq	112(%rbx), %rax
	movslq	%edi, %rcx
	imulq	%rcx, %rax
	movslq	%edx, %rcx
	cqto
	idivq	%rcx
.L1077:
	leal	(%rbp,%rbp), %r11d
	movl	%r12d, %r15d
	cmpl	%eax, %r11d
	cmovl	%eax, %r11d
	cmpl	%edi, %r11d
	cmovg	%edi, %r11d
	testl	%r8d, %r8d
	jle	.L1081
	movl	%edi, %eax
	movslq	80(%rbx), %rdx
	subl	%r11d, %eax
	cltq
	imulq	%rdx, %rax
	cqto
	idivq	%r8
	leal	(%r12,%rax), %r15d
.L1081:
	movl	80(%rsp), %r14d
	movl	%r12d, %eax
	xorl	%edx, %edx
	movl	%ebp, %r8d
	movl	%eax, %ecx
	shrl	$31, %r8d
	movl	%r11d, 96(%rsp)
	movq	%r14, %r9
	addl	%ebp, %r8d
	movq	%r14, 120(%rsp)
	salq	$32, %r9
	sarl	%r8d
	orq	%r9, %rcx
	movzwl	%r8w, %r8d
	movabsq	$-4294967296, %r9
	movq	%rcx, %rax
	movq	%rdx, %rcx
	leaq	176(%rsp), %rdx
	movl	%r8d, 80(%rsp)
	andq	%r9, %rcx
	movl	88(%rsp), %r9d
	movq	%rdx, 112(%rsp)
	orq	%rdi, %rcx
	movl	%ebp, %edi
	salq	$32, %rbp
	movq	%rax, 176(%rsp)
	movl	%ecx, %ecx
	movq	%rdi, 136(%rsp)
	orq	%rcx, %rbp
	movq	%rsi, %rcx
	movq	%rbp, 184(%rsp)
	call	control_rounded_rect
	movl	96(%rsp), %r11d
	movl	80(%rsp), %r8d
.L1082:
	movq	120(%rsp), %r9
	movl	%r15d, %eax
	xorl	%edx, %edx
	movl	%eax, %ecx
	salq	$32, %r9
	orq	%r9, %rcx
	movabsq	$-4294967296, %r9
	movq	%rcx, %rax
	movq	%rdx, %rcx
	andq	%r9, %rcx
	movq	136(%rsp), %r9
	movq	%rax, 176(%rsp)
	orq	%r11, %rcx
	salq	$32, %r9
	movl	%ecx, %ecx
	orq	%r9, %rcx
	movq	%rcx, 184(%rsp)
	movl	44(%rbx), %r9d
.L1295:
	movq	112(%rsp), %rdx
	movq	%rsi, %rcx
	call	control_rounded_rect
	movl	8(%rbx), %eax
	movl	16(%rbx), %ecx
	jmp	.L1065
	.p2align 4,,10
	.p2align 3
.L1026:
	movl	80(%rsp), %ecx
	movl	%r12d, %eax
	xorl	%edx, %edx
	movabsq	$-4294967296, %r13
	movq	%rax, 128(%rsp)
	leaq	176(%rsp), %r15
	movq	%rcx, %r8
	movq	%rcx, 120(%rsp)
	movq	%rcx, %r14
	movl	%eax, %ecx
	salq	$32, %r8
	movl	%r11d, 156(%rsp)
	orq	%r8, %rcx
	movl	%edi, %r8d
	movq	%r15, 112(%rsp)
	movq	%rcx, %rax
	movq	%rdx, %rcx
	movl	%r10d, 144(%rsp)
	andq	%r13, %rcx
	movq	%rax, 176(%rsp)
	orq	%r8, %rcx
	movq	%rbp, %r8
	movq	%rcx, %rdx
	movl	%ebp, %ecx
	salq	$32, %r8
	movq	%rcx, 136(%rsp)
	movl	%edx, %ecx
	movq	%r15, %rdx
	orq	%r8, %rcx
	movl	96(%rsp), %r8d
	movq	%rcx, 184(%rsp)
	movl	48(%rbx), %r9d
	movq	%rsi, %rcx
	call	control_rounded_rect
	movl	104(%rsp), %r9d
	xorl	%edx, %edx
	leal	(%r12,%r9), %eax
	leal	(%r9,%r14), %r8d
	movl	%eax, %ecx
	salq	$32, %r8
	orq	%r8, %rcx
	movq	%rcx, %rax
	leal	(%r9,%r9), %ecx
	subl	%ecx, %edi
	subl	%ecx, %ebp
	testl	%edi, %edi
	jle	.L1083
	testl	%ebp, %ebp
	jle	.L1083
	movzwl	144(%rsp), %ecx
	subw	156(%rsp), %cx
	xorl	%r8d, %r8d
	movq	%rax, 176(%rsp)
	cmpl	96(%rsp), %r9d
	movzwl	%cx, %ecx
	movl	88(%rsp), %r9d
	cmovl	%ecx, %r8d
	movq	%rdx, %rcx
	salq	$32, %rbp
	movq	%r15, %rdx
	andq	%r13, %rcx
	orq	%rdi, %rcx
	movl	%ecx, %ecx
	orq	%rbp, %rcx
	movq	%rcx, 184(%rsp)
	movq	%rsi, %rcx
	call	control_rounded_rect
.L1083:
	movzwl	124(%rbx), %eax
	cmpw	$2, %ax
	je	.L1322
	cmpw	$3, %ax
	je	.L1323
	cmpw	$4, %ax
	je	.L1324
	subl	$1, %eax
	cmpw	$3, %ax
	ja	.L1296
	movl	44(%rbx), %r9d
	.p2align 4
	.p2align 3
.L1086:
	movq	128(%rsp), %rax
	movq	120(%rsp), %r8
	xorl	%edx, %edx
	salq	$32, %r8
	movl	%eax, %ecx
	orq	%r8, %rcx
	movabsq	$-4294967296, %r8
	movq	%rcx, %rax
	movq	%rdx, %rcx
	movq	112(%rsp), %rdx
	andq	%r8, %rcx
	movq	136(%rsp), %r8
	movq	%rax, 176(%rsp)
	orq	$4, %rcx
	salq	$32, %r8
	movl	%ecx, %ecx
	orq	%r8, %rcx
	movl	$2, %r8d
	movq	%rcx, 184(%rsp)
	movq	%rsi, %rcx
	call	control_rounded_rect
	jmp	.L1296
	.p2align 4,,10
	.p2align 3
.L1024:
	cmpw	$6, 124(%rbx)
	ja	.L1120
	movzwl	124(%rbx), %eax
	leaq	.L1122(%rip), %rdx
	movslq	(%rdx,%rax,4), %rax
	addq	%rdx, %rax
	jmp	*%rax
	.section .rdata,"dr"
	.align 4
.L1122:
	.long	.L1120-.L1122
	.long	.L1126-.L1122
	.long	.L1125-.L1122
	.long	.L1124-.L1122
	.long	.L1124-.L1122
	.long	.L1123-.L1122
	.long	.L1121-.L1122
	.text
.L1124:
	movl	68(%rbx), %r9d
	.p2align 4
	.p2align 3
.L1127:
	movl	80(%rsp), %r15d
	movl	%r12d, %eax
	xorl	%edx, %edx
	movl	%r9d, 88(%rsp)
	movabsq	$-4294967296, %r11
	movl	%eax, %ecx
	movq	%r15, %r8
	salq	$32, %r8
	orq	%r8, %rcx
	movl	%edi, %r8d
	movq	%rcx, %rax
	movq	%rdx, %rcx
	leaq	176(%rsp), %rdx
	andq	%r11, %rcx
	movq	%rax, 176(%rsp)
	movq	%rdx, %r13
	orq	%r8, %rcx
	movq	%rbp, %r8
	salq	$32, %r8
	movl	%ecx, %ecx
	orq	%r8, %rcx
	movl	%ebp, %r8d
	shrl	$31, %r8d
	movq	%rcx, 184(%rsp)
	movq	%rsi, %rcx
	addl	%ebp, %r8d
	sarl	%r8d
	movzwl	%r8w, %r8d
	call	control_rounded_rect
	movl	104(%rsp), %r14d
	movl	%edi, %r10d
	xorl	%edx, %edx
	leal	(%r12,%r14), %eax
	leal	(%r14,%r15), %r8d
	salq	$32, %r8
	movl	%eax, %ecx
	orq	%r8, %rcx
	movl	%ebp, %r8d
	movq	%rcx, %rax
	leal	(%r14,%r14), %ecx
	subl	%ecx, %r8d
	subl	%ecx, %r10d
	testl	%r8d, %r8d
	jle	.L1128
	testl	%r10d, %r10d
	movl	88(%rsp), %r9d
	movabsq	$-4294967296, %r11
	jle	.L1128
	andq	%rdx, %r11
	movl	%r10d, %ecx
	andl	$16777215, %r9d
	movq	%r13, %rdx
	orq	%r11, %rcx
	movq	%r8, %r11
	sarl	%r8d
	orl	$855638016, %r9d
	salq	$32, %r11
	movl	%ecx, %ecx
	movzwl	%r8w, %r8d
	movq	%rax, 176(%rsp)
	orq	%r11, %rcx
	movq	%rcx, 184(%rsp)
	movq	%rsi, %rcx
	call	control_rounded_rect
.L1128:
	movzwl	76(%rbx), %eax
	testw	%ax, %ax
	je	.L1187
	leal	(%rax,%rax), %edx
.L1129:
	subl	%edx, %edi
	movl	84(%rbx), %edx
	leal	(%r12,%rax), %r10d
	movl	%edi, %r11d
	cmpl	$14, %edx
	jbe	.L1325
.L1130:
	movzbl	133(%rbx), %eax
	cmpw	$4, 124(%rbx)
	je	.L1131
	testb	%al, %al
	je	.L1296
.L1134:
	leaq	133(%rbx), %rdx
.L1133:
	subl	$20, %ebp
	movl	$1, 56(%rsp)
	movl	80(%rsp), %edi
	movl	%r11d, %r9d
	movl	$1, 48(%rsp)
	movl	%ebp, %eax
	movl	40(%rbx), %ecx
	shrl	$31, %eax
	movq	%rdx, 32(%rsp)
	movl	%r10d, %edx
	movl	%ecx, 40(%rsp)
	addl	%ebp, %eax
	movq	%rsi, %rcx
	sarl	%eax
	leal	(%rax,%rdi), %r8d
	call	nova_text_draw
	jmp	.L1296
.L1125:
	movl	64(%rbx), %r9d
	jmp	.L1127
.L1126:
	movl	60(%rbx), %r9d
	jmp	.L1127
.L1121:
	movl	44(%rbx), %r9d
	jmp	.L1127
.L1123:
	movl	52(%rbx), %r9d
	jmp	.L1127
	.p2align 4,,10
	.p2align 3
.L1315:
	andl	$4, %edx
	jne	.L1147
	leaq	px.1(%rip), %rax
	movsbl	(%rax,%rdi), %edx
	leaq	py.0(%rip), %rax
	movsbl	(%rax,%rdi), %ecx
	imull	%r12d, %edx
	imull	%r12d, %ecx
	movslq	%edx, %r8
	sarl	$31, %edx
	imulq	$954437177, %r8, %r8
	sarq	$34, %r8
	subl	%edx, %r8d
	movslq	%ecx, %rdx
	sarl	$31, %ecx
	imulq	$954437177, %rdx, %rdx
	addl	%r14d, %r8d
	addl	%esi, %r8d
	sarq	$34, %rdx
	subl	%ecx, %edx
	addl	%r13d, %edx
	addl	%esi, %edx
	jmp	.L1177
	.p2align 4,,10
	.p2align 3
.L1046:
	leaq	133(%rbx), %r8
	leaq	204(%rsp), %rdx
	movl	96(%rsp), %eax
	movq	%r8, 208(%rsp)
	movq	%rdx, 88(%rsp)
	cmpb	$0, 133(%rbx)
	je	.L1045
	movl	%ebp, 104(%rsp)
	movl	%eax, %ebp
	movq	%rsi, 392(%rsp)
	movq	%rbx, %rsi
	movq	%r8, %rbx
	jmp	.L1048
	.p2align 4,,10
	.p2align 3
.L1326:
	movq	208(%rsp), %rbx
	cmpb	$0, (%rbx)
	je	.L1287
.L1048:
	movq	88(%rsp), %rdx
	leaq	208(%rsp), %rcx
	call	nova_unicode_next
	testb	%al, %al
	jne	.L1050
	cmpq	%rbx, 208(%rsp)
	je	.L1287
.L1050:
	movabsq	$25769803782, %rdx
	movl	%ebp, %eax
	leal	7(%r15), %r8d
	addl	$10, %ebp
	movl	%eax, %ecx
	salq	$32, %r8
	movq	%rdx, 184(%rsp)
	movq	112(%rsp), %rdx
	orq	%r8, %rcx
	movl	$3, %r8d
	movq	%rcx, 176(%rsp)
	movq	392(%rsp), %rcx
	movl	40(%rsi), %r9d
	call	control_rounded_rect
	leal	(%rdi,%r12), %eax
	subl	%r14d, %eax
	subl	$5, %eax
	cmpl	%ebp, %eax
	jg	.L1326
.L1287:
	movq	%rsi, %rbx
	movl	104(%rsp), %ebp
	movq	392(%rsp), %rsi
	jmp	.L1045
	.p2align 4,,10
	.p2align 3
.L1104:
	movq	80(%rsp), %r8
	movl	%r12d, %eax
	xorl	%edx, %edx
	movabsq	$-4294967296, %r9
	movl	%eax, %ecx
	salq	$32, %r8
	orq	%r8, %rcx
	movl	%edi, %r8d
	movq	%rcx, %rax
	movq	%rdx, %rcx
	leaq	176(%rsp), %rdx
	andq	%r9, %rcx
	movl	88(%rsp), %r9d
	movq	%rax, 176(%rsp)
	orq	%r8, %rcx
	movq	%rbp, %r8
	salq	$32, %r8
	movl	%ecx, %ecx
	orq	%r8, %rcx
	movl	96(%rsp), %r8d
	movq	%rcx, 184(%rsp)
	movq	%rsi, %rcx
	call	control_rounded_rect
	jmp	.L1107
	.p2align 4,,10
	.p2align 3
.L1090:
	movl	48(%rbx), %r9d
	jmp	.L1091
	.p2align 4,,10
	.p2align 3
.L1187:
	movl	$16, %edx
	movl	$8, %eax
	jmp	.L1129
	.p2align 4,,10
	.p2align 3
.L1111:
	movl	48(%rbx), %eax
	movl	%eax, 88(%rsp)
.L1113:
	movq	80(%rsp), %r8
	movl	%r12d, %eax
	xorl	%edx, %edx
	movabsq	$-4294967296, %r9
	movl	%eax, %ecx
	salq	$32, %r8
	orq	%r8, %rcx
	movl	%edi, %r8d
	movq	%rcx, %rax
	movq	%rdx, %rcx
	andq	%r9, %rcx
	movq	%rax, 176(%rsp)
	movl	88(%rsp), %r9d
	leaq	176(%rsp), %rax
	orq	%r8, %rcx
	movq	%rbp, %r8
	movq	%rax, %rdx
	movq	%rax, 112(%rsp)
	salq	$32, %r8
	movl	%ecx, %ecx
	orq	%r8, %rcx
	movl	96(%rsp), %r8d
	movq	%rcx, 184(%rsp)
	movq	%rsi, %rcx
	call	control_rounded_rect
	jmp	.L1116
	.p2align 4,,10
	.p2align 3
.L1185:
	movl	$-24, %ecx
	movl	$8, %edx
	jmp	.L1117
	.p2align 4,,10
	.p2align 3
.L1183:
	movl	$16, %edx
	movl	$8, %eax
	jmp	.L1108
	.p2align 4,,10
	.p2align 3
.L1040:
	testb	$4, %al
	je	.L1042
	movl	44(%rbx), %r8d
	jmp	.L1041
	.p2align 4,,10
	.p2align 3
.L1192:
	movl	$20, %r14d
	movl	$24, %r15d
	movl	$1000, %r9d
	jmp	.L1158
	.p2align 4,,10
	.p2align 3
.L1164:
	testb	$4, %ch
	je	.L1165
	movl	60(%rbx), %edx
	jmp	.L1165
	.p2align 4,,10
	.p2align 3
.L1072:
	movabsq	$2361183241434822607, %rax
	movl	$16, %ecx
	cmpl	%ecx, %ebp
	cmovle	%ebp, %ecx
	subl	%ecx, %edi
	subl	%ecx, %ebp
	movslq	%edi, %r8
	movl	80(%rsp), %edi
	imulq	%r10, %r8
	movabsq	$-4294967296, %r10
	imulq	%r8
	sarq	$63, %r8
	sarq	$7, %rdx
	subq	%r8, %rdx
	movl	%ebp, %r8d
	shrl	$31, %r8d
	leal	(%r12,%rdx), %eax
	xorl	%edx, %edx
	addl	%ebp, %r8d
	sarl	%r8d
	leal	(%r8,%rdi), %r9d
	movl	%eax, %r8d
	salq	$32, %r9
	orq	%r9, %r8
	movl	%ecx, %r9d
	movq	%r8, %rax
	movq	%rdx, %r8
	andq	%r10, %r8
	movq	%rax, 176(%rsp)
	orq	%r9, %r8
	salq	$32, %r9
	movl	%r8d, %r8d
	orq	%r9, %r8
	movq	%r8, 184(%rsp)
	movl	%ecx, %r8d
	movl	40(%rbx), %r9d
	shrl	$31, %r8d
	addl	%ecx, %r8d
	sarl	%r8d
	movzwl	%r8w, %r8d
	jmp	.L1295
	.p2align 4,,10
	.p2align 3
.L1159:
	movzwl	%ax, %eax
	movl	%r9d, 56(%rsp)
	movl	80(%rsp), %r8d
	movq	%rsi, %rcx
	leaq	133(%rbx), %rdx
	movl	%r11d, 48(%rsp)
	movl	%edi, %r9d
	movq	%rdx, 40(%rsp)
	movl	%r12d, %edx
	movl	%eax, 32(%rsp)
	call	nova_text_draw_wrapped_scaled
	movl	8(%rbx), %eax
	movl	16(%rbx), %ecx
	jmp	.L1065
.L1136:
	movl	%edi, %r8d
	leaq	133(%rbx), %rdx
	leaq	160(%rsp), %rcx
	shrl	$31, %r8d
	movq	%rdx, %r14
	addl	%edi, %r8d
	sarl	%r8d
	call	nova_text_measure
	movzwl	76(%rbx), %r9d
	movl	$8, %eax
	movq	160(%rsp), %r11
	testw	%r9w, %r9w
	cmove	%eax, %r9d
	movl	%edi, %eax
	subl	%r11d, %eax
	movl	%eax, %ecx
	shrl	$31, %ecx
	addl	%eax, %ecx
	sarl	%ecx
	subl	%r9d, %ecx
	testl	%ecx, %ecx
	jle	.L1138
	movl	%ebp, %r8d
	movl	%r12d, %eax
	xorl	%edx, %edx
	movq	%r11, 96(%rsp)
	shrl	$31, %r8d
	movl	%r9d, 88(%rsp)
	addl	%ebp, %r8d
	sarl	%r8d
	addl	80(%rsp), %r8d
	movq	%r8, %r10
	movl	%eax, %r8d
	salq	$32, %r10
	orq	%r10, %r8
	movabsq	$-4294967296, %r10
	movq	%r8, %rax
	movl	%ecx, %r8d
	movq	%rdx, %rcx
	andq	%r10, %rcx
	movq	%rax, 176(%rsp)
	leaq	176(%rsp), %rdx
	orq	%r8, %rcx
	movq	104(%rsp), %r8
	movl	%ecx, %ecx
	salq	$32, %r8
	orq	%r8, %rcx
	movq	%rcx, 184(%rsp)
	movl	48(%rbx), %r8d
	movq	%rsi, %rcx
	call	nova_surface_rect
	movq	96(%rsp), %r11
	movl	88(%rsp), %r9d
.L1138:
	addl	%edi, %r11d
	leal	(%rdi,%r12), %edx
	movl	%r11d, %eax
	shrl	$31, %eax
	addl	%r11d, %eax
	sarl	%eax
	addl	%r12d, %eax
	addl	%r9d, %eax
	cmpl	%eax, %edx
	jg	.L1327
.L1139:
	subl	$20, %ebp
	movl	$1, 56(%rsp)
	movl	%edi, %r9d
	movl	80(%rsp), %edi
	movl	$1, 48(%rsp)
	movl	%ebp, %eax
	movl	40(%rbx), %edx
	movq	%rsi, %rcx
	shrl	$31, %eax
	movq	%r14, 32(%rsp)
	movl	%edx, 40(%rsp)
	addl	%ebp, %eax
	movl	%r12d, %edx
	sarl	%eax
	leal	(%rax,%rdi), %r8d
	call	nova_text_draw
	movl	8(%rbx), %eax
	movl	16(%rbx), %ecx
	jmp	.L1065
.L1162:
	movl	48(%rbx), %edx
	jmp	.L1163
.L1322:
	movl	64(%rbx), %r9d
	jmp	.L1086
.L1316:
	movl	56(%rbx), %r8d
	jmp	.L1095
.L1310:
	testb	$4, %ch
	je	.L1103
	movl	44(%rbx), %eax
	movl	%eax, 88(%rsp)
	jmp	.L1103
.L1304:
	movl	68(%rbx), %r11d
	jmp	.L1153
.L1321:
	movl	108(%rbx), %edx
	movl	%ebp, %eax
	testl	%edx, %edx
	jle	.L1076
	movslq	116(%rbx), %rax
	movslq	%ebp, %rcx
	imulq	%rcx, %rax
	movslq	%edx, %rcx
	cqto
	idivq	%rcx
.L1076:
	movl	80(%rsp), %r13d
	leal	(%rdi,%rdi), %edx
	cmpl	%eax, %edx
	cmovl	%eax, %edx
	cmpl	%ebp, %edx
	cmovg	%ebp, %edx
	movl	%edx, %r14d
	testl	%r8d, %r8d
	jle	.L1079
	movl	%ebp, %eax
	subl	%edx, %eax
	movslq	80(%rbx), %rdx
	cltq
	imulq	%rdx, %rax
	cqto
	idivq	%r8
	movl	80(%rsp), %edx
	leal	(%rdx,%rax), %r13d
.L1079:
	movq	80(%rsp), %r9
	movl	%r12d, %eax
	xorl	%edx, %edx
	movl	%edi, %r8d
	shrl	$31, %r8d
	movl	%eax, %ecx
	salq	$32, %rbp
	movl	%r12d, %r15d
	salq	$32, %r9
	addl	%edi, %r8d
	movabsq	$-4294967296, %r11
	orq	%r9, %rcx
	movl	%edi, %r9d
	sarl	%r8d
	movq	%rcx, %rax
	movq	%rdx, %rcx
	movzwl	%r8w, %r8d
	andq	%r11, %rcx
	leaq	176(%rsp), %rdx
	movl	%r8d, 80(%rsp)
	orq	%r9, %rcx
	movl	88(%rsp), %r9d
	movq	%rdx, 112(%rsp)
	movl	%ecx, %ecx
	movq	%rax, 176(%rsp)
	orq	%rcx, %rbp
	movq	%rsi, %rcx
	movq	%rbp, 184(%rsp)
	call	control_rounded_rect
	movl	%r13d, %eax
	movl	80(%rsp), %r8d
	movl	%edi, %r11d
	movq	%rax, 120(%rsp)
	movl	%r14d, %eax
	movq	%rax, 136(%rsp)
	jmp	.L1082
.L1131:
	testb	%al, %al
	jne	.L1134
	leaq	.LC19(%rip), %rdx
	jmp	.L1133
.L1150:
	movl	48(%rbx), %r11d
	jmp	.L1151
.L1110:
	subl	$20, %ebp
	movl	80(%rsp), %edi
	movq	%rsi, %rcx
	movl	%r11d, %r9d
	movl	$1, 56(%rsp)
	movl	%ebp, %eax
	movl	$1, 48(%rsp)
	movl	40(%rbx), %edx
	shrl	$31, %eax
	addl	%ebp, %eax
	movl	%edx, 40(%rsp)
	leaq	133(%rbx), %rdx
	sarl	%eax
	movq	%rdx, 32(%rsp)
	leal	(%rax,%rdi), %r8d
	movl	%r10d, %edx
	call	nova_text_draw
	movl	8(%rbx), %eax
	movl	16(%rbx), %ecx
	jmp	.L1065
.L1311:
	movl	$12, %ecx
	movl	%edi, 96(%rsp)
	leal	-12(%rbp), %edi
	movl	%ebp, %r8d
	cmpl	%ecx, %edi
	movl	%eax, 104(%rsp)
	movl	80(%rsp), %eax
	cmovl	%ecx, %edi
	movl	%r10d, 88(%rsp)
	subl	%edi, %r8d
	movl	%r8d, %ecx
	shrl	$31, %ecx
	addl	%r8d, %ecx
	movl	40(%rbx), %r8d
	sarl	%ecx
	movl	%r8d, 40(%rsp)
	movzwl	%di, %r8d
	leal	(%rcx,%rax), %r9d
	movq	%rsi, %rcx
	movl	%r8d, 32(%rsp)
	movl	%r10d, %r8d
	call	nova_icon_draw
	movl	104(%rsp), %eax
	movl	88(%rsp), %r10d
	movl	96(%rsp), %r11d
	addl	%edi, %eax
	addl	%eax, %r10d
	subl	%eax, %r11d
	jmp	.L1109
.L1307:
	movq	80(%rsp), %r8
	movl	%r12d, %eax
	xorl	%edx, %edx
	movl	%r11d, 88(%rsp)
	movabsq	$-4294967296, %r15
	movl	%eax, %ecx
	salq	$32, %r8
	orq	%r8, %rcx
	movl	%edi, %r8d
	movq	%rcx, %rax
	movq	%rdx, %rcx
	leaq	176(%rsp), %rdx
	andq	%r15, %rcx
	movq	%rax, 176(%rsp)
	orq	%r8, %rcx
	movq	%rbp, %r8
	salq	$32, %r8
	movl	%ecx, %ecx
	orq	%r8, %rcx
	movl	96(%rsp), %r8d
	movq	%rcx, 184(%rsp)
	movq	%rsi, %rcx
	call	control_rounded_rect
	movl	88(%rsp), %r11d
	jmp	.L1156
.L1325:
	movl	$10, %ecx
	movl	%edi, 96(%rsp)
	leal	-8(%rbp), %edi
	movl	%ebp, %r8d
	cmpl	%ecx, %edi
	movl	%eax, 104(%rsp)
	movl	80(%rsp), %eax
	cmovl	%ecx, %edi
	movl	%r10d, 88(%rsp)
	subl	%edi, %r8d
	movl	%r8d, %ecx
	shrl	$31, %ecx
	addl	%r8d, %ecx
	movl	40(%rbx), %r8d
	sarl	%ecx
	movl	%r8d, 40(%rsp)
	movzwl	%di, %r8d
	leal	(%rcx,%rax), %r9d
	movq	%rsi, %rcx
	movl	%r8d, 32(%rsp)
	movl	%r10d, %r8d
	call	nova_icon_draw
	movl	104(%rsp), %eax
	movl	88(%rsp), %r10d
	movl	96(%rsp), %r11d
	sarl	%eax
	addl	%edi, %eax
	addl	%eax, %r10d
	subl	%eax, %r11d
	jmp	.L1130
.L1313:
	testl	%ebp, %ebp
	leal	3(%rbp), %eax
	movl	%ebp, %r11d
	movl	80(%rsp), %r15d
	cmovns	%ebp, %eax
	shrl	$31, %r11d
	xorl	%r9d, %r9d
	movq	112(%rsp), %r14
	addl	%ebp, %r11d
	movq	%rsi, %rcx
	sarl	%r11d
	sarl	$2, %eax
	leal	(%r11,%r15), %edx
	leal	(%r12,%rax), %r8d
	movl	%eax, %r10d
	movl	%r11d, %ebp
	salq	$32, %rdx
	movl	%r8d, %eax
	movl	%r10d, 80(%rsp)
	orq	%rdx, %rax
	movl	%r10d, %edx
	movq	%rax, %r8
	movq	%r9, %rax
	andq	%rdi, %rax
	movq	%r8, 176(%rsp)
	orq	%rdx, %rax
	movq	%rax, %r9
	movl	104(%rsp), %eax
	leal	1(%rax), %r13d
	movl	%r9d, %eax
	movq	%r13, %rdx
	salq	$32, %rdx
	orq	%rdx, %rax
	movq	%r14, %rdx
	movq	%rax, 184(%rsp)
	movl	44(%rbx), %r8d
	call	nova_surface_rect
	movl	80(%rsp), %r10d
	xorl	%edx, %edx
	movq	%rbp, %r11
	leal	-1(%r12,%rbp), %eax
	salq	$32, %r11
	leal	(%r15,%r10), %r8d
	movl	%eax, %ecx
	salq	$32, %r8
	orq	%r8, %rcx
	movq	%rcx, %rax
	movq	%rdx, %rcx
	movq	%r14, %rdx
	andq	%rdi, %rcx
	movq	%rax, 176(%rsp)
	orq	%r13, %rcx
	movl	%ecx, %ecx
	orq	%rcx, %r11
	movq	%rsi, %rcx
	movq	%r11, 184(%rsp)
	movl	44(%rbx), %r8d
	call	nova_surface_rect
	movl	16(%rbx), %ecx
	jmp	.L1100
.L1314:
	movq	392(%rsp), %rsi
	movl	8(%rbx), %eax
	movl	16(%rbx), %ecx
	jmp	.L1065
.L1303:
	movzwl	520(%rbx), %eax
	cmpl	$7, 8(%rbx)
	jne	.L1060
	leal	(%rax,%rax,4), %eax
	addl	%eax, %eax
.L1061:
	addl	%r12d, %edi
	addl	96(%rsp), %eax
	subl	%r14d, %edi
	cmpl	%eax, %edi
	jle	.L1289
	movl	80(%rsp), %edi
	movl	%eax, %ecx
	xorl	%edx, %edx
	leal	5(%rdi), %r8d
	salq	$32, %r8
	orq	%r8, %rcx
	movabsq	$-4294967296, %r8
	movq	%rcx, %rax
	movq	%rdx, %rcx
	andq	%r8, %rcx
	leal	-10(%rbp), %r8d
	orq	$1, %rcx
	movq	%rcx, %rdx
	jmp	.L1298
.L1188:
	movl	$-1, %r11d
	movl	$2, %r15d
	jmp	.L1140
.L1042:
	movl	48(%rbx), %r8d
	jmp	.L1041
.L1305:
	movl	64(%rbx), %r11d
	jmp	.L1153
.L1317:
	movl	60(%rbx), %r8d
	jmp	.L1095
.L1323:
	movl	68(%rbx), %r9d
	jmp	.L1086
.L1309:
	movl	%edi, %eax
	movq	80(%rsp), %r8
	xorl	%edx, %edx
	salq	$32, %rbp
	movabsq	$-4294967296, %r9
	shrl	$31, %eax
	addl	%edi, %eax
	salq	$32, %r8
	sarl	%eax
	addl	%r12d, %eax
	movl	%eax, %eax
	movl	%eax, %ecx
	orq	%r8, %rcx
	movl	104(%rsp), %r8d
	movq	%rcx, %rax
	movq	%rdx, %rcx
	andq	%r9, %rcx
	movq	%rax, 176(%rsp)
	orq	%r8, %rcx
	movl	%ecx, %ecx
	orq	%rcx, %rbp
	movq	%rbp, 184(%rsp)
	jmp	.L1297
.L1308:
	subl	%r14d, %ebp
	movl	%ebp, %eax
	shrl	$31, %eax
	addl	%ebp, %eax
	sarl	%eax
	addl	%eax, 80(%rsp)
	jmp	.L1161
.L1285:
	movl	$-85, %edx
	movl	%ebp, %eax
	leal	(%r15,%r15,2), %r8d
	addl	%r11d, %r13d
	mulb	%dl
	leal	(%r14,%r11), %edi
	movl	%r15d, %ebp
	movl	%r13d, %r13d
	leaq	176(%rsp), %rdx
	movq	%rbx, 384(%rsp)
	subl	%r8d, %edi
	xorl	%r12d, %r12d
	movq	%rdx, 112(%rsp)
	movl	%r8d, %ebx
	xorl	%edx, %edx
	shrw	$9, %ax
	movw	%ax, 80(%rsp)
	movl	%r15d, %eax
	movabsq	$-4294967296, %r15
	sarl	%eax
	movzwl	%ax, %eax
	movl	%eax, 96(%rsp)
	xorl	%eax, %eax
	andq	%rax, %r15
.L1143:
	movl	88(%rsp), %r9d
	cmpb	%r12b, 80(%rsp)
	jne	.L1142
	movq	384(%rsp), %rcx
	movl	44(%rcx), %r9d
.L1142:
	movl	%edi, %ecx
	movq	%r13, %r8
	addl	$1, %r12d
	addl	%ebx, %edi
	orq	%r15, %rcx
	salq	$32, %r8
	movabsq	$-4294967296, %r14
	movl	%ecx, %ecx
	orq	%r8, %rcx
	movq	%rbp, %r8
	movq	%rcx, %rax
	movq	%rdx, %rcx
	salq	$32, %r8
	movq	112(%rsp), %rdx
	andq	%r14, %rcx
	movq	%rax, 176(%rsp)
	orq	%rbp, %rcx
	movl	%ecx, %ecx
	orq	%r8, %rcx
	movl	96(%rsp), %r8d
	movq	%rcx, 184(%rsp)
	movq	%rsi, %rcx
	call	control_rounded_rect
	cmpb	$3, %r12b
	jne	.L1143
	movq	384(%rsp), %rbx
	movl	8(%rbx), %eax
	movl	16(%rbx), %ecx
	jmp	.L1065
.L1312:
	movq	80(%rsp), %r8
	movl	%r12d, %eax
	xorl	%edx, %edx
	movabsq	$-4294967296, %r11
	movl	%eax, %ecx
	salq	$32, %r8
	orq	%r8, %rcx
	movl	%edi, %r8d
	movq	%rcx, %rax
	movq	%rdx, %rcx
	leaq	176(%rsp), %rdx
	andq	%r11, %rcx
	movq	%rax, 176(%rsp)
	orq	%r8, %rcx
	movq	%rbp, %r8
	salq	$32, %r8
	movl	%ecx, %ecx
	orq	%r8, %rcx
	movl	96(%rsp), %r8d
	movq	%rcx, 184(%rsp)
	movq	%rsi, %rcx
	call	control_rounded_rect
	movl	16(%rbx), %ecx
	jmp	.L1101
.L1120:
	movl	56(%rbx), %r9d
	jmp	.L1127
.L1301:
	movl	$1, 56(%rsp)
	leal	(%rcx,%rcx), %eax
	movl	$0, 48(%rsp)
	movl	48(%rbx), %edx
	movl	%edx, 40(%rsp)
	leaq	325(%rbx), %rdx
	jmp	.L1288
.L1318:
	movl	68(%rbx), %r8d
	jmp	.L1095
.L1306:
	movl	60(%rbx), %r11d
	jmp	.L1153
.L1324:
	movl	60(%rbx), %r9d
	jmp	.L1086
.L1302:
	xchgw	%r11w, %ax
.L1053:
	cmpw	%ax, %r9w
	cmova	%eax, %r9d
	testw	%r9w, %r9w
	je	.L1328
	leaq	133(%rbx), %rdx
	movzwl	%r9w, %r8d
	movl	%r11d, 120(%rsp)
	leaq	208(%rsp), %rcx
	movl	%r9d, 104(%rsp)
	movq	%rcx, %r13
	movq	%rdx, 88(%rsp)
	call	memcpy
	movl	104(%rsp), %r9d
	movl	120(%rsp), %r11d
.L1058:
	movzwl	%r9w, %r9d
	movq	%r13, %rdx
	leaq	160(%rsp), %rax
	movl	$32767, %r8d
	movb	$0, 208(%rsp,%r9)
	movq	%rax, %rcx
	movl	%r11d, 136(%rsp)
	movq	%rax, 128(%rsp)
	call	nova_text_measure
	movq	160(%rsp), %rax
	movl	96(%rsp), %edx
	movl	136(%rsp), %r11d
	movq	%rax, 120(%rsp)
	addl	%edx, %eax
	movl	%eax, 104(%rsp)
	movzwl	518(%rbx), %eax
	cmpw	%ax, %r11w
	cmova	%eax, %r11d
	testw	%r11w, %r11w
	je	.L1057
	movq	88(%rsp), %rdx
	movzwl	%r11w, %r8d
	movq	%r13, %rcx
	movl	%r11d, 136(%rsp)
	call	memcpy
	movl	136(%rsp), %r11d
.L1057:
	movzwl	%r11w, %r11d
	movq	%r13, %rdx
	movq	128(%rsp), %rcx
	movl	$32767, %r8d
	movb	$0, 208(%rsp,%r11)
	call	nova_text_measure
	movl	80(%rsp), %ecx
	movl	104(%rsp), %eax
	xorl	%edx, %edx
	movabsq	$-4294967296, %r9
	movl	44(%rbx), %r11d
	leal	3(%rcx), %r8d
	movl	%eax, %ecx
	salq	$32, %r8
	andl	$16777215, %r11d
	orq	%r8, %rcx
	movq	160(%rsp), %r8
	subl	120(%rsp), %r8d
	orl	$1711276032, %r11d
	movq	%rcx, %rax
	movq	%rdx, %rcx
	movq	112(%rsp), %rdx
	andq	%r9, %rcx
	movq	%rax, 176(%rsp)
	orq	%r8, %rcx
	leal	-6(%rbp), %r8d
	salq	$32, %r8
	movl	%ecx, %ecx
	orq	%r8, %rcx
	movl	%r11d, %r8d
	movq	%rcx, 184(%rsp)
	movq	%rsi, %rcx
	call	nova_surface_rect
	jmp	.L1054
.L1327:
	movl	%ebp, %ecx
	movl	80(%rsp), %r15d
	movl	%eax, %r8d
	xorl	%r9d, %r9d
	shrl	$31, %ecx
	subl	%eax, %edx
	movq	%r9, %rax
	addl	%ebp, %ecx
	sarl	%ecx
	leal	(%rcx,%r15), %r11d
	movl	%r8d, %ecx
	salq	$32, %r11
	orq	%r11, %rcx
	movq	%rcx, %r8
	movabsq	$-4294967296, %rcx
	andq	%rcx, %rax
	movq	%r8, 176(%rsp)
	movq	%rsi, %rcx
	orq	%rdx, %rax
	movq	%rax, %r9
	movq	104(%rsp), %rax
	movl	%r9d, %edx
	salq	$32, %rax
	orq	%rdx, %rax
	leaq	176(%rsp), %rdx
	movq	%rax, 184(%rsp)
	movl	48(%rbx), %r8d
	call	nova_surface_rect
	jmp	.L1139
.L1060:
	movzwl	518(%rbx), %edx
	cmpw	%dx, %ax
	cmovbe	%eax, %edx
	movl	%edx, %r9d
	testw	%dx, %dx
	je	.L1329
	movzwl	%r9w, %r8d
	leaq	208(%rsp), %rcx
	movl	%r9d, 88(%rsp)
	leaq	133(%rbx), %rdx
	movq	%rcx, %r13
	call	memcpy
	movl	88(%rsp), %r9d
.L1063:
	movzwl	%r9w, %eax
	leaq	160(%rsp), %rcx
	movl	$32767, %r8d
	movq	%r13, %rdx
	movb	$0, 208(%rsp,%rax)
	call	nova_text_measure
	movl	160(%rsp), %eax
	jmp	.L1061
.L1193:
	movl	80(%rsp), %r8d
	movl	%r12d, %r11d
	movl	%ebp, %ecx
	movl	%edi, %r9d
	jmp	.L1166
.L1098:
	movl	64(%rbx), %r8d
	jmp	.L1095
.L1319:
	movslq	%edi, %rax
	movzwl	%r15w, %r8d
	imulq	%rax, %rdx
	movslq	%ebp, %rax
	imulq	%r8, %rax
	cmpq	%rax, %rdx
	jge	.L1172
.L1299:
	testl	%r9d, %r9d
	je	.L1296
	imull	%edi, %ecx
	movl	$2, %r8d
	movl	%r12d, %r11d
	movl	%ecx, %eax
	cltd
	idivl	%r9d
	movl	%edi, %r9d
	movl	%eax, %ecx
	movl	%ebp, %eax
	subl	%ecx, %eax
	cltd
	idivl	%r8d
	movl	80(%rsp), %edx
	leal	(%rax,%rdx), %r8d
	jmp	.L1166
.L1320:
	movslq	%edi, %rax
	imulq	%rdx, %rax
	movslq	%ebp, %rdx
	movq	%rax, %r8
	movzwl	%r15w, %eax
	imulq	%rdx, %rax
	cmpq	%rax, %r8
	jg	.L1299
.L1172:
	testl	%ecx, %ecx
	je	.L1296
	imull	%ebp, %r9d
	movl	80(%rsp), %r8d
	movl	%r9d, %eax
	cltd
	idivl	%ecx
	movl	$2, %ecx
	movl	%eax, %r9d
	movl	%edi, %eax
	subl	%r9d, %eax
	cltd
	idivl	%ecx
	movl	%ebp, %ecx
	leal	(%rax,%r12), %r11d
	jmp	.L1166
.L1328:
	leaq	133(%rbx), %rax
	leaq	208(%rsp), %r13
	movq	%rax, 88(%rsp)
	jmp	.L1058
.L1329:
	leaq	208(%rsp), %r13
	jmp	.L1063
	.seh_endproc
	.p2align 4
	.globl	nova_control_diagnostics
	.def	nova_control_diagnostics;	.scl	2;	.type	32;	.endef
	.seh_proc	nova_control_diagnostics
nova_control_diagnostics:
	.seh_endprologue
	leaq	diagnostics(%rip), %rax
	ret
	.seh_endproc
	.section .rdata,"dr"
	.align 32
CSWTCH.45:
	.long	51
	.long	39
	.long	47
	.long	17
	.long	17
	.long	224
	.long	17
	.long	17
	.long	8
	.long	4
	.long	512
	.long	48
	.long	301
	.long	17
	.long	192
	.long	320
	.long	17
	.long	17
	.long	17
	.long	15
	.long	10
	.align 8
py.0:
	.ascii "\371\373\0\5\7\5\0\373"
	.align 8
px.1:
	.ascii "\0\5\7\5\0\373\371\373"
.lcomm templates,1536,32
.lcomm styles,1920,32
.lcomm list_selection_mask,1024,32
.lcomm list_entry_count,256,32
.lcomm list_entries,16384,32
.lcomm diagnostics,36,32
	.data
	.align 2
focused_id:
	.word	-1
.lcomm default_style,44,32
.lcomm used,128,32
.lcomm controls,68096,32
	.section .rdata,"dr"
	.align 4
.LC0:
	.byte	-16
	.byte	-128
	.byte	-128
	.byte	-128
	.align 16
.LC1:
	.long	-14539222
	.long	-723208
	.long	-11746561
	.long	-10065814
	.align 16
.LC2:
	.long	-12168868
	.long	-11746561
	.long	-12003945
	.long	-20448
	.align 8
.LC7:
	.long	1000
	.long	1
	.ident	"GCC: (Rev13, Built by MSYS2 project) 15.2.0"
	.def	nova_surface_rect;	.scl	2;	.type	32;	.endef
	.def	nova_unicode_next;	.scl	2;	.type	32;	.endef
	.def	nova_text_draw;	.scl	2;	.type	32;	.endef
	.def	nova_text_draw_scaled;	.scl	2;	.type	32;	.endef
	.def	nova_icon_draw;	.scl	2;	.type	32;	.endef
	.def	nova_branding_intrinsic_width;	.scl	2;	.type	32;	.endef
	.def	nova_branding_intrinsic_height;	.scl	2;	.type	32;	.endef
	.def	nova_branding_draw_clipped;	.scl	2;	.type	32;	.endef
	.def	nova_text_draw_wrapped_scaled;	.scl	2;	.type	32;	.endef
	.def	nova_text_measure;	.scl	2;	.type	32;	.endef
	.def	memcpy;	.scl	2;	.type	32;	.endef
