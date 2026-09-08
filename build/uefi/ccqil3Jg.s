	.file	"layout.c"
	.text
	.p2align 4
	.def	grid_add_track;	.scl	3;	.type	32;	.endef
grid_add_track:
	movq	%rcx, %rax
	testq	%rcx, %rcx
	je	.L2
	cmpl	$3, %r8d
	ja	.L2
	testl	%r8d, %r8d
	je	.L10
	testw	%r9w, %r9w
	je	.L2
.L10:
	cmpl	$3, %r8d
	jne	.L11
	cmpw	$1000, %r9w
	jbe	.L11
.L2:
	addl	$1, 16+grid_diagnostics(%rip)
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L11:
	testb	%dl, %dl
	jne	.L20
	movzbl	10229(%rax), %edx
	cmpb	$7, %dl
	ja	.L2
	leal	1(%rdx), %ecx
	movb	%cl, 10229(%rax)
	movzbl	%dl, %ecx
	leaq	(%rcx,%rcx,2), %rcx
	movl	%r8d, 128(%rax,%rcx,4)
	movw	%r9w, 132(%rax,%rcx,4)
	movl	$0, 136(%rax,%rcx,4)
.L9:
	movb	$1, 10232(%rax)
	movl	$1, %eax
	addl	$1, 12+grid_diagnostics(%rip)
	ret
	.p2align 4,,10
	.p2align 3
.L20:
	movzbl	10228(%rax), %edx
	cmpb	$7, %dl
	ja	.L2
	leal	1(%rdx), %ecx
	movb	%cl, 10228(%rax)
	movzbl	%dl, %ecx
	leaq	(%rcx,%rcx,2), %rcx
	movl	%r8d, 32(%rax,%rcx,4)
	movw	%r9w, 36(%rax,%rcx,4)
	movl	$0, 40(%rax,%rcx,4)
	jmp	.L9
	.p2align 4
	.def	constraint_axis.isra.0;	.scl	3;	.type	32;	.endef
constraint_axis.isra.0:
	movl	40(%rsp), %eax
	cmpl	$1, %ecx
	je	.L40
	testl	%r9d, %r9d
	jle	.L26
	testl	%ecx, %ecx
	je	.L26
	cmpl	$4, %ecx
	je	.L27
	movl	56(%rsp), %eax
	movslq	%r9d, %r9
	addl	$1, 32+constraint_diagnostics(%rip)
	imulq	%rax, %r9
	movabsq	$2361183241434822607, %rax
	addq	$500, %r9
	shrq	$3, %r9
	mulq	%r9
	shrq	$4, %rdx
	movl	%edx, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L26:
	cmpl	$2, %ecx
	je	.L41
	subl	$3, %ecx
	andl	$-3, %ecx
	jne	.L27
	addl	$1, 36+constraint_diagnostics(%rip)
	movl	48(%rsp), %eax
	ret
	.p2align 4,,10
	.p2align 3
.L40:
	movl	56(%rsp), %eax
	movslq	%edx, %rdx
	imulq	%rax, %rdx
	testq	%rdx, %rdx
	jns	.L42
	leaq	-500(%rdx), %rcx
	movabsq	$2361183241434822607, %rax
	imulq	%rcx
	sarq	$63, %rcx
	movq	%rdx, %rax
	sarq	$7, %rax
	subq	%rcx, %rax
	ret
	.p2align 4,,10
	.p2align 3
.L42:
	leaq	500(%rdx), %rax
	movabsq	$2361183241434822607, %rdx
	shrq	$3, %rax
	mulq	%rdx
	movq	%rdx, %rax
	shrq	$4, %rax
	ret
	.p2align 4,,10
	.p2align 3
.L27:
	movl	56(%rsp), %eax
	movslq	%r8d, %r8
	imulq	%rax, %r8
	testq	%r8, %r8
	jns	.L43
	subq	$500, %r8
	movabsq	$2361183241434822607, %rax
	imulq	%r8
	sarq	$63, %r8
	movq	%rdx, %rax
	sarq	$7, %rax
	subq	%r8, %rax
	ret
	.p2align 4,,10
	.p2align 3
.L41:
	movslq	48(%rsp), %rdx
	movzwl	%ax, %eax
	addl	$1, 40+constraint_diagnostics(%rip)
	imulq	%rax, %rdx
	movabsq	$2361183241434822607, %rax
	shrq	$3, %rdx
	mulq	%rdx
	shrq	$4, %rdx
	movl	%edx, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L43:
	addq	$500, %r8
	movabsq	$2361183241434822607, %rax
	shrq	$3, %r8
	mulq	%r8
	movq	%rdx, %rax
	shrq	$4, %rax
	ret
	.p2align 4
	.def	grid_resolve_tracks.isra.0;	.scl	3;	.type	32;	.endef
grid_resolve_tracks.isra.0:
	testb	%dl, %dl
	je	.L62
	movzbl	%dl, %edx
	pushq	%rdi
	movq	%rcx, %r9
	xorl	%r10d, %r10d
	pushq	%rsi
	leaq	(%rdx,%rdx,2), %rax
	movabsq	$2361183241434822607, %rsi
	pushq	%rbx
	movl	%r8d, %ebx
	leaq	(%rcx,%rax,4), %r11
	xorl	%r8d, %r8d
	movslq	%ebx, %rdi
	jmp	.L49
	.p2align 4,,10
	.p2align 3
.L46:
	cmpl	$2, %eax
	je	.L65
	addl	8(%rcx), %r10d
.L47:
	addq	$12, %rcx
	cmpq	%r11, %rcx
	je	.L66
.L49:
	movl	(%rcx), %eax
	cmpl	$3, %eax
	jne	.L46
	movzwl	4(%rcx), %edx
	addq	$12, %rcx
	imulq	%rdi, %rdx
	shrq	$3, %rdx
	movq	%rdx, %rax
	mulq	%rsi
	shrq	$4, %rdx
	movl	%edx, -4(%rcx)
	addl	%edx, %r10d
	cmpq	%r11, %rcx
	jne	.L49
.L66:
	cmpl	%r10d, %ebx
	jl	.L50
	testl	%r8d, %r8d
	jne	.L55
	testl	%r10d, %r10d
	jns	.L55
.L50:
	popq	%rbx
	xorl	%eax, %eax
	popq	%rsi
	addl	$1, 16+grid_diagnostics(%rip)
	popq	%rdi
	ret
	.p2align 4,,10
	.p2align 3
.L65:
	movzwl	4(%rcx), %eax
	addl	%eax, %r8d
	jmp	.L47
	.p2align 4,,10
	.p2align 3
.L55:
	subl	%r10d, %ebx
	movslq	%ebx, %rbx
	.p2align 6
	.p2align 4
	.p2align 3
.L52:
	cmpl	$2, (%r9)
	jne	.L54
	movzwl	4(%r9), %eax
	imulq	%rbx, %rax
	cqto
	idivq	%r8
	movl	%eax, 8(%r9)
.L54:
	addq	$12, %r9
	cmpq	%r11, %r9
	jne	.L52
	popq	%rbx
	movl	$1, %eax
	popq	%rsi
	popq	%rdi
	ret
	.p2align 4,,10
	.p2align 3
.L62:
	movl	$1, %eax
	ret
	.p2align 4
	.globl	nova_layout_initialize
	.def	nova_layout_initialize;	.scl	2;	.type	32;	.endef
nova_layout_initialize:
	pushq	%rbp
	pxor	%xmm0, %xmm0
	movl	$11, %ecx
	xorl	%eax, %eax
	movq	%rsp, %rbp
	pushq	%rdi
	leaq	constraint_diagnostics(%rip), %rdi
	andq	$-16, %rsp
	movups	%xmm0, 32+alignment_diagnostics(%rip)
	rep stosq
	movups	%xmm0, 16+grid_diagnostics(%rip)
	movq	$0, 32+diagnostics(%rip)
	movl	$0, 48+overlay_diagnostics(%rip)
	movl	$-2147450881, 52+overlay_diagnostics(%rip)
	movl	$0, 48+responsive_state(%rip)
	movups	%xmm0, diagnostics(%rip)
	movups	%xmm0, 16+diagnostics(%rip)
	movups	%xmm0, alignment_diagnostics(%rip)
	movups	%xmm0, 16+alignment_diagnostics(%rip)
	movups	%xmm0, 44+alignment_diagnostics(%rip)
	movups	%xmm0, stack_diagnostics(%rip)
	movups	%xmm0, 16+stack_diagnostics(%rip)
	movups	%xmm0, 32+stack_diagnostics(%rip)
	movups	%xmm0, grid_diagnostics(%rip)
	movups	%xmm0, 28+grid_diagnostics(%rip)
	movups	%xmm0, overlay_diagnostics(%rip)
	movups	%xmm0, 16+overlay_diagnostics(%rip)
	movups	%xmm0, 32+overlay_diagnostics(%rip)
	movups	%xmm0, responsive_state(%rip)
	movups	%xmm0, 16+responsive_state(%rip)
	movups	%xmm0, 32+responsive_state(%rip)
	movups	%xmm0, responsive_diagnostics(%rip)
	movups	%xmm0, 16+responsive_diagnostics(%rip)
	movups	%xmm0, 32+responsive_diagnostics(%rip)
	movups	%xmm0, 48+responsive_diagnostics(%rip)
	movl	$0, 64+responsive_diagnostics(%rip)
	movq	-8(%rbp), %rdi
	leave
	ret
	.p2align 4
	.globl	nova_layout_apply_alignment
	.def	nova_layout_apply_alignment;	.scl	2;	.type	32;	.endef
nova_layout_apply_alignment:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rdi
	pushq	%rsi
	pushq	%rbx
	andq	$-16, %rsp
	subq	$48, %rsp
	testq	%rcx, %rcx
	je	.L70
	movq	%rdx, %r11
	testq	%rdx, %rdx
	je	.L70
	testq	%r8, %r8
	je	.L70
	cmpq	$0, 48(%rbp)
	je	.L70
	movl	8(%rcx), %esi
	movq	%rcx, %r10
	testl	%esi, %esi
	js	.L70
	movl	12(%rcx), %ebx
	testl	%ebx, %ebx
	js	.L70
	movl	8(%rdx), %ecx
	testl	%ecx, %ecx
	js	.L70
	movl	12(%rdx), %edx
	testl	%edx, %edx
	js	.L70
	leal	-500(%r9), %eax
	cmpl	$3500, %eax
	ja	.L70
	cmpl	$3, (%r8)
	ja	.L70
	cmpl	$4, 4(%r8)
	ja	.L70
	cmpl	$4, 48(%r8)
	ja	.L70
	movl	40(%r8), %eax
	testl	%eax, %eax
	js	.L70
	movl	12(%r8), %eax
	orl	8(%r8), %eax
	orl	16(%r8), %eax
	orl	20(%r8), %eax
	js	.L70
	movl	28(%r8), %eax
	orl	24(%r8), %eax
	orl	32(%r8), %eax
	orl	36(%r8), %eax
	js	.L70
	movzbl	52(%r8), %edx
	movl	%r9d, %ecx
	movq	%r11, 24(%rbp)
	movq	%r10, 16(%rbp)
	movq	%r8, 32(%rbp)
	call	nova_scaling_effective
	movq	32(%rbp), %r8
	movq	16(%rbp), %r10
	movl	%eax, %ebx
	movl	%eax, 40(%rsp)
	movq	24(%rbp), %r11
	movslq	8(%r8), %rax
	movq	%rbx, %rdi
	imulq	%rbx, %rax
	testq	%rax, %rax
	jns	.L108
	subq	$500, %rax
	movl	$1000, %ecx
	cqto
	idivq	%rcx
.L74:
	movl	%eax, %r9d
	movslq	12(%r8), %rax
	imulq	%rdi, %rax
	testq	%rax, %rax
	jns	.L109
	subq	$500, %rax
	movl	$1000, %ecx
	cqto
	idivq	%rcx
.L76:
	movl	%eax, %ecx
	movslq	16(%r8), %rax
	imulq	%rdi, %rax
	testq	%rax, %rax
	jns	.L110
	subq	$500, %rax
	movl	$1000, %esi
	cqto
	idivq	%rsi
.L78:
	movl	%eax, %r15d
	movslq	20(%r8), %rax
	imulq	%rdi, %rax
	testq	%rax, %rax
	jns	.L111
	subq	$500, %rax
	movl	$1000, %esi
	cqto
	idivq	%rsi
.L80:
	movl	%eax, %r12d
	movslq	24(%r8), %rax
	imulq	%rdi, %rax
	testq	%rax, %rax
	jns	.L112
	subq	$500, %rax
	movl	$1000, %esi
	cqto
	idivq	%rsi
.L82:
	movl	%eax, %r13d
	movslq	28(%r8), %rax
	imulq	%rdi, %rax
	testq	%rax, %rax
	jns	.L113
	subq	$500, %rax
	movl	$1000, %esi
	cqto
	idivq	%rsi
.L84:
	movl	%eax, %r14d
	movslq	32(%r8), %rax
	imulq	%rdi, %rax
	testq	%rax, %rax
	jns	.L114
	subq	$500, %rax
	movl	$1000, %esi
	cqto
	idivq	%rsi
.L86:
	movl	%eax, %esi
	movslq	36(%r8), %rax
	imulq	%rdi, %rax
	testq	%rax, %rax
	jns	.L115
	subq	$500, %rax
	movl	$1000, %ebx
	cqto
	idivq	%rbx
.L88:
	movl	8(%r10), %edx
	subl	%r9d, %edx
	subl	%r15d, %edx
	subl	%r13d, %edx
	movl	%edx, %ebx
	movl	12(%r10), %edx
	subl	%esi, %ebx
	subl	%ecx, %edx
	movl	%ebx, %r15d
	subl	%r12d, %edx
	subl	%r14d, %edx
	subl	%eax, %edx
	orl	%edx, %ebx
	movl	%edx, %r12d
	js	.L99
	movl	4(%r8), %eax
	movl	(%r8), %edx
	addl	(%r10), %r9d
	addl	4(%r10), %ecx
	movl	%eax, 44(%rsp)
	addl	%r13d, %r9d
	addl	%r14d, %ecx
	cmpl	$3, %edx
	je	.L91
	movl	8(%r11), %ebx
	movl	%r15d, %eax
	subl	%ebx, %eax
	cmpl	$1, %edx
	je	.L116
	addl	%r9d, %eax
	cmpl	$2, %edx
	cmovne	%r9d, %eax
	movl	%eax, %r13d
.L93:
	cmpl	$3, 44(%rsp)
	je	.L103
.L101:
	movl	12(%r11), %r14d
	cmpl	$1, 44(%rsp)
	je	.L117
	cmpl	$2, 44(%rsp)
	je	.L118
	movl	%ecx, %eax
	cmpl	$4, 44(%rsp)
	je	.L119
.L94:
	movl	%r14d, %edx
	orl	%ebx, %edx
	shrl	$31, %edx
	cmpl	%r13d, %r9d
	setg	%r8b
	orb	%dl, %r8b
	jne	.L99
	cmpl	%eax, %ecx
	jle	.L100
.L99:
	addl	$1, 16+alignment_diagnostics(%rip)
	.p2align 4
	.p2align 3
.L73:
	xorl	%eax, %eax
.L69:
	leaq	-56(%rbp), %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	ret
	.p2align 4,,10
	.p2align 3
.L70:
	movq	12+alignment_diagnostics(%rip), %xmm0
	movq	.LC3(%rip), %xmm1
	paddd	%xmm1, %xmm0
	movq	%xmm0, 12+alignment_diagnostics(%rip)
	jmp	.L73
.L115:
	addq	$500, %rax
	movl	$1000, %ebx
	cqto
	idivq	%rbx
	jmp	.L88
.L114:
	addq	$500, %rax
	movl	$1000, %ebx
	cqto
	idivq	%rbx
	jmp	.L86
.L113:
	addq	$500, %rax
	movl	$1000, %ebx
	cqto
	idivq	%rbx
	jmp	.L84
.L112:
	addq	$500, %rax
	movl	$1000, %esi
	cqto
	idivq	%rsi
	jmp	.L82
.L111:
	addq	$500, %rax
	movl	$1000, %esi
	cqto
	idivq	%rsi
	jmp	.L80
.L110:
	addq	$500, %rax
	movl	$1000, %esi
	cqto
	idivq	%rsi
	jmp	.L78
.L109:
	addq	$500, %rax
	movl	$1000, %ecx
	cqto
	idivq	%rcx
	jmp	.L76
.L108:
	addq	$500, %rax
	movl	$1000, %ecx
	cqto
	idivq	%rcx
	jmp	.L74
.L105:
	movl	%ecx, %eax
	movl	%r15d, %ebx
	movl	%r12d, %r14d
.L100:
	leal	(%rbx,%r13), %edx
	addl	%r15d, %r9d
	cmpl	%r9d, %edx
	jg	.L99
	leal	(%r14,%rax), %edx
	addl	%r12d, %ecx
	cmpl	%ecx, %edx
	jg	.L99
	movq	48(%rbp), %rsi
	movl	%r13d, (%rsi)
	movl	%eax, 4(%rsi)
	movl	%ebx, 8(%rsi)
	movl	%r14d, 12(%rsi)
	movl	40(%rsp), %esi
	movdqu	(%r10), %xmm2
	addl	$1, alignment_diagnostics(%rip)
	movl	%eax, 48+alignment_diagnostics(%rip)
	movl	$1, %eax
	addl	$1, 8+alignment_diagnostics(%rip)
	movl	%esi, 24+alignment_diagnostics(%rip)
	movl	%r13d, 44+alignment_diagnostics(%rip)
	movl	%ebx, 52+alignment_diagnostics(%rip)
	movl	%r14d, 56+alignment_diagnostics(%rip)
	movups	%xmm2, 28+alignment_diagnostics(%rip)
	jmp	.L69
.L119:
	movslq	44(%r8), %rax
	imulq	%rdi, %rax
	testq	%rax, %rax
	jns	.L120
	subq	$500, %rax
	movl	$1000, %r8d
	cqto
	idivq	%r8
.L98:
	subl	4(%r11), %eax
	addl	%ecx, %eax
	jmp	.L94
.L118:
	movl	%r12d, %eax
	subl	%r14d, %eax
	addl	%ecx, %eax
	jmp	.L94
.L120:
	addq	$500, %rax
	movl	$1000, %r8d
	cqto
	idivq	%r8
	jmp	.L98
.L117:
	movl	%r12d, %eax
	movl	$2, %r8d
	subl	%r14d, %eax
	cltd
	idivl	%r8d
	addl	%ecx, %eax
	jmp	.L94
.L103:
	movl	%r12d, %r14d
	movl	%ecx, %eax
	jmp	.L94
.L116:
	movl	$2, %esi
	cltd
	idivl	%esi
	leal	(%rax,%r9), %r13d
	jmp	.L93
.L91:
	movl	%r9d, %r13d
	cmpl	$3, 44(%rsp)
	je	.L105
	movl	%r15d, %ebx
	jmp	.L101
	.p2align 4
	.def	stack_arrange_depth;	.scl	3;	.type	32;	.endef
stack_arrange_depth:
	testq	%rcx, %rcx
	je	.L172
	cmpw	$17, %dx
	je	.L172
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rdi
	pushq	%rsi
	pushq	%rbx
	movq	%rcx, %rbx
	subq	$64, %rsp
	andq	$-16, %rsp
	subq	$144, %rsp
	movl	%edx, 56(%rsp)
	movzbl	64(%rcx), %edx
	movl	60(%rcx), %ecx
	movups	%xmm6, 144(%rsp)
	movups	%xmm7, 160(%rsp)
	movups	%xmm8, 176(%rsp)
	movups	%xmm9, 192(%rsp)
	call	nova_scaling_effective
	movl	%eax, %r10d
	movl	%eax, 60(%rsp)
	movslq	36(%rbx), %rax
	imulq	%r10, %rax
	testq	%rax, %rax
	jns	.L192
	leaq	-500(%rax), %r8
	movabsq	$2361183241434822607, %rax
	imulq	%r8
	movq	%r8, %rax
	sarq	$63, %rax
	movq	%rdx, %rcx
	sarq	$7, %rcx
	subq	%rax, %rcx
	movslq	40(%rbx), %rax
	imulq	%r10, %rax
	testq	%rax, %rax
	jns	.L193
.L127:
	leaq	-500(%rax), %r9
	movabsq	$2361183241434822607, %rax
	imulq	%r9
	movq	%r9, %rax
	sarq	$63, %rax
	sarq	$7, %rdx
	movq	%rdx, %r8
	subq	%rax, %r8
	movslq	44(%rbx), %rax
	imulq	%r10, %rax
	testq	%rax, %rax
	jns	.L194
.L129:
	leaq	-500(%rax), %r9
	movabsq	$2361183241434822607, %rax
	imulq	%r9
	movq	%r9, %rax
	sarq	$63, %rax
	sarq	$7, %rdx
	movq	%rdx, %r11
	subq	%rax, %r11
.L130:
	movslq	48(%rbx), %rax
	imulq	%r10, %rax
	testq	%rax, %rax
	jns	.L195
	leaq	-500(%rax), %r9
	movabsq	$2361183241434822607, %rax
	imulq	%r9
	movq	%r9, %rax
	sarq	$63, %rax
	sarq	$7, %rdx
	subq	%rax, %rdx
.L132:
	movl	12(%rbx), %eax
	movl	16(%rbx), %esi
	subl	%ecx, %eax
	subl	%r8d, %esi
	subl	%r11d, %eax
	subl	%edx, %esi
	movl	%eax, %r12d
	orl	%esi, %eax
	js	.L133
	movzwl	4560(%rbx), %r15d
	xorl	%r11d, %r11d
	xorl	%r9d, %r9d
	xorl	%edi, %edi
	testw	%r15w, %r15w
	je	.L135
	movzwl	%r15w, %edx
	leaq	92(%rbx), %rax
	xorl	%edi, %edi
	xorl	%r11d, %r11d
	imulq	$280, %rdx, %rdx
	xorl	%r9d, %r9d
	addq	%rax, %rdx
	jmp	.L140
	.p2align 4,,10
	.p2align 3
.L196:
	addl	$1, %r11d
.L136:
	addq	$280, %rax
	cmpq	%rdx, %rax
	je	.L135
.L140:
	cmpb	$0, 260(%rax)
	je	.L136
	addl	$1, %r9d
	cmpb	$0, 261(%rax)
	jne	.L196
	movl	(%rbx), %r13d
	testl	%r13d, %r13d
	jne	.L138
	movl	4(%rax), %r13d
.L139:
	addq	$280, %rax
	addl	%r13d, %edi
	cmpq	%rdx, %rax
	jne	.L140
.L135:
	movl	(%rbx), %r13d
	movslq	52(%rbx), %rax
	movl	%esi, %r14d
	testl	%r13d, %r13d
	cmovne	%r12d, %r14d
	imulq	%r10, %rax
	testq	%rax, %rax
	jns	.L197
	leaq	-500(%rax), %r10
	movabsq	$2361183241434822607, %rax
	imulq	%r10
	sarq	$63, %r10
	sarq	$7, %rdx
	subq	%r10, %rdx
.L143:
	subl	%edi, %r14d
	movl	%edx, 52(%rsp)
	movl	%r14d, %r10d
	movl	%r14d, %eax
	cmpw	$1, %r9w
	jbe	.L144
	movzwl	%r9w, %eax
	subl	$1, %eax
	imull	%eax, %edx
	movl	%r14d, %eax
	subl	%edx, %eax
.L144:
	testl	%eax, %eax
	js	.L133
	movl	4(%rbx), %edi
	addl	8(%rbx), %r8d
	addl	%ecx, %edi
	movd	%edi, %xmm7
	testw	%r11w, %r11w
	je	.L145
	cltd
	movzwl	%r11w, %r11d
	idivl	%r11d
	testl	%r13d, %r13d
	cmove	%r8d, %edi
	movl	%edi, %r14d
	movd	%eax, %xmm1
.L146:
	movzwl	56(%rsp), %edx
	movd	%r12d, %xmm4
	leaq	100(%rbx), %rdi
	xorl	%r13d, %r13d
	punpckldq	%xmm1, %xmm4
	addl	$1, %edx
	testw	%r15w, %r15w
	je	.L163
	movl	%r12d, 48(%rsp)
	movd	%r8d, %xmm8
	movdqa	%xmm1, %xmm6
	movzwl	%dx, %r12d
	movl	%esi, 56(%rsp)
	movdqa	%xmm4, %xmm9
	movl	52(%rsp), %esi
	jmp	.L162
	.p2align 4,,10
	.p2align 3
.L198:
	testl	%eax, %eax
	jne	.L154
	movl	-4(%rdi), %r15d
	movd	48(%rsp), %xmm1
	movdqa	%xmm7, %xmm2
	movd	%r15d, %xmm5
	punpckldq	%xmm5, %xmm1
	movd	%r14d, %xmm5
	punpckldq	%xmm5, %xmm2
.L190:
	punpcklqdq	%xmm1, %xmm2
	movq	%xmm0, 80(%rsp)
	movups	%xmm2, 64(%rsp)
	pxor	%xmm2, %xmm2
	movups	%xmm2, 88(%rsp)
	movups	%xmm2, 104(%rsp)
	movups	%xmm2, 120(%rsp)
.L155:
	movq	%rdi, 32(%rsp)
	movl	60(%rsp), %r9d
	leaq	-16(%rdi), %rdx
	leaq	64(%rsp), %rcx
	leaq	80(%rsp), %r8
	call	nova_layout_apply_alignment
	testb	%al, %al
	je	.L124
	movq	244(%rdi), %rcx
	testq	%rcx, %rcx
	je	.L161
	movdqu	(%rdi), %xmm3
	movl	%r12d, %edx
	movups	%xmm3, 4(%rcx)
	call	stack_arrange_depth
	testb	%al, %al
	je	.L124
.L161:
	addl	%esi, %r15d
	addl	%r15d, %r14d
.L152:
	addl	$1, %r13d
	addq	$280, %rdi
	cmpw	4560(%rbx), %r13w
	jnb	.L163
.L162:
	cmpb	$0, 252(%rdi)
	je	.L152
	movq	236(%rdi), %xmm0
	movl	(%rbx), %eax
	cmpb	$0, 253(%rdi)
	je	.L198
	testl	%eax, %eax
	jne	.L199
	movdqa	%xmm7, %xmm2
	movd	%r14d, %xmm5
	movl	$3, 84(%rsp)
	punpckldq	%xmm5, %xmm2
	movd	%xmm0, 80(%rsp)
	punpcklqdq	%xmm9, %xmm2
	movups	%xmm2, 64(%rsp)
	pxor	%xmm2, %xmm2
	movups	%xmm2, 88(%rsp)
	movups	%xmm2, 104(%rsp)
	movups	%xmm2, 120(%rsp)
.L191:
	movd	%xmm6, %r15d
	jmp	.L155
	.p2align 4,,10
	.p2align 3
.L172:
	addl	$1, 24+stack_diagnostics(%rip)
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L133:
	addl	$1, 16+stack_diagnostics(%rip)
.L124:
	xorl	%eax, %eax
.L121:
	movups	144(%rsp), %xmm6
	movups	160(%rsp), %xmm7
	movups	176(%rsp), %xmm8
	movups	192(%rsp), %xmm9
	leaq	-56(%rbp), %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	ret
	.p2align 4,,10
	.p2align 3
.L138:
	movl	(%rax), %r13d
	jmp	.L139
	.p2align 4,,10
	.p2align 3
.L195:
	movabsq	$2361183241434822607, %rdx
	addq	$500, %rax
	shrq	$3, %rax
	mulq	%rdx
	shrq	$4, %rdx
	jmp	.L132
	.p2align 4,,10
	.p2align 3
.L192:
	movabsq	$2361183241434822607, %rcx
	addq	$500, %rax
	shrq	$3, %rax
	mulq	%rcx
	movslq	40(%rbx), %rax
	imulq	%r10, %rax
	movq	%rdx, %rcx
	shrq	$4, %rcx
	testq	%rax, %rax
	js	.L127
.L193:
	movabsq	$2361183241434822607, %r8
	addq	$500, %rax
	shrq	$3, %rax
	mulq	%r8
	movslq	44(%rbx), %rax
	imulq	%r10, %rax
	movq	%rdx, %r8
	shrq	$4, %r8
	testq	%rax, %rax
	js	.L129
.L194:
	movabsq	$2361183241434822607, %r11
	addq	$500, %rax
	shrq	$3, %rax
	mulq	%r11
	movq	%rdx, %r11
	shrq	$4, %r11
	jmp	.L130
	.p2align 4,,10
	.p2align 3
.L197:
	leaq	500(%rax), %rdx
	movabsq	$2361183241434822607, %rax
	shrq	$3, %rdx
	mulq	%rdx
	shrq	$4, %rdx
	jmp	.L143
	.p2align 4,,10
	.p2align 3
.L154:
	movl	-8(%rdi), %r15d
	movd	56(%rsp), %xmm3
	movd	%r14d, %xmm2
	punpckldq	%xmm8, %xmm2
	movd	%r15d, %xmm1
	punpckldq	%xmm3, %xmm1
	jmp	.L190
	.p2align 4,,10
	.p2align 3
.L199:
	movd	56(%rsp), %xmm3
	movd	%r14d, %xmm2
	movdqa	%xmm6, %xmm1
	movq	%xmm0, 80(%rsp)
	punpckldq	%xmm8, %xmm2
	punpckldq	%xmm3, %xmm1
	punpcklqdq	%xmm1, %xmm2
	movups	%xmm2, 64(%rsp)
	pxor	%xmm2, %xmm2
	movups	%xmm2, 88(%rsp)
	movups	%xmm2, 104(%rsp)
	movups	%xmm2, 120(%rsp)
	cmpl	$1, %eax
	jne	.L191
	movl	$3, 80(%rsp)
	jmp	.L191
	.p2align 4,,10
	.p2align 3
.L163:
	addl	$1, 4+stack_diagnostics(%rip)
	movl	$1, %eax
	movb	$0, 65(%rbx)
	jmp	.L121
	.p2align 4,,10
	.p2align 3
.L145:
	movl	56(%rbx), %eax
	testl	%r13d, %r13d
	movl	%edi, %r14d
	cmove	%r8d, %r14d
	testl	%eax, %eax
	je	.L170
	testw	%r9w, %r9w
	jne	.L200
.L170:
	pxor	%xmm1, %xmm1
	jmp	.L146
	.p2align 4,,10
	.p2align 3
.L200:
	movl	%eax, %edx
	andl	$-3, %edx
	cmpl	$1, %edx
	jne	.L148
	cmpw	$1, %r9w
	je	.L171
	movl	%r10d, %eax
	movzwl	%r9w, %ecx
	pxor	%xmm1, %xmm1
	subl	$1, %ecx
	cltd
	idivl	%ecx
	movl	%eax, 52(%rsp)
	jmp	.L146
.L148:
	movzwl	%r9w, %r9d
	cmpl	$2, %eax
	je	.L201
	movl	%r10d, %eax
	addl	$1, %r9d
	pxor	%xmm1, %xmm1
	cltd
	idivl	%r9d
	movl	%eax, 52(%rsp)
	addl	%eax, %r14d
	jmp	.L146
.L171:
	movl	$0, 52(%rsp)
	pxor	%xmm1, %xmm1
	jmp	.L146
.L201:
	movl	%r10d, %eax
	pxor	%xmm1, %xmm1
	cltd
	idivl	%r9d
	movl	%eax, %edi
	movl	%eax, 52(%rsp)
	shrl	$31, %eax
	addl	%edi, %eax
	sarl	%eax
	addl	%eax, %r14d
	jmp	.L146
	.p2align 4
	.def	grid_arrange_depth;	.scl	3;	.type	32;	.endef
grid_arrange_depth:
	testq	%rcx, %rcx
	je	.L234
	cmpb	$13, %dl
	je	.L234
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%r15
	pushq	%r14
	pushq	%r13
	movl	%edx, %r13d
	pushq	%r12
	pushq	%rdi
	pushq	%rsi
	pushq	%rbx
	movq	%rcx, %rbx
	andq	$-16, %rsp
	subq	$208, %rsp
	movzbl	10231(%rcx), %edx
	movl	10216(%rcx), %ecx
	call	nova_scaling_effective
	movl	%eax, %r8d
	movl	%eax, %r14d
	movslq	10220(%rbx), %rax
	imulq	%r8, %rax
	testq	%rax, %rax
	jns	.L252
	leaq	-500(%rax), %rcx
	movabsq	$2361183241434822607, %rax
	imulq	%rcx
	movq	%rcx, %rax
	sarq	$63, %rax
	sarq	$7, %rdx
	movq	%rdx, %r9
	subq	%rax, %r9
.L207:
	movslq	10224(%rbx), %rax
	imulq	%r8, %rax
	testq	%rax, %rax
	jns	.L253
	leaq	-500(%rax), %rcx
	movabsq	$2361183241434822607, %rax
	imulq	%rcx
	movq	%rcx, %rax
	sarq	$63, %rax
	sarq	$7, %rdx
	subq	%rax, %rdx
.L209:
	movzbl	10229(%rbx), %ecx
	movl	8(%rbx), %r8d
	movl	%edx, %esi
	movl	12(%rbx), %edi
	leal	-1(%rcx), %eax
	imull	%edx, %eax
	subl	%eax, %r8d
	movzbl	10228(%rbx), %eax
	subl	$1, %eax
	imull	%r9d, %eax
	subl	%eax, %edi
	movl	%r8d, %eax
	orl	%edi, %eax
	js	.L205
	movl	%ecx, %edx
	leaq	128(%rbx), %rcx
	movl	%r9d, 60(%rsp)
	call	grid_resolve_tracks.isra.0
	testb	%al, %al
	jne	.L254
.L205:
	xorl	%eax, %eax
.L202:
	leaq	-56(%rbp), %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	ret
	.p2align 4,,10
	.p2align 3
.L234:
	addl	$1, 24+grid_diagnostics(%rip)
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L253:
	movabsq	$2361183241434822607, %rdx
	addq	$500, %rax
	shrq	$3, %rax
	mulq	%rdx
	shrq	$4, %rdx
	jmp	.L209
	.p2align 4,,10
	.p2align 3
.L252:
	movabsq	$2361183241434822607, %r9
	addq	$500, %rax
	shrq	$3, %rax
	mulq	%r9
	movq	%rdx, %r9
	shrq	$4, %r9
	jmp	.L207
	.p2align 4,,10
	.p2align 3
.L254:
	movzbl	10228(%rbx), %edx
	leaq	32(%rbx), %rcx
	movl	%edi, %r8d
	call	grid_resolve_tracks.isra.0
	testb	%al, %al
	je	.L205
	movzbl	10228(%rbx), %r10d
	movl	60(%rsp), %r9d
	testb	%r10b, %r10b
	je	.L216
	leaq	80(%rsp), %rdx
	movl	4(%rbx), %r8d
	leaq	40(%rbx), %rcx
	leaq	(%rdx,%r10,4), %r11
	.p2align 5
	.p2align 4
	.p2align 3
.L215:
	movl	(%rcx), %r10d
	movl	%r8d, (%rdx)
	addq	$4, %rdx
	addq	$12, %rcx
	addl	%r9d, %r10d
	addl	%r10d, %r8d
	cmpq	%rdx, %r11
	jne	.L215
.L216:
	movzbl	10229(%rbx), %r10d
	testb	%r10b, %r10b
	je	.L214
	leaq	112(%rsp), %rdx
	movl	(%rbx), %r8d
	leaq	136(%rbx), %rcx
	leaq	(%rdx,%r10,4), %r11
	.p2align 5
	.p2align 4
	.p2align 3
.L219:
	movl	(%rcx), %r10d
	movl	%r8d, (%rdx)
	addq	$4, %rdx
	addq	$12, %rcx
	addl	%esi, %r10d
	addl	%r10d, %r8d
	cmpq	%rdx, %r11
	jne	.L219
.L214:
	leal	1(%r13), %edx
	movzbl	%dl, %edi
	movl	%edi, 56(%rsp)
	cmpb	$0, 10230(%rbx)
	je	.L218
	movl	%esi, 60(%rsp)
	leaq	244(%rbx), %rdi
	movq	%rbx, %rsi
	movl	%r9d, %r12d
	movb	%al, 55(%rsp)
	xorl	%ebx, %ebx
	jmp	.L225
	.p2align 4,,10
	.p2align 3
.L222:
	addl	$1, %ebx
	addq	$312, %rdi
	cmpb	10230(%rsi), %bl
	jnb	.L255
.L225:
	cmpb	$0, 290(%rdi)
	jne	.L221
	cmpb	$0, 291(%rdi)
	je	.L222
.L221:
	movzbl	287(%rdi), %r8d
	testb	%r8b, %r8b
	je	.L256
	movzbl	285(%rdi), %ecx
	movzbl	%r8b, %edx
	addq	%rcx, %rdx
	leaq	(%rcx,%rcx,2), %rax
	xorl	%ecx, %ecx
	leaq	(%rdx,%rdx,2), %rdx
	leaq	136(%rsi,%rax,4), %rax
	leaq	136(%rsi,%rdx,4), %rdx
	.p2align 4
	.p2align 4
	.p2align 3
.L226:
	addl	(%rax), %ecx
	addq	$12, %rax
	cmpq	%rax, %rdx
	jne	.L226
	subl	$1, %r8d
.L224:
	movzbl	286(%rdi), %r9d
	movzbl	284(%rdi), %r10d
	testb	%r9b, %r9b
	je	.L233
	movzbl	%r10b, %r13d
	movzbl	%r9b, %edx
	addq	%r13, %rdx
	leaq	0(%r13,%r13,2), %rax
	leaq	(%rdx,%rdx,2), %rdx
	leaq	40(%rsi,%rax,4), %rax
	leaq	40(%rsi,%rdx,4), %r13
	xorl	%edx, %edx
	.p2align 4
	.p2align 4
	.p2align 3
.L228:
	addl	(%rax), %edx
	addq	$12, %rax
	cmpq	%r13, %rax
	jne	.L228
	subl	$1, %r9d
.L227:
	movq	268(%rdi), %rax
	imull	%r12d, %r9d
	movq	$0, 192(%rsp)
	movzbl	%r10b, %r13d
	imull	60(%rsp), %r8d
	movd	80(%rsp,%r13,4), %xmm2
	movq	$0, 184(%rsp)
	movq	%rax, 144(%rsp)
	movzbl	10231(%rsi), %eax
	addl	%edx, %r9d
	movdqu	236(%rdi), %xmm3
	leaq	-16(%rdi), %rdx
	movdqu	252(%rdi), %xmm4
	movb	%al, 196(%rsp)
	addl	%r8d, %ecx
	movzbl	285(%rdi), %eax
	movd	%r9d, %xmm5
	movd	%ecx, %xmm0
	movl	%r14d, %r9d
	leaq	64(%rsp), %rcx
	movups	%xmm3, 152(%rsp)
	movd	112(%rsp,%rax,4), %xmm1
	movq	%rdi, 32(%rsp)
	punpckldq	%xmm5, %xmm0
	leaq	144(%rsp), %r8
	movups	%xmm4, 168(%rsp)
	punpckldq	%xmm2, %xmm1
	punpcklqdq	%xmm0, %xmm1
	movups	%xmm1, 64(%rsp)
	call	nova_layout_apply_alignment
	testb	%al, %al
	je	.L257
	movq	276(%rdi), %rcx
	testq	%rcx, %rcx
	je	.L222
	movdqu	(%rdi), %xmm2
	movl	56(%rsp), %edx
	movups	%xmm2, (%rcx)
	call	grid_arrange_depth
	testb	%al, %al
	jne	.L222
	jmp	.L205
	.p2align 4,,10
	.p2align 3
.L255:
	movzbl	55(%rsp), %eax
	movq	%rsi, %rbx
.L218:
	addl	$1, 4+grid_diagnostics(%rip)
	movb	$0, 10232(%rbx)
	jmp	.L202
	.p2align 4,,10
	.p2align 3
.L256:
	movl	$-1, %r8d
	xorl	%ecx, %ecx
	jmp	.L224
	.p2align 4,,10
	.p2align 3
.L233:
	movl	$-1, %r9d
	xorl	%edx, %edx
	jmp	.L227
.L257:
	addl	$1, 16+grid_diagnostics(%rip)
	jmp	.L205
	.p2align 4
	.def	overlay_arrange_depth;	.scl	3;	.type	32;	.endef
overlay_arrange_depth:
	testq	%rcx, %rcx
	je	.L297
	cmpb	$13, %dl
	je	.L297
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rdi
	pushq	%rsi
	movl	%edx, %esi
	pushq	%rbx
	movq	%rcx, %rbx
	subq	$16, %rsp
	andq	$-16, %rsp
	subq	$176, %rsp
	movzbl	5070(%rcx), %edx
	movl	5048(%rcx), %ecx
	movups	%xmm6, 176(%rsp)
	call	nova_scaling_effective
	movl	%eax, %r13d
	movslq	32(%rbx), %rax
	movq	%r13, %r15
	imulq	%r13, %rax
	testq	%rax, %rax
	jns	.L312
	leaq	-500(%rax), %rcx
	movabsq	$2361183241434822607, %rax
	imulq	%rcx
	movq	%rcx, %rax
	sarq	$63, %rax
	sarq	$7, %rdx
	movq	%rdx, %r8
	subq	%rax, %r8
.L263:
	movslq	36(%rbx), %rax
	imulq	%r13, %rax
	testq	%rax, %rax
	jns	.L313
	leaq	-500(%rax), %r10
	movabsq	$2361183241434822607, %rax
	imulq	%r10
	movq	%r10, %rax
	sarq	$63, %rax
	movq	%rdx, %rcx
	sarq	$7, %rcx
	subq	%rax, %rcx
.L265:
	movslq	40(%rbx), %rax
	imulq	%r13, %rax
	testq	%rax, %rax
	jns	.L314
	leaq	-500(%rax), %r10
	movabsq	$2361183241434822607, %rax
	imulq	%r10
	movq	%r10, %rax
	sarq	$63, %rax
	sarq	$7, %rdx
	movq	%rdx, %r11
	subq	%rax, %r11
.L267:
	movslq	44(%rbx), %rax
	imulq	%r13, %rax
	testq	%rax, %rax
	jns	.L315
	leaq	-500(%rax), %r10
	movabsq	$2361183241434822607, %rax
	imulq	%r10
	movq	%r10, %rax
	sarq	$63, %rax
	sarq	$7, %rdx
	subq	%rax, %rdx
.L269:
	movl	8(%rbx), %eax
	subl	%r8d, %eax
	subl	%r11d, %eax
	movl	%eax, %edi
	movl	%eax, 76(%rsp)
	movl	12(%rbx), %eax
	subl	%ecx, %eax
	subl	%edx, %eax
	orl	%eax, %edi
	movl	%eax, 72(%rsp)
	js	.L311
	movl	(%rbx), %eax
	movl	4(%rbx), %edi
	movd	76(%rsp), %xmm0
	addl	%r8d, %eax
	addl	%ecx, %edi
	movd	%edi, %xmm5
	movd	%eax, %xmm1
	movl	%eax, 64(%rsp)
	punpckldq	%xmm5, %xmm1
	movd	72(%rsp), %xmm5
	movl	%edi, 60(%rsp)
	punpckldq	%xmm5, %xmm0
	punpcklqdq	%xmm0, %xmm1
	cmpb	$0, 5069(%rbx)
	jne	.L316
	movdqu	(%rbx), %xmm5
	movups	%xmm5, 16(%rbx)
.L273:
	movb	$0, 5072(%rbx)
	cmpb	$0, 5052(%rbx)
	je	.L277
	imulq	$44, %r13, %rdx
	addl	$1, %esi
	movdqa	%xmm1, %xmm6
	xorl	%edi, %edi
	movabsq	$2361183241434822607, %r12
	addq	$500, %rdx
	shrq	$3, %rdx
	movq	%rdx, %rax
	mulq	%r12
	movzbl	%sil, %eax
	movl	%eax, 68(%rsp)
	shrq	$4, %rdx
	movq	%rdx, 48(%rsp)
	jmp	.L276
	.p2align 4,,10
	.p2align 3
.L284:
	leaq	-500(%rax), %rcx
	movq	%rcx, %rax
	sarq	$63, %rcx
	imulq	%r12
	sarq	$7, %rdx
	subq	%rcx, %rdx
.L285:
	imulq	$312, %rsi, %rax
	addl	64(%rsp), %edx
	movl	%edx, 68(%rbx,%rax)
	movslq	348(%rbx,%rax), %rax
	imulq	%r13, %rax
	testq	%rax, %rax
	jns	.L317
	leaq	-500(%rax), %rcx
	movq	%rcx, %rax
	sarq	$63, %rcx
	imulq	%r12
	sarq	$7, %rdx
	subq	%rcx, %rdx
.L287:
	imulq	$312, %rsi, %rax
	addl	60(%rsp), %edx
	movl	%edx, 72(%rbx,%rax)
.L283:
	cmpb	$0, 5069(%rbx)
	je	.L288
	imulq	$312, %rsi, %rcx
	movl	16(%rbx), %r8d
	movl	%r8d, %r9d
	addq	%rbx, %rcx
	movl	68(%rcx), %eax
	movl	72(%rcx), %edx
	movl	76(%rcx), %r11d
	movl	80(%rcx), %r14d
	movl	20(%rbx), %ecx
	cmpl	%r8d, %eax
	cmovge	%eax, %r9d
	movl	%ecx, %r10d
	cmpl	%ecx, %edx
	cmovge	%edx, %r10d
	addl	24(%rbx), %r8d
	addl	%r11d, %eax
	cmpl	%eax, %r8d
	cmovle	%r8d, %eax
	addl	28(%rbx), %ecx
	addl	%r14d, %edx
	cmpl	%edx, %ecx
	cmovle	%ecx, %edx
	cmpl	%eax, %r9d
	jge	.L296
	cmpl	%edx, %r10d
	jge	.L296
	subl	%r9d, %eax
	subl	%r10d, %edx
	cmpl	%r11d, %eax
	je	.L318
.L290:
	addl	$1, 32+overlay_diagnostics(%rip)
.L291:
	imulq	$312, %rsi, %rcx
	movl	%r9d, 68(%rbx,%rcx)
	movl	%r10d, 72(%rbx,%rcx)
	movl	%eax, 76(%rbx,%rcx)
	movl	%edx, 80(%rbx,%rcx)
.L288:
	imulq	$312, %rsi, %rax
	addq	%rbx, %rax
	cmpb	$0, 356(%rax)
	je	.L292
	movl	328(%rax), %edx
	testl	%edx, %edx
	jne	.L292
	movl	48(%rax), %eax
	movb	$1, 5072(%rbx)
	movl	%eax, 5076(%rbx)
.L292:
	imulq	$312, %rsi, %rsi
	addq	%rbx, %rsi
	movq	336(%rsi), %rcx
	testq	%rcx, %rcx
	je	.L294
	movdqu	68(%rsi), %xmm5
	movl	68(%rsp), %edx
	movups	%xmm5, (%rcx)
	call	overlay_arrange_depth
	testb	%al, %al
	je	.L261
.L294:
	addq	$1, %rdi
	cmpb	5052(%rbx), %dil
	jnb	.L277
.L276:
	movzbl	5053(%rbx,%rdi), %esi
	imulq	$312, %rsi, %r8
	movq	%rsi, %rax
	addq	%rbx, %r8
	cmpl	$2, 328(%r8)
	je	.L294
	movdqu	52(%r8), %xmm2
	movl	60(%r8), %ecx
	movl	64(%r8), %edx
	movl	312(%r8), %r8d
	movups	%xmm2, 96(%rsp)
	testl	%ecx, %ecx
	jne	.L278
	cmpl	$3, %r8d
	cmove	76(%rsp), %ecx
.L278:
	imulq	$312, %rsi, %r8
	addq	%rbx, %r8
	movq	312(%r8), %r9
	movl	316(%r8), %r8d
	testl	%edx, %edx
	jne	.L279
	cmpl	$3, %r8d
	cmove	72(%rsp), %edx
.L279:
	movzbl	5070(%rbx), %r8d
	testb	%r8b, %r8b
	je	.L280
	imulq	$312, %rsi, %r10
	cmpb	$0, 357(%rbx,%r10)
	je	.L280
	movl	48(%rsp), %r10d
	cmpl	%r10d, %ecx
	cmovl	%r10d, %ecx
	cmpl	%r10d, %edx
	cmovl	%r10d, %edx
.L280:
	imulq	$312, %rsi, %r14
	pxor	%xmm0, %xmm0
	movq	%r9, 112(%rsp)
	movl	%r15d, %r9d
	imulq	$312, %rax, %rax
	movl	%ecx, 104(%rsp)
	leaq	80(%rsp), %rcx
	movl	%edx, 108(%rsp)
	leaq	96(%rsp), %rdx
	addq	%rbx, %r14
	movups	%xmm0, 152(%rsp)
	leaq	68(%rbx,%rax), %rax
	movdqu	84(%r14), %xmm3
	movb	%r8b, 164(%rsp)
	leaq	112(%rsp), %r8
	movq	%rax, 32(%rsp)
	movups	%xmm0, 136(%rsp)
	movups	%xmm3, 120(%rsp)
	movups	%xmm6, 80(%rsp)
	call	nova_layout_apply_alignment
	testb	%al, %al
	je	.L311
	cmpb	$0, 355(%r14)
	je	.L283
	movslq	344(%r14), %rax
	imulq	%r13, %rax
	testq	%rax, %rax
	js	.L284
	leaq	500(%rax), %rdx
	shrq	$3, %rdx
	movq	%rdx, %rax
	mulq	%r12
	shrq	$4, %rdx
	jmp	.L285
	.p2align 4,,10
	.p2align 3
.L297:
	addl	$1, 28+overlay_diagnostics(%rip)
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L311:
	addl	$1, 16+overlay_diagnostics(%rip)
.L261:
	xorl	%eax, %eax
.L258:
	movups	176(%rsp), %xmm6
	leaq	-56(%rbp), %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	ret
	.p2align 4,,10
	.p2align 3
.L315:
	movabsq	$2361183241434822607, %rdx
	addq	$500, %rax
	shrq	$3, %rax
	mulq	%rdx
	shrq	$4, %rdx
	jmp	.L269
	.p2align 4,,10
	.p2align 3
.L314:
	movabsq	$2361183241434822607, %r11
	addq	$500, %rax
	shrq	$3, %rax
	mulq	%r11
	movq	%rdx, %r11
	shrq	$4, %r11
	jmp	.L267
	.p2align 4,,10
	.p2align 3
.L313:
	movabsq	$2361183241434822607, %rcx
	addq	$500, %rax
	shrq	$3, %rax
	mulq	%rcx
	movq	%rdx, %rcx
	shrq	$4, %rcx
	jmp	.L265
	.p2align 4,,10
	.p2align 3
.L312:
	movabsq	$2361183241434822607, %r8
	addq	$500, %rax
	shrq	$3, %rax
	mulq	%r8
	movq	%rdx, %r8
	shrq	$4, %r8
	jmp	.L263
	.p2align 4,,10
	.p2align 3
.L316:
	movups	%xmm1, 16(%rbx)
	jmp	.L273
	.p2align 4,,10
	.p2align 3
.L296:
	xorl	%eax, %eax
	xorl	%edx, %edx
	xorl	%r10d, %r10d
	xorl	%r9d, %r9d
	cmpl	%r11d, %eax
	jne	.L290
.L318:
	cmpl	%r14d, %edx
	jne	.L290
	jmp	.L291
	.p2align 4,,10
	.p2align 3
.L317:
	leaq	500(%rax), %rdx
	shrq	$3, %rdx
	movq	%rdx, %rax
	mulq	%r12
	shrq	$4, %rdx
	jmp	.L287
	.p2align 4,,10
	.p2align 3
.L277:
	addl	$1, 4+overlay_diagnostics(%rip)
	movl	$1, %eax
	movb	$0, 5071(%rbx)
	jmp	.L258
	.p2align 4
	.globl	nova_layout_apply_spacing
	.def	nova_layout_apply_spacing;	.scl	2;	.type	32;	.endef
nova_layout_apply_spacing:
	pushq	%r15
	pushq	%r14
	pushq	%rdi
	pushq	%rsi
	pushq	%rbx
	movl	88(%rsp), %r11d
	testq	%rcx, %rcx
	je	.L320
	cmpq	$0, 104(%rsp)
	je	.L320
	movl	%edx, %r10d
	leal	-1(%rdx), %eax
	cmpw	112(%rsp), %ax
	jnb	.L320
	movl	%r8d, %eax
	orl	%r9d, %eax
	js	.L320
	movl	96(%rsp), %eax
	subl	$500, %eax
	cmpl	$3500, %eax
	ja	.L320
	cmpl	$4, 80(%rsp)
	ja	.L320
	movl	96(%rsp), %eax
	movslq	%r8d, %r8
	movzwl	%r10w, %ebx
	movl	%ebx, %edi
	imulq	%rax, %r8
	movq	%rax, %r14
	movabsq	$2361183241434822607, %rax
	addq	$500, %r8
	shrq	$3, %r8
	mulq	%r8
	movl	12(%rcx), %r8d
	shrq	$4, %rdx
	testb	%r11b, %r11b
	cmovne	8(%rcx), %r8d
	movq	%rdx, %rsi
	movl	%edx, %eax
	imull	%ebx, %edx
	movl	%r8d, %ebx
	subl	%edx, %ebx
	movl	%edx, %r15d
	movl	%ebx, %edx
	js	.L320
	cmpl	$1, 80(%rsp)
	je	.L340
	cmpl	$3, 80(%rsp)
	je	.L340
	cmpl	$2, 80(%rsp)
	je	.L341
	cmpl	$4, 80(%rsp)
	je	.L342
	movabsq	$2361183241434822607, %rax
	movslq	%r9d, %rdx
	imulq	%r14, %rdx
	addq	$500, %rdx
	shrq	$3, %rdx
	mulq	%rdx
	shrq	$4, %rdx
	movl	%edx, %r9d
	cmpw	$1, %r10w
	je	.L338
	leal	-1(%rdi), %eax
	imull	%edx, %eax
	addl	%r15d, %eax
	cmpl	%r8d, %eax
	jg	.L320
.L338:
	leal	(%rsi,%rdx), %eax
	xorl	%edx, %edx
	.p2align 4
	.p2align 3
.L326:
	movl	4(%rcx), %r8d
	testb	%r11b, %r11b
	cmovne	(%rcx), %r8d
	movzwl	%r10w, %r10d
	leal	(%rdx,%r8), %ecx
	movq	104(%rsp), %rdx
	leaq	(%rdx,%r10,4), %r8
	.p2align 4
	.p2align 4
	.p2align 3
.L332:
	movl	%ecx, (%rdx)
	addq	$4, %rdx
	addl	%eax, %ecx
	cmpq	%r8, %rdx
	jne	.L332
	movq	4+alignment_diagnostics(%rip), %xmm0
	movl	96(%rsp), %eax
	movl	%r9d, 20+alignment_diagnostics(%rip)
	movq	.LC3(%rip), %xmm1
	movl	%eax, 24+alignment_diagnostics(%rip)
	movl	$1, %eax
	paddd	%xmm1, %xmm0
	movq	%xmm0, 4+alignment_diagnostics(%rip)
	jmp	.L319
	.p2align 4,,10
	.p2align 3
.L320:
	addl	$1, 16+alignment_diagnostics(%rip)
	xorl	%eax, %eax
.L319:
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%r14
	popq	%r15
	ret
	.p2align 4,,10
	.p2align 3
.L340:
	cmpw	$1, %r10w
	je	.L334
	movl	%edx, %eax
	leal	-1(%rdi), %r9d
	cltd
	idivl	%r9d
	xorl	%edx, %edx
	movl	%eax, %r9d
	leal	(%rsi,%rax), %eax
	jmp	.L326
	.p2align 4,,10
	.p2align 3
.L334:
	xorl	%edx, %edx
	xorl	%r9d, %r9d
	jmp	.L326
	.p2align 4,,10
	.p2align 3
.L341:
	movl	%ebx, %eax
	cltd
	idivl	%edi
	movl	%eax, %edx
	movl	%eax, %r9d
	leal	(%rsi,%rax), %eax
	sarl	%edx
	jmp	.L326
.L342:
	movl	%ebx, %eax
	leal	1(%rdi), %r9d
	cltd
	idivl	%r9d
	movl	%eax, %r9d
	leal	(%rsi,%rax), %eax
	movl	%r9d, %edx
	jmp	.L326
	.p2align 4
	.globl	nova_layout_apply_text
	.def	nova_layout_apply_text;	.scl	2;	.type	32;	.endef
nova_layout_apply_text:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%r14
	pushq	%rdi
	pushq	%rsi
	pushq	%rbx
	andq	$-16, %rsp
	subq	$48, %rsp
	movq	(%rdx), %r10
	movq	8(%rdx), %r11
	movq	%r10, %rsi
	movq	%r11, %rbx
	sarq	$32, %rsi
	sarq	$32, %rbx
	cmpq	$0, 48(%rbp)
	sete	%al
	cmpq	$0, 56(%rbp)
	sete	%dl
	orl	%edx, %eax
	testq	%rcx, %rcx
	sete	%dl
	orb	%dl, %al
	jne	.L346
	movl	%r10d, %eax
	orl	%esi, %eax
	orl	%r11d, %eax
	orl	%ebx, %eax
	orl	%r9d, %eax
	orl	%r8d, %eax
	jns	.L360
.L346:
	addl	$1, 16+alignment_diagnostics(%rip)
	xorl	%edi, %edi
.L343:
	leaq	-32(%rbp), %rsp
	movl	%edi, %eax
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%r14
	popq	%rbp
	ret
	.p2align 4,,10
	.p2align 3
.L360:
	movq	56(%rbp), %rdx
	movq	%r11, 32(%rsp)
	movq	%r10, 40(%rsp)
	movl	%r9d, 40(%rbp)
	movl	%r8d, 32(%rbp)
	movq	%rcx, 16(%rbp)
	call	nova_text_layout_measure
	movq	16(%rbp), %rcx
	movl	32(%rbp), %r8d
	testb	%al, %al
	movl	40(%rbp), %r9d
	movq	40(%rsp), %r10
	movl	%eax, %edi
	movq	32(%rsp), %r11
	je	.L346
	movl	16(%rcx), %eax
	movl	%eax, %r14d
	cmpb	$0, 32(%rcx)
	je	.L350
	leal	(%rax,%rax,4), %edx
	movl	$4000, %ecx
	shrl	$2, %edx
	cmpl	$3200, %eax
	cmova	%rcx, %rdx
	movq	%rdx, %r14
.L350:
	leal	(%r10,%r11), %edx
	movabsq	$2361183241434822607, %r10
	movslq	%edx, %rdx
	imulq	%r14, %rdx
	addq	$500, %rdx
	shrq	$3, %rdx
	movq	%rdx, %rax
	mulq	%r10
	shrq	$4, %rdx
	movslq	%edx, %rcx
	leal	(%rbx,%rsi), %edx
	movslq	%edx, %rdx
	imulq	%r14, %rdx
	addq	$500, %rdx
	shrq	$3, %rdx
	movq	%rdx, %rax
	mulq	%r10
	shrq	$4, %rdx
	movslq	%edx, %r11
	testl	%r8d, %r8d
	je	.L351
	leal	(%r8,%r9), %edx
	movslq	%edx, %rdx
	imulq	%r14, %rdx
	addq	$500, %rdx
	shrq	$3, %rdx
	movq	%rdx, %rax
	mulq	%r10
	shrq	$4, %rdx
	movslq	%edx, %rax
	addq	%rax, %rcx
.L351:
	movq	56(%rbp), %rax
	movslq	8(%rax), %rax
	addq	%rax, %rcx
	movq	56(%rbp), %rax
	movslq	12(%rax), %r8
	addq	%r11, %r8
	cmpq	$2147483647, %rcx
	jg	.L346
	cmpq	$2147483647, %r8
	jg	.L346
	movabsq	$2361183241434822607, %rax
	imulq	%r14, %rsi
	leaq	500(%rsi), %rdx
	shrq	$3, %rdx
	mulq	%rdx
	movq	56(%rbp), %rax
	shrq	$4, %rdx
	addl	28(%rax), %edx
	movq	48(%rbp), %rax
	movl	$0, (%rax)
	movl	%edx, 4(%rax)
	movl	%ecx, 8(%rax)
	movl	%r8d, 12(%rax)
	addl	$1, 8+alignment_diagnostics(%rip)
	jmp	.L343
	.p2align 4
	.globl	nova_alignment_diagnostics
	.def	nova_alignment_diagnostics;	.scl	2;	.type	32;	.endef
nova_alignment_diagnostics:
	leaq	alignment_diagnostics(%rip), %rax
	ret
	.p2align 4
	.globl	nova_stack_initialize
	.def	nova_stack_initialize;	.scl	2;	.type	32;	.endef
nova_stack_initialize:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rdi
	pushq	%rsi
	pushq	%rbx
	movq	(%r8), %r10
	movq	8(%r8), %r11
	movq	(%r9), %rbx
	movq	8(%r9), %r8
	andq	$-16, %rsp
	testq	%rcx, %rcx
	je	.L363
	cmpl	$1, %edx
	ja	.L363
	movq	%r11, %rax
	movq	%rbx, %r9
	sarq	$32, %rax
	sarq	$32, %r9
	orl	%r11d, %eax
	orl	%ebx, %eax
	orl	%r9d, %eax
	movq	%r8, %r9
	orl	%r8d, %eax
	sarq	$32, %r9
	orl	%r9d, %eax
	orl	48(%rbp), %eax
	jns	.L369
.L363:
	addl	$1, 16+stack_diagnostics(%rip)
	xorl	%eax, %eax
.L362:
	leaq	-24(%rbp), %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%rbp
	ret
	.p2align 4,,10
	.p2align 3
.L369:
	cmpl	$4, 56(%rbp)
	ja	.L363
	movl	64(%rbp), %eax
	subl	$500, %eax
	cmpl	$3500, %eax
	ja	.L363
	movzbl	72(%rbp), %eax
	movb	$1, %ah
	movl	%eax, %edi
	movq	%rcx, %rax
	.p2align 5
	.p2align 4
	.p2align 3
.L366:
	movq	%rax, %r9
	addq	$1, %rax
	movb	$0, (%r9)
	leaq	4568(%rcx), %r9
	cmpq	%r9, %rax
	jne	.L366
	movl	48(%rbp), %eax
	movl	%edx, (%rcx)
	movq	%r10, 4(%rcx)
	movl	%eax, 52(%rcx)
	movl	56(%rbp), %eax
	movq	%r11, 12(%rcx)
	movl	%eax, 56(%rcx)
	movl	64(%rbp), %eax
	movq	%rbx, 36(%rcx)
	movl	%eax, 60(%rcx)
	movl	$1, %eax
	movq	%r8, 44(%rcx)
	movw	%di, 64(%rcx)
	jmp	.L362
	.p2align 4
	.globl	nova_stack_add
	.def	nova_stack_add;	.scl	2;	.type	32;	.endef
nova_stack_add:
	testq	%rcx, %rcx
	je	.L371
	movq	%rdx, %rax
	testq	%rdx, %rdx
	je	.L371
	movzwl	4560(%rcx), %r11d
	movq	%rcx, %r8
	cmpw	$15, %r11w
	ja	.L371
	movl	12(%rdx), %ecx
	testl	%ecx, %ecx
	js	.L371
	movl	16(%rdx), %edx
	testl	%edx, %edx
	js	.L371
	cmpl	$3, 256(%rax)
	ja	.L371
	cmpl	$4, 260(%rax)
	ja	.L371
	testw	%r11w, %r11w
	je	.L402
	movzwl	%r11w, %r10d
	movl	(%rax), %r9d
	leaq	80(%r8), %rdx
	imulq	$280, %r10, %r10
	addq	%rdx, %r10
	testl	%r9d, %r9d
	je	.L403
	.p2align 4
	.p2align 3
.L375:
	cmpl	(%rdx), %r9d
	je	.L381
	movq	264(%rax), %rcx
	testq	%rcx, %rcx
	je	.L382
	cmpq	264(%rdx), %rcx
	je	.L381
	addq	$280, %rdx
	cmpq	%rdx, %r10
	jne	.L375
.L380:
	testq	%rcx, %rcx
	je	.L378
	movq	%r8, %rdx
	.p2align 4
	.p2align 4
	.p2align 3
.L386:
	cmpq	%rdx, %rcx
	je	.L404
	movq	72(%rdx), %rdx
	testq	%rdx, %rdx
	jne	.L386
	movq	72(%rcx), %rdx
	cmpq	%r8, %rdx
	je	.L388
	testq	%rdx, %rdx
	jne	.L381
.L388:
	movq	%r8, 72(%rcx)
.L378:
	leal	1(%r11), %edx
	pushq	%rdi
	movl	$35, %ecx
	imulq	$280, %r11, %r11
	pushq	%rsi
	movq	%rax, %rsi
	movl	$1, %eax
	movw	%dx, 4560(%r8)
	addl	$1, 12+stack_diagnostics(%rip)
	leaq	80(%r8,%r11), %rdi
	rep movsq
	movb	$1, 65(%r8)
	popq	%rsi
	popq	%rdi
	ret
	.p2align 4,,10
	.p2align 3
.L371:
	addl	$1, 16+stack_diagnostics(%rip)
.L374:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L381:
	addl	$1, 28+stack_diagnostics(%rip)
	jmp	.L374
	.p2align 4,,10
	.p2align 3
.L403:
	movq	264(%rax), %r9
	.p2align 4
	.p2align 3
.L376:
	movq	%r9, %rcx
	testq	%r9, %r9
	je	.L405
.L377:
	cmpq	264(%rdx), %r9
	je	.L381
	addq	$280, %rdx
	cmpq	%rdx, %r10
	je	.L380
	movq	%r9, %rcx
	testq	%r9, %r9
	jne	.L377
.L405:
	leaq	280(%rdx), %rcx
	cmpq	%r10, %rcx
	je	.L378
	addq	$560, %rdx
	cmpq	%rdx, %r10
	jne	.L376
	jmp	.L378
	.p2align 4,,10
	.p2align 3
.L404:
	addl	$1, 24+stack_diagnostics(%rip)
	jmp	.L374
.L402:
	movq	264(%rax), %rcx
	jmp	.L380
.L382:
	addq	$280, %rdx
	cmpq	%rdx, %r10
	jne	.L375
	jmp	.L378
	.p2align 4
	.globl	nova_stack_remove
	.def	nova_stack_remove;	.scl	2;	.type	32;	.endef
nova_stack_remove:
	movq	%rcx, %r8
	testq	%rcx, %rcx
	je	.L407
	movzwl	4560(%r8), %r11d
	movl	%edx, %ecx
	cmpw	%r11w, %dx
	jnb	.L407
	movzwl	%dx, %eax
	pushq	%rdi
	imulq	$280, %rax, %rax
	pushq	%rsi
	movq	344(%r8,%rax), %rax
	testq	%rax, %rax
	je	.L410
	movq	$0, 72(%rax)
.L410:
	leal	1(%rcx), %esi
	cmpw	%r11w, %si
	jnb	.L411
	movl	%r11d, %edx
	movzwl	%si, %r10d
	movzwl	%si, %esi
	subl	%ecx, %edx
	imulq	$280, %r10, %r9
	subl	$2, %edx
	movzwl	%dx, %edx
	addq	%r10, %rdx
	leaq	80(%r8,%r9), %rax
	imulq	$280, %rdx, %rdx
	leaq	360(%r8,%rdx), %r10
	leal	-1(%rsi), %edx
	imulq	$280, %rdx, %rdx
	.p2align 5
	.p2align 4
	.p2align 3
.L412:
	movq	%rax, %rdi
	movq	%rax, %rsi
	addq	$280, %rax
	movl	$35, %ecx
	subq	%r9, %rdi
	addq	%rdx, %rdi
	rep movsq
	cmpq	%r10, %rax
	jne	.L412
.L411:
	subl	$1, %r11d
	movl	$1, %eax
	popq	%rsi
	movb	$1, 65(%r8)
	addl	$1, 12+stack_diagnostics(%rip)
	popq	%rdi
	movw	%r11w, 4560(%r8)
	ret
	.p2align 4,,10
	.p2align 3
.L407:
	addl	$1, 16+stack_diagnostics(%rip)
	xorl	%eax, %eax
	ret
	.p2align 4
	.globl	nova_stack_arrange
	.def	nova_stack_arrange;	.scl	2;	.type	32;	.endef
nova_stack_arrange:
	movl	$1, %edx
	jmp	stack_arrange_depth
	.p2align 4
	.globl	nova_stack_diagnostics
	.def	nova_stack_diagnostics;	.scl	2;	.type	32;	.endef
nova_stack_diagnostics:
	leaq	stack_diagnostics(%rip), %rax
	ret
	.p2align 4
	.globl	nova_grid_initialize
	.def	nova_grid_initialize;	.scl	2;	.type	32;	.endef
nova_grid_initialize:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rdi
	pushq	%rsi
	pushq	%rbx
	movq	(%rdx), %r11
	movq	8(%rdx), %rbx
	movl	48(%rbp), %r10d
	andq	$-16, %rsp
	testq	%rcx, %rcx
	je	.L426
	movq	%rbx, %rax
	sarq	$32, %rax
	orl	%ebx, %eax
	orl	%r10d, %eax
	orl	%r9d, %eax
	js	.L426
	leal	-500(%r8), %eax
	cmpl	$3500, %eax
	ja	.L426
	movzbl	56(%rbp), %edi
	leaq	10240(%rcx), %rsi
	movl	%edi, %eax
	movb	$1, %ah
	movl	%eax, %edi
	movq	%rcx, %rax
	.p2align 4
	.p2align 4
	.p2align 3
.L427:
	movq	%rax, %rdx
	addq	$1, %rax
	movb	$0, (%rdx)
	cmpq	%rax, %rsi
	jne	.L427
	movq	%r11, (%rcx)
	movl	$1, %eax
	movq	%rbx, 8(%rcx)
	movl	%r8d, 10216(%rcx)
	movl	%r9d, 10220(%rcx)
	movl	%r10d, 10224(%rcx)
	movw	%di, 10231(%rcx)
	leaq	-24(%rbp), %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%rbp
	ret
	.p2align 4,,10
	.p2align 3
.L426:
	addl	$1, 16+grid_diagnostics(%rip)
	leaq	-24(%rbp), %rsp
	xorl	%eax, %eax
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%rbp
	ret
	.p2align 4
	.globl	nova_grid_add_row
	.def	nova_grid_add_row;	.scl	2;	.type	32;	.endef
nova_grid_add_row:
	movzwl	%r8w, %r9d
	movl	%edx, %r8d
	movl	$1, %edx
	jmp	grid_add_track
	.p2align 4
	.globl	nova_grid_add_column
	.def	nova_grid_add_column;	.scl	2;	.type	32;	.endef
nova_grid_add_column:
	movzwl	%r8w, %r9d
	movl	%edx, %r8d
	xorl	%edx, %edx
	jmp	grid_add_track
	.p2align 4
	.globl	nova_grid_add
	.def	nova_grid_add;	.scl	2;	.type	32;	.endef
nova_grid_add:
	testq	%rcx, %rcx
	je	.L493
	movq	%rdx, %rax
	testq	%rdx, %rdx
	je	.L493
	movq	%rcx, %r8
	movzbl	10230(%rcx), %ecx
	cmpb	$31, %cl
	ja	.L493
	movzbl	306(%rdx), %edx
	testb	%dl, %dl
	je	.L493
	movzbl	307(%rax), %r10d
	testb	%r10b, %r10b
	je	.L493
	movzbl	304(%rax), %r9d
	movzbl	10228(%r8), %r11d
	cmpb	%r11b, %r9b
	jnb	.L493
	pushq	%r15
	pushq	%r14
	pushq	%r12
	pushq	%rbp
	pushq	%rdi
	pushq	%rsi
	pushq	%rbx
	movzbl	305(%rax), %esi
	movzbl	10229(%r8), %edi
	cmpb	%dil, %sil
	jnb	.L437
	addl	%r9d, %edx
	movl	%edx, %ebx
	cmpl	%r11d, %edx
	jg	.L437
	movzbl	%sil, %r11d
	movzbl	%dil, %edx
	addl	%r11d, %r10d
	cmpl	%edx, %r10d
	jg	.L437
	movl	12(%rax), %esi
	testl	%esi, %esi
	js	.L437
	movl	16(%rax), %edx
	testl	%edx, %edx
	js	.L437
	movl	268(%rax), %edx
	orl	264(%rax), %edx
	js	.L437
	movabsq	$-9223372034707292160, %rdx
	andq	256(%rax), %rdx
	jne	.L437
	movl	284(%rax), %edx
	orl	280(%rax), %edx
	notl	%edx
	movl	%edx, %esi
	movabsq	$-9223372034707292160, %rdx
	shrl	$31, %esi
	andq	272(%rax), %rdx
	sete	%dl
	andb	%sil, %dl
	je	.L437
	cmpl	$3, 288(%rax)
	ja	.L437
	cmpl	$4, 292(%rax)
	ja	.L437
	testb	%cl, %cl
	je	.L496
	movzbl	%cl, %esi
	movl	(%rax), %ebp
	leaq	224(%r8), %r14
	imulq	$312, %rsi, %rsi
	leaq	224(%r8,%rsi), %r12
	jmp	.L446
	.p2align 4,,10
	.p2align 3
.L445:
	leaq	312(%r14), %rsi
	movq	%rsi, %r14
	cmpq	%rsi, %r12
	je	.L439
.L446:
	testl	%ebp, %ebp
	je	.L441
	cmpl	(%r14), %ebp
	je	.L442
.L441:
	movq	296(%rax), %rdi
	movq	%rdi, %r15
	testq	%rdi, %rdi
	je	.L443
	cmpq	296(%r14), %rdi
	je	.L442
.L443:
	movzbl	304(%r14), %esi
	movzbl	306(%r14), %edi
	addl	%esi, %edi
	cmpl	%edi, %r9d
	jge	.L445
	cmpl	%esi, %ebx
	jle	.L445
	movzbl	305(%r14), %esi
	movzbl	307(%r14), %edi
	addl	%esi, %edi
	cmpl	%edi, %r11d
	jge	.L445
	cmpl	%esi, %r10d
	jle	.L445
	addl	$1, 20+grid_diagnostics(%rip)
	.p2align 4
	.p2align 3
.L440:
	xorl	%edx, %edx
.L436:
	popq	%rbx
	movl	%edx, %eax
	popq	%rsi
	popq	%rdi
	popq	%rbp
	popq	%r12
	popq	%r14
	popq	%r15
	ret
	.p2align 4,,10
	.p2align 3
.L493:
	addl	$1, 16+grid_diagnostics(%rip)
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L437:
	addl	$1, 16+grid_diagnostics(%rip)
	jmp	.L440
.L496:
	movq	296(%rax), %r15
.L439:
	testq	%r15, %r15
	je	.L447
	movq	%r8, %r9
.L449:
	cmpq	%r9, %r15
	je	.L497
	movq	10208(%r9), %r9
	testq	%r9, %r9
	jne	.L449
	movq	10208(%r15), %r9
	cmpq	%r8, %r9
	je	.L453
	testq	%r9, %r9
	jne	.L442
.L453:
	movq	%r8, 10208(%r15)
.L447:
	leal	1(%rcx), %r9d
	movq	%rax, %rsi
	imulq	$312, %rcx, %rcx
	movb	%r9b, 10230(%r8)
	leaq	224(%r8,%rcx), %rdi
	movl	$39, %ecx
	rep movsq
	movb	$1, 10232(%r8)
	cmpb	$1, 306(%rax)
	jbe	.L498
.L451:
	addl	$1, 36+grid_diagnostics(%rip)
.L452:
	addl	$1, 12+grid_diagnostics(%rip)
	jmp	.L436
.L442:
	addl	$1, 28+grid_diagnostics(%rip)
	jmp	.L440
.L497:
	addl	$1, 24+grid_diagnostics(%rip)
	jmp	.L440
.L498:
	cmpb	$1, 307(%rax)
	ja	.L451
	jmp	.L452
	.p2align 4
	.globl	nova_grid_arrange
	.def	nova_grid_arrange;	.scl	2;	.type	32;	.endef
nova_grid_arrange:
	movl	$1, %edx
	jmp	grid_arrange_depth
	.p2align 4
	.globl	nova_grid_diagnostics
	.def	nova_grid_diagnostics;	.scl	2;	.type	32;	.endef
nova_grid_diagnostics:
	leaq	grid_diagnostics(%rip), %rax
	ret
	.p2align 4
	.globl	nova_overlay_initialize
	.def	nova_overlay_initialize;	.scl	2;	.type	32;	.endef
nova_overlay_initialize:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%r14
	pushq	%rdi
	pushq	%rsi
	pushq	%rbx
	movq	(%rdx), %r10
	movq	(%r8), %r11
	movq	8(%rdx), %rdx
	movq	8(%r8), %r8
	movl	48(%rbp), %esi
	movl	56(%rbp), %edi
	andq	$-16, %rsp
	testq	%rcx, %rcx
	je	.L502
	movq	%rdx, %rax
	movq	%r11, %rbx
	sarq	$32, %rax
	sarq	$32, %rbx
	orl	%edx, %eax
	orl	%r11d, %eax
	orl	%ebx, %eax
	movq	%r8, %rbx
	orl	%r8d, %eax
	sarq	$32, %rbx
	orl	%ebx, %eax
	jns	.L512
.L502:
	addl	$1, 16+overlay_diagnostics(%rip)
	xorl	%eax, %eax
.L501:
	leaq	-32(%rbp), %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%r14
	popq	%rbp
	ret
	.p2align 4,,10
	.p2align 3
.L512:
	leal	-500(%r9), %ebx
	movq	%rcx, %rax
	cmpl	$3500, %ebx
	ja	.L502
	.p2align 5
	.p2align 4
	.p2align 3
.L505:
	movq	%rax, %rbx
	addq	$1, %rax
	movb	$0, (%rbx)
	leaq	5080(%rcx), %rbx
	cmpq	%rbx, %rax
	jne	.L505
	movq	%r10, (%rcx)
	movl	$1, %eax
	movq	%rdx, 8(%rcx)
	movq	%r10, 16(%rcx)
	movq	%rdx, 24(%rcx)
	movq	%r11, 32(%rcx)
	movq	%r8, 40(%rcx)
	movl	%r9d, 5048(%rcx)
	movb	%sil, 5069(%rcx)
	movb	%dil, 5070(%rcx)
	movb	$1, 5071(%rcx)
	jmp	.L501
	.p2align 4
	.globl	nova_overlay_add
	.def	nova_overlay_add;	.scl	2;	.type	32;	.endef
nova_overlay_add:
	testq	%rcx, %rcx
	je	.L553
	movq	%rdx, %rax
	testq	%rdx, %rdx
	je	.L553
	movzbl	5052(%rcx), %edx
	movq	%rcx, %r8
	cmpb	$15, %dl
	ja	.L553
	movl	(%rax), %r9d
	testl	%r9d, %r9d
	je	.L553
	movl	12(%rax), %r10d
	testl	%r10d, %r10d
	js	.L553
	movl	16(%rax), %ecx
	testl	%ecx, %ecx
	js	.L553
	movl	40(%rax), %ecx
	orl	36(%rax), %ecx
	orl	44(%rax), %ecx
	orl	48(%rax), %ecx
	jns	.L556
.L553:
	addl	$1, 16+overlay_diagnostics(%rip)
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L556:
	cmpl	$3, 264(%rax)
	ja	.L553
	cmpl	$4, 268(%rax)
	ja	.L553
	pushq	%r14
	pushq	%rdi
	pushq	%rsi
	pushq	%rbx
	movl	272(%rax), %esi
	movl	%esi, %edi
	cmpl	$6, %esi
	ja	.L514
	cmpl	$3, 276(%rax)
	ja	.L514
	cmpl	$2, 280(%rax)
	ja	.L514
	testb	%dl, %dl
	je	.L557
	movzbl	%dl, %r11d
	leaq	48(%r8), %rcx
	imulq	$312, %r11, %r11
	addq	%rcx, %r11
	jmp	.L522
	.p2align 4,,10
	.p2align 3
.L559:
	movq	288(%rax), %r10
	testq	%r10, %r10
	je	.L519
	cmpq	288(%rcx), %r10
	je	.L518
.L519:
	cmpb	$0, 308(%rax)
	je	.L521
	cmpb	$0, 308(%rcx)
	je	.L521
	cmpl	272(%rcx), %edi
	je	.L558
.L521:
	addq	$312, %rcx
	cmpq	%r11, %rcx
	je	.L516
.L522:
	cmpl	(%rcx), %r9d
	jne	.L559
.L518:
	addl	$1, 20+overlay_diagnostics(%rip)
	.p2align 4
	.p2align 3
.L517:
	xorl	%eax, %eax
.L513:
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%r14
	ret
.L514:
	addl	$1, 16+overlay_diagnostics(%rip)
	jmp	.L517
.L557:
	movq	288(%rax), %r10
.L516:
	testq	%r10, %r10
	je	.L523
	movq	%r8, %rcx
.L525:
	cmpq	%rcx, %r10
	je	.L560
	movq	5040(%rcx), %rcx
	testq	%rcx, %rcx
	jne	.L525
	movq	5040(%r10), %rcx
	cmpq	%r8, %rcx
	je	.L536
	testq	%rcx, %rcx
	jne	.L518
.L536:
	movq	%r8, 5040(%r10)
.L523:
	movzbl	%dl, %ecx
	movq	%rax, %rsi
	movzbl	%dl, %r14d
	xorl	%r9d, %r9d
	imulq	$312, %rcx, %rcx
	leaq	48(%r8,%rcx), %rdi
	movl	$39, %ecx
	rep movsq
.L533:
	movl	%r9d, %r10d
	cmpb	%dl, %r9b
	jnb	.L527
	movzbl	5053(%r8,%r9), %ecx
	movl	272(%rax), %edi
	imulq	$312, %rcx, %rcx
	cmpl	%edi, 320(%r8,%rcx)
	jb	.L528
	je	.L561
.L530:
	leal	-1(%rdx), %r11d
	movzbl	%dl, %edi
	subl	%r10d, %r11d
	leaq	(%rdi,%r8), %rcx
	leal	-1(%r14), %r10d
	movq	%rdi, %rsi
	movzbl	%r11b, %r11d
	leaq	-1(%r8,%rdi), %rdi
	subq	%r11, %rdi
	movq	%rdi, %rbx
.L534:
	movq	%rcx, %r11
	subq	$1, %rcx
	subq	%rsi, %r11
	movzbl	5053(%r11,%r10), %r11d
	movb	%r11b, 5054(%rcx)
	cmpq	%rbx, %rcx
	jne	.L534
.L527:
	movslq	%r9d, %r9
	movb	%dl, 5053(%r8,%r9)
	addl	$1, %edx
	movb	%dl, 5052(%r8)
	movzwl	304(%rax), %eax
	cmpw	52+overlay_diagnostics(%rip), %ax
	movb	$1, 5071(%r8)
	jge	.L532
	movw	%ax, 52+overlay_diagnostics(%rip)
.L532:
	cmpw	54+overlay_diagnostics(%rip), %ax
	jle	.L535
	movw	%ax, 54+overlay_diagnostics(%rip)
.L535:
	addl	$1, 12+overlay_diagnostics(%rip)
	movl	$1, %eax
	jmp	.L513
.L561:
	movzwl	304(%rax), %edi
	cmpw	%di, 352(%r8,%rcx)
	jg	.L530
.L528:
	addq	$1, %r9
	jmp	.L533
.L560:
	addl	$1, 28+overlay_diagnostics(%rip)
	jmp	.L517
.L558:
	addl	$1, 24+overlay_diagnostics(%rip)
	jmp	.L517
	.p2align 4
	.globl	nova_overlay_set_visibility
	.def	nova_overlay_set_visibility;	.scl	2;	.type	32;	.endef
nova_overlay_set_visibility:
	testq	%rcx, %rcx
	je	.L563
	cmpl	$2, %r8d
	ja	.L563
	movzbl	5052(%rcx), %r10d
	leaq	48(%rcx), %r9
	xorl	%eax, %eax
	testb	%r10b, %r10b
	jne	.L568
	jmp	.L563
	.p2align 5
	.p2align 4,,10
	.p2align 3
.L566:
	addl	$1, %eax
	addq	$312, %r9
	cmpl	%r10d, %eax
	je	.L563
.L568:
	cmpl	%edx, (%r9)
	jne	.L566
	cltq
	imulq	$312, %rax, %rax
	cmpl	%r8d, 328(%rcx,%rax)
	je	.L567
	addl	$1, 36+overlay_diagnostics(%rip)
	addl	$1, 12+overlay_diagnostics(%rip)
	movl	%r8d, 328(%rcx,%rax)
	movb	$1, 5071(%rcx)
.L567:
	movl	$1, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L563:
	addl	$1, 16+overlay_diagnostics(%rip)
	xorl	%eax, %eax
	ret
	.p2align 4
	.globl	nova_overlay_arrange
	.def	nova_overlay_arrange;	.scl	2;	.type	32;	.endef
nova_overlay_arrange:
	movl	$1, %edx
	jmp	overlay_arrange_depth
	.p2align 4
	.globl	nova_overlay_input_allowed
	.def	nova_overlay_input_allowed;	.scl	2;	.type	32;	.endef
nova_overlay_input_allowed:
	testq	%rcx, %rcx
	je	.L573
	movl	$1, %eax
	cmpb	$0, 5072(%rcx)
	je	.L572
	movzbl	5052(%rcx), %r10d
	testb	%r10b, %r10b
	je	.L575
	imulq	$312, %r10, %r10
	movl	5076(%rcx), %r11d
	xorl	%r9d, %r9d
	xorl	%r8d, %r8d
	addq	$48, %rcx
	addq	%rcx, %r10
	.p2align 5
	.p2align 4
	.p2align 3
.L578:
	movl	(%rcx), %eax
	cmpl	%eax, %r11d
	cmove	%rcx, %r8
	cmpl	%eax, %edx
	cmove	%rcx, %r9
	addq	$312, %rcx
	cmpq	%rcx, %r10
	jne	.L578
	testq	%r8, %r8
	setne	%al
	testq	%r9, %r9
	setne	%dl
	andb	%dl, %al
	je	.L575
	cmpq	%r9, %r8
	je	.L572
	movl	272(%r9), %ecx
	movl	272(%r8), %edx
	cmpl	%ecx, %edx
	jb	.L572
	cmpl	%edx, %ecx
	je	.L591
	.p2align 4
	.p2align 3
.L575:
	addl	$1, 44+overlay_diagnostics(%rip)
.L573:
	xorl	%eax, %eax
.L572:
	ret
.L591:
	movzwl	304(%r8), %edx
	cmpw	%dx, 304(%r9)
	jl	.L575
	ret
	.p2align 4
	.globl	nova_overlay_hit_test
	.def	nova_overlay_hit_test;	.scl	2;	.type	32;	.endef
nova_overlay_hit_test:
	testq	%rcx, %rcx
	je	.L609
	movzbl	5052(%rcx), %eax
	addl	$1, 40+overlay_diagnostics(%rip)
	testb	%al, %al
	je	.L609
	pushq	%rsi
	leaq	5052(%rcx), %r11
	movl	%r8d, %esi
	pushq	%rbx
	movl	%edx, %ebx
	movzbl	%al, %edx
	subl	$1, %eax
	movzbl	%al, %eax
	subl	$1, %edx
	subq	%rax, %r11
	leaq	5053(%rcx,%rdx), %r9
	subq	$72, %rsp
	addq	%rdx, %r11
	.p2align 4
	.p2align 3
.L595:
	movzbl	(%r9), %eax
	movq	%rax, %r10
	imulq	$312, %rax, %rax
	addq	%rcx, %rax
	movl	328(%rax), %edx
	testl	%edx, %edx
	jne	.L596
	cmpb	$0, 357(%rax)
	je	.L596
	movl	68(%rax), %edx
	cmpl	%ebx, %edx
	jg	.L596
	movl	72(%rax), %r8d
	cmpl	%esi, %r8d
	jg	.L596
	addl	76(%rax), %edx
	cmpl	%ebx, %edx
	jle	.L596
	addl	80(%rax), %r8d
	cmpl	%esi, %r8d
	jg	.L613
	.p2align 4
	.p2align 3
.L596:
	subq	$1, %r9
	cmpq	%r11, %r9
	jne	.L595
	xorl	%eax, %eax
.L592:
	addq	$72, %rsp
	popq	%rbx
	popq	%rsi
	ret
	.p2align 4,,10
	.p2align 3
.L613:
	movl	48(%rax), %edx
	movq	%r11, 56(%rsp)
	movq	%r9, 48(%rsp)
	movq	%rcx, 96(%rsp)
	movb	%r10b, 47(%rsp)
	call	nova_overlay_input_allowed
	movq	96(%rsp), %rcx
	movq	48(%rsp), %r9
	testb	%al, %al
	movq	56(%rsp), %r11
	je	.L596
	movzbl	47(%rsp), %eax
	imulq	$312, %rax, %rax
	leaq	48(%rcx,%rax), %rax
	jmp	.L592
.L609:
	xorl	%eax, %eax
	ret
	.p2align 4
	.globl	nova_overlay_diagnostics
	.def	nova_overlay_diagnostics;	.scl	2;	.type	32;	.endef
nova_overlay_diagnostics:
	leaq	overlay_diagnostics(%rip), %rax
	ret
	.p2align 4
	.globl	nova_dlu_to_pixels
	.def	nova_dlu_to_pixels;	.scl	2;	.type	32;	.endef
nova_dlu_to_pixels:
	movslq	%ecx, %rcx
	movl	%edx, %edx
	imulq	%rdx, %rcx
	testq	%rcx, %rcx
	jns	.L618
	subq	$500, %rcx
	movabsq	$2361183241434822607, %rax
	imulq	%rcx
	sarq	$63, %rcx
	movq	%rdx, %rax
	sarq	$7, %rax
	subq	%rcx, %rax
	ret
	.p2align 4,,10
	.p2align 3
.L618:
	addq	$500, %rcx
	movabsq	$2361183241434822607, %rax
	shrq	$3, %rcx
	mulq	%rcx
	movq	%rdx, %rax
	shrq	$4, %rax
	ret
	.p2align 4
	.globl	nova_constraints_validate
	.def	nova_constraints_validate;	.scl	2;	.type	32;	.endef
nova_constraints_validate:
	addl	$1, constraint_diagnostics(%rip)
	testq	%rcx, %rcx
	je	.L620
	cmpl	$5, (%rcx)
	ja	.L620
	cmpl	$5, 4(%rcx)
	ja	.L620
	movl	8(%rcx), %r11d
	testl	%r11d, %r11d
	js	.L620
	movl	12(%rcx), %r10d
	testl	%r10d, %r10d
	js	.L620
	movl	16(%rcx), %eax
	testl	%eax, %eax
	js	.L620
	movl	20(%rcx), %edx
	testl	%edx, %edx
	js	.L620
	movl	24(%rcx), %r9d
	testl	%r9d, %r9d
	js	.L620
	movl	28(%rcx), %r8d
	testl	%r8d, %r8d
	js	.L620
	movl	32(%rcx), %r8d
	testl	%r8d, %r8d
	js	.L620
	movl	36(%rcx), %r9d
	testl	%r9d, %r9d
	js	.L620
	movl	40(%rcx), %r11d
	testl	%r11d, %r11d
	js	.L620
	movl	44(%rcx), %r10d
	testl	%r10d, %r10d
	js	.L620
	cmpw	$1000, 48(%rcx)
	ja	.L620
	cmpw	$1000, 50(%rcx)
	ja	.L620
	movl	64(%rcx), %r10d
	orl	60(%rcx), %r10d
	orl	68(%rcx), %r10d
	orl	72(%rcx), %r10d
	js	.L620
	movl	76(%rcx), %r10d
	orl	80(%rcx), %r10d
	orl	84(%rcx), %r10d
	orl	88(%rcx), %r10d
	js	.L620
	cmpw	$0, 56(%rcx)
	sete	%r10b
	cmpw	$0, 58(%rcx)
	sete	%cl
	cmpb	%cl, %r10b
	jne	.L620
	cmpl	%r8d, %eax
	jle	.L624
	testl	%r8d, %r8d
	jne	.L620
.L624:
	cmpl	%r9d, %edx
	jle	.L623
	testl	%r9d, %r9d
	jne	.L620
.L623:
	movl	$1, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L620:
	addl	$1, 48+constraint_diagnostics(%rip)
	xorl	%eax, %eax
	ret
	.p2align 4
	.globl	nova_constraints_resolve_dlu
	.def	nova_constraints_resolve_dlu;	.scl	2;	.type	32;	.endef
nova_constraints_resolve_dlu:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rdi
	pushq	%rsi
	pushq	%rbx
	andq	$-16, %rsp
	subq	$96, %rsp
	movq	(%rdx), %rdi
	movq	8(%rdx), %rbx
	testq	%r8, %r8
	je	.L638
	movq	%r8, %r10
	movq	%rbx, %r8
	sarq	$32, %r8
	movl	%r8d, %eax
	orl	%ebx, %eax
	js	.L640
	leal	-500(%r9), %eax
	cmpl	$3500, %eax
	ja	.L640
	movq	%r10, 32(%rbp)
	movq	%rcx, 16(%rbp)
	movl	%r9d, 40(%rbp)
	movq	%r8, 88(%rsp)
	call	nova_constraints_validate
	movq	32(%rbp), %r10
	testb	%al, %al
	movl	%eax, %esi
	jne	.L707
	.p2align 4
	.p2align 3
.L640:
	movq	%rdi, (%r10)
	movq	%rbx, 8(%r10)
.L638:
	addl	$1, 16+constraint_diagnostics(%rip)
.L642:
	xorl	%esi, %esi
.L637:
	leaq	-56(%rbp), %rsp
	movl	%esi, %eax
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	ret
	.p2align 4,,10
	.p2align 3
.L707:
	movq	16(%rbp), %r11
	movl	40(%rbp), %ecx
	movq	%r10, 32(%rbp)
	movzbl	92(%r11), %edx
	call	nova_scaling_effective
	movq	16(%rbp), %r11
	movq	32(%rbp), %r10
	movl	%eax, %ecx
	movl	%eax, 84(%rsp)
	movq	88(%rsp), %r8
	movslq	60(%r11), %rax
	movq	%rcx, %r15
	imulq	%rcx, %rax
	testq	%rax, %rax
	jns	.L708
	leaq	-500(%rax), %r9
	movabsq	$2361183241434822607, %rax
	imulq	%r9
	sarq	$63, %r9
	movq	%rdx, %rcx
	sarq	$7, %rcx
	subq	%r9, %rcx
.L644:
	movslq	64(%r11), %rax
	imulq	%r15, %rax
	testq	%rax, %rax
	jns	.L709
	leaq	-500(%rax), %r9
	movabsq	$2361183241434822607, %rax
	imulq	%r9
	movq	%r9, %rax
	sarq	$63, %rax
	sarq	$7, %rdx
	subq	%rax, %rdx
	movq	%rdx, %r9
.L646:
	movslq	68(%r11), %rax
	imulq	%r15, %rax
	testq	%rax, %rax
	jns	.L710
	movabsq	$2361183241434822607, %rdx
	subq	$500, %rax
	movq	%rax, 88(%rsp)
	imulq	%rdx
	movq	88(%rsp), %rax
	sarq	$63, %rax
	sarq	$7, %rdx
	subq	%rax, %rdx
.L648:
	movslq	72(%r11), %rax
	movl	%edx, %r14d
	imulq	%r15, %rax
	testq	%rax, %rax
	jns	.L711
	movabsq	$2361183241434822607, %rdx
	subq	$500, %rax
	movq	%rax, 88(%rsp)
	imulq	%rdx
	movq	88(%rsp), %rax
	sarq	$63, %rax
	sarq	$7, %rdx
	subq	%rax, %rdx
.L650:
	movl	%ebx, %eax
	subl	%r9d, %r8d
	subl	%ecx, %eax
	subl	%edx, %r8d
	subl	%r14d, %eax
	movl	%r8d, %r12d
	movl	%eax, %r13d
	orl	%r8d, %eax
	js	.L712
	leal	(%rcx,%rdi), %eax
	movd	%eax, %xmm0
	movq	%rdi, %rax
	sarq	$32, %rax
	addl	%r9d, %eax
	movd	%eax, %xmm2
	movl	84(%r11), %eax
	addl	76(%r11), %eax
	cltq
	imulq	%r15, %rax
	testq	%rax, %rax
	jns	.L713
	leaq	-500(%rax), %r8
	movabsq	$2361183241434822607, %rax
	imulq	%r8
	sarq	$63, %r8
	sarq	$7, %rdx
	subq	%r8, %rdx
.L654:
	movl	88(%r11), %eax
	addl	80(%r11), %eax
	movl	%edx, 88(%rsp)
	cltq
	imulq	%r15, %rax
	testq	%rax, %rax
	jns	.L714
	leaq	-500(%rax), %r8
	movabsq	$2361183241434822607, %rax
	imulq	%r8
	movq	%r8, %rax
	sarq	$63, %rax
	sarq	$7, %rdx
	subq	%rax, %rdx
	movq	%rdx, 72(%rsp)
.L656:
	movl	84(%rsp), %r14d
	movl	8(%r11), %edx
	movl	(%r11), %ecx
	movl	%r13d, 40(%rsp)
	movl	%r14d, 48(%rsp)
	movzwl	48(%r11), %eax
	movl	%eax, 32(%rsp)
	movl	24(%r11), %r9d
	movl	40(%r11), %r8d
	call	constraint_axis.isra.0
	movl	88(%rsp), %ecx
	movl	12(%r11), %edx
	addl	%eax, %ecx
	movl	%ecx, 88(%rsp)
	movl	4(%r11), %ecx
	movl	%r14d, 48(%rsp)
	movl	%r12d, 40(%rsp)
	movzwl	50(%r11), %eax
	movl	%eax, 32(%rsp)
	movl	28(%r11), %r9d
	movl	44(%r11), %r8d
	call	constraint_axis.isra.0
	movl	72(%rsp), %ecx
	leal	(%rcx,%rax), %r14d
	movslq	16(%r11), %rax
	imulq	%r15, %rax
	testq	%rax, %rax
	jns	.L715
	leaq	-500(%rax), %r8
	movabsq	$2361183241434822607, %rax
	imulq	%r8
	sarq	$63, %r8
	sarq	$7, %rdx
	subq	%r8, %rdx
.L658:
	movl	%edx, %r9d
	movslq	20(%r11), %rdx
	imulq	%r15, %rdx
	testq	%rdx, %rdx
	jns	.L716
	leaq	-500(%rdx), %rcx
	movabsq	$2361183241434822607, %rax
	imulq	%rcx
	sarq	$63, %rcx
	sarq	$7, %rdx
	subq	%rcx, %rdx
.L660:
	movslq	32(%r11), %rax
	movl	%edx, %r8d
	testl	%eax, %eax
	je	.L661
	imulq	%r15, %rax
	testq	%rax, %rax
	jns	.L717
	leaq	-500(%rax), %rcx
	movabsq	$2361183241434822607, %rax
	imulq	%rcx
	sarq	$63, %rcx
	sarq	$7, %rdx
	subq	%rcx, %rdx
.L663:
	movl	%edx, %r13d
.L661:
	movslq	36(%r11), %rax
	testl	%eax, %eax
	je	.L664
	imulq	%r15, %rax
	testq	%rax, %rax
	jns	.L718
	leaq	-500(%rax), %rcx
	movabsq	$2361183241434822607, %rax
	imulq	%rcx
	sarq	$63, %rcx
	sarq	$7, %rdx
	subq	%rcx, %rdx
.L666:
	movl	%edx, %r12d
.L664:
	cmpb	$0, 92(%r11)
	je	.L667
	cmpb	$0, 93(%r11)
	je	.L667
	movabsq	$2361183241434822607, %rdx
	imulq	$44, %r15, %rax
	addq	$500, %rax
	shrq	$3, %rax
	mulq	%rdx
	shrq	$4, %rdx
	cmpl	%edx, %r9d
	jge	.L668
	addl	$1, 44+constraint_diagnostics(%rip)
	movl	%edx, %r9d
.L668:
	cmpl	%edx, %r8d
	jge	.L667
	addl	$1, 44+constraint_diagnostics(%rip)
	movl	%edx, %r8d
	.p2align 4
	.p2align 3
.L667:
	cmpl	%r13d, %r9d
	jle	.L669
	addl	$1, 20+constraint_diagnostics(%rip)
	movl	%r9d, %r13d
	addl	$1, 12+constraint_diagnostics(%rip)
.L669:
	cmpl	%r12d, %r8d
	jle	.L670
	addl	$1, 20+constraint_diagnostics(%rip)
	movl	%r8d, %r12d
	addl	$1, 12+constraint_diagnostics(%rip)
.L670:
	cmpl	88(%rsp), %r9d
	jle	.L671
	addl	$1, 20+constraint_diagnostics(%rip)
	addl	$1, 12+constraint_diagnostics(%rip)
	movl	%r9d, 88(%rsp)
.L671:
	cmpl	%r14d, %r8d
	jle	.L672
	addl	$1, 20+constraint_diagnostics(%rip)
	movl	%r8d, %r14d
	addl	$1, 12+constraint_diagnostics(%rip)
.L672:
	cmpl	%r13d, 88(%rsp)
	jle	.L673
	addl	$1, 24+constraint_diagnostics(%rip)
	addl	$1, 12+constraint_diagnostics(%rip)
	movl	%r13d, 88(%rsp)
.L673:
	cmpl	%r12d, %r14d
	jle	.L674
	addl	$1, 24+constraint_diagnostics(%rip)
	movl	%r12d, %r14d
	addl	$1, 12+constraint_diagnostics(%rip)
.L674:
	movzwl	56(%r11), %eax
	testw	%ax, %ax
	je	.L706
	movzwl	58(%r11), %ecx
	testw	%cx, %cx
	je	.L706
	movzwl	%ax, %r11d
	movslq	88(%rsp), %rax
	imulq	%rcx, %rax
	cqto
	idivq	%r11
	cmpl	%eax, %r8d
	jg	.L681
	cmpl	%eax, %r12d
	jl	.L681
	movq	.LC3(%rip), %xmm3
	movl	%eax, %r14d
.L676:
	addl	$1, 28+constraint_diagnostics(%rip)
.L675:
	movd	88(%rsp), %xmm1
	movd	%r14d, %xmm4
	punpckldq	%xmm2, %xmm0
	movl	84(%rsp), %eax
	punpckldq	%xmm4, %xmm1
	punpcklqdq	%xmm1, %xmm0
	movups	%xmm0, (%r10)
	movq	4+constraint_diagnostics(%rip), %xmm0
	movq	%rdi, 56+constraint_diagnostics(%rip)
	paddd	%xmm3, %xmm0
	movq	%rbx, 64+constraint_diagnostics(%rip)
	movl	%eax, 52+constraint_diagnostics(%rip)
	movq	%xmm0, 4+constraint_diagnostics(%rip)
	movdqu	(%r10), %xmm5
	movups	%xmm5, 72+constraint_diagnostics(%rip)
	jmp	.L637
	.p2align 4,,10
	.p2align 3
.L713:
	leaq	500(%rax), %rdx
	movabsq	$2361183241434822607, %rax
	shrq	$3, %rdx
	mulq	%rdx
	shrq	$4, %rdx
	jmp	.L654
	.p2align 4,,10
	.p2align 3
.L712:
	movq	%rdi, (%r10)
	movq	%rbx, 8(%r10)
	addl	$1, 16+constraint_diagnostics(%rip)
	addl	$1, 48+constraint_diagnostics(%rip)
	jmp	.L642
	.p2align 4,,10
	.p2align 3
.L711:
	leaq	500(%rax), %rdx
	movabsq	$2361183241434822607, %rax
	shrq	$3, %rdx
	mulq	%rdx
	shrq	$4, %rdx
	jmp	.L650
	.p2align 4,,10
	.p2align 3
.L710:
	leaq	500(%rax), %rdx
	movabsq	$2361183241434822607, %rax
	shrq	$3, %rdx
	mulq	%rdx
	shrq	$4, %rdx
	jmp	.L648
	.p2align 4,,10
	.p2align 3
.L709:
	leaq	500(%rax), %r9
	movabsq	$2361183241434822607, %rax
	shrq	$3, %r9
	mulq	%r9
	movq	%rdx, %r9
	shrq	$4, %r9
	jmp	.L646
	.p2align 4,,10
	.p2align 3
.L708:
	leaq	500(%rax), %rcx
	movabsq	$2361183241434822607, %rax
	shrq	$3, %rcx
	mulq	%rcx
	movq	%rdx, %rcx
	shrq	$4, %rcx
	jmp	.L644
	.p2align 4,,10
	.p2align 3
.L706:
	movq	.LC3(%rip), %xmm3
	jmp	.L675
	.p2align 4,,10
	.p2align 3
.L716:
	addq	$500, %rdx
	movabsq	$2361183241434822607, %rax
	shrq	$3, %rdx
	mulq	%rdx
	shrq	$4, %rdx
	jmp	.L660
	.p2align 4,,10
	.p2align 3
.L715:
	movabsq	$2361183241434822607, %rcx
	addq	$500, %rax
	shrq	$3, %rax
	mulq	%rcx
	shrq	$4, %rdx
	jmp	.L658
	.p2align 4,,10
	.p2align 3
.L714:
	movabsq	$2361183241434822607, %rdx
	addq	$500, %rax
	shrq	$3, %rax
	mulq	%rdx
	shrq	$4, %rdx
	movq	%rdx, 72(%rsp)
	jmp	.L656
	.p2align 4,,10
	.p2align 3
.L717:
	movabsq	$2361183241434822607, %rdx
	addq	$500, %rax
	shrq	$3, %rax
	mulq	%rdx
	shrq	$4, %rdx
	jmp	.L663
	.p2align 4,,10
	.p2align 3
.L718:
	movabsq	$2361183241434822607, %rdx
	addq	$500, %rax
	shrq	$3, %rax
	mulq	%rdx
	shrq	$4, %rdx
	jmp	.L666
.L681:
	movslq	%r14d, %rax
	imulq	%r11, %rax
	cqto
	idivq	%rcx
	cmpl	%eax, %r9d
	jg	.L682
	cmpl	%eax, %r13d
	jl	.L682
	movq	.LC3(%rip), %xmm3
	movl	%eax, 88(%rsp)
	jmp	.L676
.L682:
	movq	12+constraint_diagnostics(%rip), %xmm1
	movq	.LC3(%rip), %xmm3
	paddd	%xmm3, %xmm1
	movq	%xmm1, 12+constraint_diagnostics(%rip)
	jmp	.L676
	.p2align 4
	.def	overlay_measure_depth;	.scl	3;	.type	32;	.endef
overlay_measure_depth:
	testq	%rcx, %rcx
	je	.L738
	cmpb	$13, %dl
	je	.L738
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%r13
	pushq	%r12
	pushq	%rdi
	pushq	%rsi
	movq	%rcx, %rsi
	pushq	%rbx
	andq	$-16, %rsp
	addq	$-128, %rsp
	cmpb	%dl, 50+overlay_diagnostics(%rip)
	jb	.L751
.L723:
	leal	1(%rdx), %r12d
	leaq	52(%rsi), %rbx
	xorl	%edi, %edi
	movzbl	%r12b, %r12d
	cmpb	$0, 5052(%rsi)
	jne	.L736
	jmp	.L737
	.p2align 5
	.p2align 4,,10
	.p2align 3
.L727:
	addl	$1, %edi
	addq	$312, %rbx
	cmpb	5052(%rsi), %dil
	jnb	.L737
.L736:
	cmpl	$2, 276(%rbx)
	je	.L727
	movq	284(%rbx), %rcx
	testq	%rcx, %rcx
	je	.L728
	cmpq	%rsi, 5040(%rcx)
	jne	.L722
	movl	%r12d, %edx
	call	overlay_measure_depth
	testb	%al, %al
	je	.L722
	movq	284(%rbx), %rax
	movdqu	(%rax), %xmm0
	movups	%xmm0, (%rbx)
.L728:
	cmpb	$0, 256(%rbx)
	je	.L731
	movq	180(%rbx), %rdx
	movdqu	164(%rbx), %xmm2
	leaq	-4(%rbx), %rax
	movdqu	148(%rbx), %xmm1
	movq	%rdx, 112(%rsp)
	movl	5048(%rsi), %edx
	movups	%xmm2, 96(%rsp)
	movl	%edx, 96(%rsp)
	movzbl	5070(%rsi), %edx
	movups	%xmm1, 80(%rsp)
	movb	%dl, 112(%rsp)
	movl	88(%rsp), %edx
	testl	%edx, %edx
	jne	.L732
	movl	8(%rsi), %edx
	movl	%edx, 88(%rsp)
.L732:
	movdqu	236(%rax), %xmm3
	movq	%rbx, 32(%rsp)
	leaq	64(%rsp), %rdx
	leaq	188(%rbx), %rax
	movq	%rax, 40(%rsp)
	leaq	80(%rsp), %rcx
	movups	%xmm3, 64(%rsp)
	movl	252(%rbx), %r9d
	movl	248(%rbx), %r8d
	call	nova_layout_apply_text
	testb	%al, %al
	je	.L752
.L731:
	cmpb	$0, 144(%rbx)
	je	.L727
	movdqu	(%rsi), %xmm4
	leaq	48(%rsp), %rdx
	leaq	48(%rbx), %rcx
	leaq	80(%rsp), %r8
	movups	%xmm4, 48(%rsp)
	movl	5048(%rsi), %r9d
	call	nova_constraints_resolve_dlu
	testb	%al, %al
	je	.L722
	movq	88(%rsp), %rax
	movq	%rax, 8(%rbx)
	jmp	.L727
	.p2align 4,,10
	.p2align 3
.L751:
	movb	%dl, 50+overlay_diagnostics(%rip)
	jmp	.L723
	.p2align 4,,10
	.p2align 3
.L752:
	addl	$1, 16+overlay_diagnostics(%rip)
.L722:
	leaq	-40(%rbp), %rsp
	xorl	%eax, %eax
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%r12
	popq	%r13
	popq	%rbp
	ret
	.p2align 4,,10
	.p2align 3
.L738:
	addl	$1, 28+overlay_diagnostics(%rip)
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L737:
	addl	$1, overlay_diagnostics(%rip)
	leaq	-40(%rbp), %rsp
	movl	$1, %eax
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%r12
	popq	%r13
	popq	%rbp
	ret
	.p2align 4
	.globl	nova_overlay_measure
	.def	nova_overlay_measure;	.scl	2;	.type	32;	.endef
nova_overlay_measure:
	movl	$1, %edx
	jmp	overlay_measure_depth
	.p2align 4
	.globl	nova_overlay_update
	.def	nova_overlay_update;	.scl	2;	.type	32;	.endef
nova_overlay_update:
	testq	%rcx, %rcx
	je	.L778
	cmpb	$0, 5071(%rcx)
	jne	.L779
	movl	$1, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L779:
	subq	$56, %rsp
	movl	$1, %edx
	movq	%rcx, 64(%rsp)
	call	overlay_measure_depth
	testb	%al, %al
	jne	.L780
.L758:
	xorl	%eax, %eax
	addq	$56, %rsp
	ret
	.p2align 4,,10
	.p2align 3
.L778:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L780:
	movq	64(%rsp), %rcx
	movl	$1, %edx
	call	overlay_arrange_depth
	testb	%al, %al
	je	.L758
	movq	64(%rsp), %rcx
	movl	$0, 41(%rsp)
	addl	$1, 8+overlay_diagnostics(%rip)
	movzbl	5052(%rcx), %edx
	movl	$0, 44(%rsp)
	movb	%dl, 48+overlay_diagnostics(%rip)
	testb	%dl, %dl
	je	.L759
	imulq	$312, %rdx, %rdx
	addq	$320, %rcx
	leaq	(%rdx,%rcx), %r8
	xorl	%edx, %edx
	.p2align 5
	.p2align 4
	.p2align 3
.L761:
	movl	(%rcx), %eax
	cmpb	$0, 41(%rsp,%rax)
	jne	.L760
	movb	$1, 41(%rsp,%rax)
	addl	$1, %edx
.L760:
	addq	$312, %rcx
	cmpq	%r8, %rcx
	jne	.L761
.L759:
	movb	%dl, 49+overlay_diagnostics(%rip)
	movl	$1, %eax
	addq	$56, %rsp
	ret
	.p2align 4
	.def	stack_measure_depth;	.scl	3;	.type	32;	.endef
stack_measure_depth:
	testq	%rcx, %rcx
	je	.L819
	cmpw	$17, %dx
	je	.L819
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rdi
	pushq	%rsi
	movq	%rcx, %rsi
	pushq	%rbx
	andq	$-16, %rsp
	subq	$176, %rsp
	cmpw	%dx, 34+stack_diagnostics(%rip)
	jnb	.L785
	movw	%dx, 34+stack_diagnostics(%rip)
.L785:
	movl	60(%rsi), %ecx
	movl	%edx, %r15d
	movzbl	64(%rsi), %edx
	call	nova_scaling_effective
	movl	%eax, %r8d
	movslq	36(%rsi), %rax
	imulq	%r8, %rax
	testq	%rax, %rax
	jns	.L831
	leaq	-500(%rax), %rcx
	movabsq	$2361183241434822607, %rax
	imulq	%rcx
	movq	%rcx, %rax
	sarq	$63, %rax
	sarq	$7, %rdx
	subq	%rax, %rdx
.L787:
	movslq	40(%rsi), %rax
	movl	%edx, 88(%rsp)
	imulq	%r8, %rax
	testq	%rax, %rax
	jns	.L832
	leaq	-500(%rax), %rcx
	movabsq	$2361183241434822607, %rax
	imulq	%rcx
	movq	%rcx, %rax
	sarq	$63, %rax
	sarq	$7, %rdx
	subq	%rax, %rdx
	movq	%rdx, 64(%rsp)
.L789:
	movslq	44(%rsi), %rax
	imulq	%r8, %rax
	testq	%rax, %rax
	jns	.L833
	leaq	-500(%rax), %rcx
	movabsq	$2361183241434822607, %rax
	imulq	%rcx
	movq	%rcx, %rax
	sarq	$63, %rax
	sarq	$7, %rdx
	subq	%rax, %rdx
.L791:
	movslq	48(%rsi), %rax
	movl	%edx, 92(%rsp)
	imulq	%r8, %rax
	testq	%rax, %rax
	jns	.L834
	leaq	-500(%rax), %rcx
	movabsq	$2361183241434822607, %rax
	imulq	%rcx
	movq	%rcx, %rax
	sarq	$63, %rax
	sarq	$7, %rdx
	subq	%rax, %rdx
	movq	%rdx, 72(%rsp)
.L793:
	movslq	52(%rsi), %rax
	imulq	%r8, %rax
	testq	%rax, %rax
	jns	.L835
	leaq	-500(%rax), %rcx
	movabsq	$2361183241434822607, %rax
	imulq	%rcx
	movq	%rcx, %rax
	sarq	$63, %rax
	sarq	$7, %rdx
	subq	%rax, %rdx
	movq	%rdx, 56(%rsp)
.L795:
	cmpw	$0, 4560(%rsi)
	je	.L818
	addl	$1, %r15d
	leaq	84(%rsi), %rbx
	xorl	%r12d, %r12d
	xorl	%r14d, %r14d
	movzwl	%r15w, %eax
	xorl	%r13d, %r13d
	movl	$2147483647, %edi
	xorl	%r15d, %r15d
	movl	%eax, 84(%rsp)
	jmp	.L815
	.p2align 4,,10
	.p2align 3
.L797:
	movq	260(%rbx), %rcx
	testq	%rcx, %rcx
	je	.L799
	cmpq	%rsi, 72(%rcx)
	jne	.L784
	movl	84(%rsp), %edx
	call	stack_measure_depth
	testb	%al, %al
	je	.L784
	movq	260(%rbx), %rax
	movdqu	20(%rax), %xmm0
	movups	%xmm0, (%rbx)
.L799:
	cmpb	$0, 248(%rbx)
	je	.L801
	movq	188(%rbx), %rax
	movdqu	156(%rbx), %xmm1
	leaq	-4(%rbx), %rdx
	movdqu	172(%rbx), %xmm2
	movq	%rax, 160(%rsp)
	movl	60(%rsi), %eax
	movups	%xmm1, 128(%rsp)
	movl	136(%rsp), %r8d
	movups	%xmm2, 144(%rsp)
	movl	%eax, 144(%rsp)
	movzbl	64(%rsi), %eax
	movb	%al, 160(%rsp)
	testl	%r8d, %r8d
	jne	.L802
	movl	12(%rsi), %eax
	subl	88(%rsp), %eax
	subl	92(%rsp), %eax
	movl	%eax, 136(%rsp)
.L802:
	movdqu	136(%rdx), %xmm3
	leaq	196(%rbx), %rax
	movq	%rbx, 32(%rsp)
	leaq	112(%rsp), %rdx
	movq	%rax, 40(%rsp)
	leaq	128(%rsp), %rcx
	movups	%xmm3, 112(%rsp)
	movl	244(%rbx), %r9d
	movl	240(%rbx), %r8d
	call	nova_layout_apply_text
	testb	%al, %al
	je	.L830
.L801:
	cmpb	$0, 152(%rbx)
	jne	.L836
.L805:
	movl	48(%rbx), %eax
	movl	52(%rbx), %ecx
	movl	40(%rbx), %r8d
	movl	32(%rbx), %r9d
	testl	%eax, %eax
	movl	44(%rbx), %edx
	cmove	%edi, %eax
	testl	%ecx, %ecx
	cmove	%edi, %ecx
	testl	%r8d, %r8d
	jne	.L809
	movl	8(%rbx), %r8d
.L809:
	testl	%edx, %edx
	jne	.L810
	movl	12(%rbx), %edx
.L810:
	cmpl	%r9d, %eax
	jb	.L811
	movl	36(%rbx), %r10d
	cmpl	%r10d, %ecx
	jb	.L811
	cmpl	%r8d, %r9d
	movl	$2147483647, %r11d
	cmovnb	%r9d, %r8d
	cmpl	%r11d, %eax
	cmova	%r11d, %eax
	cmpl	%eax, %r8d
	cmovbe	%r8d, %eax
	cmpl	%edx, %r10d
	movl	$2147483647, %r8d
	cmovnb	%r10d, %edx
	cmpl	%r8d, %ecx
	cmova	%r8d, %ecx
	cmpl	%ecx, %edx
	cmova	%ecx, %edx
	cmpl	%r9d, %eax
	jb	.L811
	cmpl	%r10d, %edx
	jb	.L811
	movl	%edx, 12(%rbx)
	movl	%eax, 8(%rbx)
	movl	(%rsi), %ecx
	testl	%ecx, %ecx
	movl	%edx, %ecx
	cmove	%eax, %ecx
	cmovne	%eax, %edx
	addl	%edx, %r15d
	cmpl	%ecx, %r13d
	cmovl	%ecx, %r13d
	addl	$1, %r14d
.L798:
	addl	$1, %r12d
	addq	$280, %rbx
	cmpw	4560(%rsi), %r12w
	jnb	.L837
.L815:
	cmpb	$0, 268(%rbx)
	jne	.L797
	addl	$1, 20+stack_d