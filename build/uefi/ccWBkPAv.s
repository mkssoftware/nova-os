	.file	"resources.c"
	.text
	.p2align 4
	.def	next_use;	.scl	3;	.type	32;	.endef
next_use:
	movq	use_clock(%rip), %rax
	cmpq	$-1, %rax
	je	.L2
	addq	$1, %rax
	movq	%rax, use_clock(%rip)
	ret
.L2:
	leaq	152+registry(%rip), %rax
	leaq	used(%rip), %rdx
	leaq	38912(%rax), %rcx
	.p2align 5
	.p2align 4
	.p2align 3
.L5:
	cmpb	$0, (%rdx)
	je	.L4
	shrq	(%rax)
.L4:
	addq	$304, %rax
	addq	$1, %rdx
	cmpq	%rcx, %rax
	jne	.L5
	movabsq	$-9223372036854775808, %rax
	movq	%rax, use_clock(%rip)
	ret
	.p2align 4
	.def	lookup_index;	.scl	3;	.type	32;	.endef
lookup_index:
	testq	%rcx, %rcx
	je	.L13
	cmpb	$0, 184+diagnostics(%rip)
	je	.L13
	pushq	%rbx
	movl	%ecx, %eax
	leal	256(%rcx), %r11d
	leaq	hash_slots(%rip), %r9
	leaq	used(%rip), %r10
	leaq	registry(%rip), %rbx
	jmp	.L11
	.p2align 4,,10
	.p2align 3
.L20:
	movswq	%r8w, %rdx
	cmpb	$0, (%r10,%rdx)
	je	.L10
	imulq	$304, %rdx, %rdx
	cmpq	%rcx, (%rbx,%rdx)
	je	.L8
.L10:
	addl	$1, %eax
	cmpl	%r11d, %eax
	je	.L14
.L11:
	movzbl	%al, %edx
	movzwl	(%r9,%rdx,2), %r8d
	testw	%r8w, %r8w
	jns	.L20
.L14:
	movl	$-1, %r8d
.L8:
	movl	%r8d, %eax
	popq	%rbx
	ret
.L13:
	movl	$-1, %r8d
	movl	%r8d, %eax
	ret
	.p2align 4
	.def	allocate_decoded;	.scl	3;	.type	32;	.endef
allocate_decoded:
	pushq	%rbp
	movq	160(%rcx), %r8
	xorl	%eax, %eax
	leaq	-1(%r8), %rdx
	movq	%rsp, %rbp
	andq	$-16, %rsp
	cmpq	$524287, %rdx
	ja	.L21
	addq	$8191, %r8
	movq	%rcx, %r11
	xorl	%r10d, %r10d
	shrq	$13, %r8
	leaq	decoded_blocks(%rip), %r9
.L23:
	movslq	%r10d, %rcx
	xorl	%edx, %edx
	addq	%r9, %rcx
.L25:
	cmpb	$0, (%rcx,%rdx)
	jne	.L40
	addq	$1, %rdx
	cmpb	%r8b, %dl
	jb	.L25
	leal	-1(%r8), %edx
	cmpb	$14, %dl
	jbe	.L31
	movl	%r8d, %edx
	movdqu	.LC0(%rip), %xmm0
	shrb	$4, %dl
	movzbl	%dl, %edx
	salq	$4, %rdx
	addq	%rcx, %rdx
.L27:
	movups	%xmm0, (%rcx)
	addq	$16, %rcx
	cmpq	%rcx, %rdx
	jne	.L27
	testb	$15, %r8b
	je	.L28
	movl	%r8d, %ecx
	andl	$-16, %ecx
.L26:
	movzbl	%cl, %edx
	addl	%edx, %r10d
	movl	%r10d, %r10d
	leaq	(%r9,%r10), %rdx
	.p2align 4
	.p2align 4
	.p2align 3
.L29:
	addl	$1, %ecx
	movb	$1, (%rdx)
	addq	$1, %rdx
	cmpb	%r8b, %cl
	jb	.L29
.L28:
	movb	%al, 225(%r11)
	salq	$13, %rax
	leaq	decoded_pool(%rip), %rdx
	movb	%r8b, 226(%r11)
	andl	$2088960, %eax
	addq	%rdx, %rax
.L21:
	leave
	ret
	.p2align 4,,10
	.p2align 3
.L40:
	leal	1(%rax,%rdx), %eax
	movzbl	%al, %r10d
	leal	(%r10,%r8), %edx
	cmpl	$64, %edx
	jle	.L23
	leave
	xorl	%eax, %eax
	ret
.L31:
	xorl	%ecx, %ecx
	jmp	.L26
	.p2align 4
	.def	invalidate_resource;	.scl	3;	.type	32;	.endef
invalidate_resource:
	pushq	%rbp
	movl	116(%rcx), %eax
	movq	%rcx, %r8
	andl	$-3, %eax
	movq	%rsp, %rbp
	andq	$-16, %rsp
	cmpl	$5, %eax
	jne	.L42
	movq	112+diagnostics(%rip), %rax
	subq	160(%rcx), %rax
	movq	%rax, 112+diagnostics(%rip)
.L42:
	movzbl	226(%r8), %ecx
	testb	%cl, %cl
	je	.L43
	leal	-1(%rcx), %eax
	movzbl	225(%r8), %r10d
	cmpb	$14, %al
	jbe	.L48
	movl	%ecx, %r9d
	movzbl	%r10b, %edx
	leaq	decoded_blocks(%rip), %rax
	shrb	$4, %r9b
	addq	%rax, %rdx
	pxor	%xmm0, %xmm0
	movzbl	%r9b, %r9d
	salq	$4, %r9
	addq	%rdx, %r9
.L45:
	movups	%xmm0, (%rdx)
	addq	$16, %rdx
	cmpq	%r9, %rdx
	jne	.L45
	testb	$15, %cl
	je	.L46
	movl	%ecx, %edx
	andl	$-16, %edx
.L44:
	movzbl	%dl, %r9d
	addl	%r10d, %r9d
	movl	%r9d, %r9d
	addq	%r9, %rax
	.p2align 4
	.p2align 4
	.p2align 3
.L47:
	addl	$1, %edx
	movb	$0, (%rax)
	addq	$1, %rax
	cmpb	%cl, %dl
	jb	.L47
.L46:
	movq	$0, 168(%r8)
	movl	$255, %eax
	movw	%ax, 225(%r8)
.L43:
	addl	$1, 20+diagnostics(%rip)
	movl	$8, 116(%r8)
	leave
	ret
	.p2align 4,,10
	.p2align 3
.L48:
	xorl	%edx, %edx
	leaq	decoded_blocks(%rip), %rax
	jmp	.L44
	.p2align 4
	.globl	nova_resource_id
	.def	nova_resource_id;	.scl	2;	.type	32;	.endef
nova_resource_id:
	testq	%rcx, %rcx
	je	.L61
	movzbl	(%rcx), %edx
	xorl	%eax, %eax
	testb	%dl, %dl
	je	.L58
	movabsq	$1469598103934665603, %rax
	movabsq	$1099511628211, %r8
	.p2align 5
	.p2align 4
	.p2align 3
.L60:
	addq	$1, %rcx
	xorq	%rdx, %rax
	movzbl	(%rcx), %edx
	imulq	%r8, %rax
	testb	%dl, %dl
	jne	.L60
	testq	%rax, %rax
	movl	$1, %edx
	cmove	%rdx, %rax
	ret
	.p2align 4,,10
	.p2align 3
.L61:
	xorl	%eax, %eax
.L58:
	ret
	.p2align 4
	.globl	nova_resource_checksum
	.def	nova_resource_checksum;	.scl	2;	.type	32;	.endef
nova_resource_checksum:
	testq	%rcx, %rcx
	je	.L69
	testq	%rdx, %rdx
	je	.L69
	movl	$-1, %eax
	xorl	%r10d, %r10d
	.p2align 4
	.p2align 3
.L68:
	movzbl	(%rcx,%r10), %r8d
	xorl	%r8d, %eax
	movl	$8, %r8d
	.p2align 5
	.p2align 4
	.p2align 3
.L67:
	movl	%eax, %r9d
	andl	$1, %eax
	negl	%eax
	shrl	%r9d
	andl	$-306674912, %eax
	xorl	%r9d, %eax
	subb	$1, %r8b
	jne	.L67
	addq	$1, %r10
	cmpq	%rdx, %r10
	jb	.L68
	notl	%eax
	ret
.L69:
	xorl	%eax, %eax
	ret
	.p2align 4
	.globl	nova_resource_manager_shutdown
	.def	nova_resource_manager_shutdown;	.scl	2;	.type	32;	.endef
nova_resource_manager_shutdown:
	pushq	%rbp
	movq	%rsp, %rbp
	andq	$-16, %rsp
	cmpb	$0, 184+diagnostics(%rip)
	je	.L72
	leaq	116+registry(%rip), %rax
	leaq	used(%rip), %rdx
	leaq	38912(%rax), %rcx
	.p2align 6
	.p2align 4
	.p2align 3
.L75:
	cmpb	$0, (%rdx)
	je	.L74
	movl	$0, 24(%rax)
	movl	$1, (%rax)
.L74:
	addq	$304, %rax
	addq	$1, %rdx
	cmpq	%rax, %rcx
	jne	.L75
	xorl	%eax, %eax
	pxor	%xmm0, %xmm0
	addl	$1, 72+diagnostics(%rip)
	movq	$0, 112+diagnostics(%rip)
	movw	%ax, 184+diagnostics(%rip)
	movl	$0, 104+diagnostics(%rip)
	movups	%xmm0, decoded_blocks(%rip)
	movups	%xmm0, 16+decoded_blocks(%rip)
	movups	%xmm0, 32+decoded_blocks(%rip)
	movups	%xmm0, 48+decoded_blocks(%rip)
.L72:
	leave
	ret
	.p2align 4
	.globl	nova_resource_find
	.def	nova_resource_find;	.scl	2;	.type	32;	.endef
nova_resource_find:
	subq	$40, %rsp
	call	lookup_index
	cmpw	$-1, %ax
	je	.L82
	movswq	%ax, %rax
	addl	$1, 40+diagnostics(%rip)
	leaq	registry(%rip), %rdx
	imulq	$304, %rax, %rax
	addq	%rdx, %rax
.L78:
	addq	$40, %rsp
	ret
	.p2align 4,,10
	.p2align 3
.L82:
	addl	$1, 44+diagnostics(%rip)
	xorl	%eax, %eax
	jmp	.L78
	.p2align 4
	.globl	nova_resource_register_descriptor
	.def	nova_resource_register_descriptor;	.scl	2;	.type	32;	.endef
nova_resource_register_descriptor:
	movq	%rcx, %r8
	movq	%rdx, %r9
	testq	%rdx, %rdx
	je	.L84
	movq	$0, (%rdx)
.L84:
	cmpb	$0, 184+diagnostics(%rip)
	movl	$11, %edx
	je	.L166
	testq	%r8, %r8
	je	.L167
	movq	(%r8), %rdx
	testq	%rdx, %rdx
	je	.L167
	movzbl	(%rdx), %eax
	testb	%al, %al
	je	.L167
	movl	8(%r8), %r11d
	cmpl	$9, %r11d
	ja	.L167
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
	subq	$480, %rsp
	movl	12(%r8), %esi
	movl	%esi, %ebx
	testl	%esi, %esi
	je	.L103
	movq	16(%r8), %rdi
	movq	%rdi, %r14
	testq	%rdi, %rdi
	je	.L103
	movq	24(%r8), %rdi
	movq	%rdi, %r15
	testq	%rdi, %rdi
	je	.L103
	movl	48(%r8), %esi
	movl	%esi, %r12d
	cmpl	$2, %esi
	ja	.L103
	cmpl	$3, 52(%r8)
	ja	.L103
	cmpl	$2, 56(%r8)
	ja	.L103
	cmpl	$2, 60(%r8)
	ja	.L103
	movabsq	$1469598103934665603, %rsi
	xorl	%r10d, %r10d
	movq	%rdx, %rcx
	addq	136+diagnostics(%rip), %rdi
	setc	%r10b
	testq	%r10, %r10
	jne	.L103
.L89:
	xorq	%rsi, %rax
	addq	$1, %rcx
	movabsq	$1099511628211, %rsi
	imulq	%rsi, %rax
	movq	%rax, %rsi
	movzbl	(%rcx), %eax
	testb	%al, %al
	jne	.L89
	testq	%rsi, %rsi
	movl	$1, %eax
	movq	%r8, 16(%rbp)
	cmovne	%rsi, %rax
	movq	%r10, 40(%rsp)
	movl	%r11d, 52(%rsp)
	movq	%rax, %rcx
	movq	%rdx, 56(%rsp)
	movq	%rax, %r13
	movq	%r9, 24(%rbp)
	call	lookup_index
	movq	56(%rsp), %rdx
	movl	52(%rsp), %r11d
	cmpw	$-1, %ax
	movq	40(%rsp), %r10
	movq	16(%rbp), %r8
	jne	.L172
	movq	%r15, %rdx
	movq	%r14, %rcx
	movq	%r8, 16(%rbp)
	movq	%r10, 56(%rsp)
	call	nova_resource_checksum
	movl	%eax, %edi
	testl	%eax, %eax
	je	.L170
	movq	16(%rbp), %r8
	addl	$1, %r11d
	movq	56(%rsp), %r10
	movq	24(%rbp), %r9
	movl	32(%r8), %eax
	testl	%eax, %eax
	je	.L100
	cmpl	%edi, %eax
	jne	.L170
.L100:
	movq	80(%r8), %rax
	movd	%r11d, %xmm0
	movd	%ebx, %xmm3
	testl	%r12d, %r12d
	punpckldq	%xmm3, %xmm0
	leaq	64(%rsp), %rdx
	leaq	112(%rsp), %rcx
	movq	%r9, 24(%rbp)
	movq	%rax, 152(%rsp)
	movl	96(%r8), %eax
	movq	%r10, 56(%rsp)
	movq	%r8, 16(%rbp)
	sete	164(%rsp)
	movq	%r13, 112(%rsp)
	movq	%r14, 128(%rsp)
	movq	%r15, 136(%rsp)
	movl	%edi, 144(%rsp)
	movl	%eax, 160(%rsp)
	movq	%xmm0, 120(%rsp)
	call	nova_resource_verify
	movq	16(%rbp), %r8
	movq	56(%rsp), %r10
	testl	%eax, %eax
	movq	24(%rbp), %r9
	movl	%eax, %edx
	jne	.L170
	movl	60(%r8), %r11d
	testl	%r11d, %r11d
	je	.L102
	movq	64(%r8), %rcx
	movl	72(%r8), %eax
	movq	%rcx, %rbx
	movl	%eax, %r15d
	testl	%eax, %eax
	je	.L103
	testq	%rcx, %rcx
	jne	.L106
	jmp	.L103
	.p2align 5
	.p2align 4,,10
	.p2align 3
.L107:
	addq	$1, %r10
	cmpq	$128, %r10
	je	.L171
.L106:
	leaq	used(%rip), %rax
	cmpb	$0, (%rax,%r10)
	jne	.L107
	xorl	%eax, %eax
	movd	%edi, %xmm0
	movd	%r11d, %xmm4
	movl	$38, %ecx
	punpckldq	%xmm4, %xmm0
	movd	12(%r8), %xmm5
	testl	%r11d, %r11d
	movq	(%r8), %rsi
	leaq	176(%rsp), %rdi
	movd	56(%r8), %xmm2
	movd	52(%r8), %xmm4
	movq	%r13, %xmm1
	movq	%rdi, 56(%rsp)
	movhps	40(%r8), %xmm1
	rep stosq
	punpckldq	%xmm5, %xmm2
	cmove	16(%r8), %rax
	movl	8(%r8), %ecx
	movups	%xmm1, 176(%rsp)
	movd	48(%r8), %xmm1
	movd	68(%rsp), %xmm5
	movq	%xmm0, 368(%rsp)
	movdqu	80(%r8), %xmm0
	punpckldq	%xmm4, %xmm1
	movq	%rbx, 336(%rsp)
	movq	%rsi, %rbx
	punpcklqdq	%xmm2, %xmm1
	movups	%xmm0, 376(%rsp)
	movd	96(%r8), %xmm0
	movups	%xmm1, 296(%rsp)
	movq	%rax, %xmm1
	movq	24(%r8), %rax
	movhps	16(%r8), %xmm1
	punpckldq	%xmm5, %xmm0
	movl	%ecx, 288(%rsp)
	movq	%rax, 360(%rsp)
	leaq	192(%rsp), %rax
	movl	%r15d, 312(%rsp)
	movb	$-1, 401(%rsp)
	movups	%xmm1, 344(%rsp)
	movq	%xmm0, 392(%rsp)
	testq	%rsi, %rsi
	je	.L109
	movzbl	(%rsi), %ecx
	testb	%cl, %cl
	je	.L109
	movq	56(%rsp), %r11
	movl	$1, %eax
	jmp	.L111
	.p2align 5
	.p2align 4,,10
	.p2align 3
.L110:
	addq	$1, %rax
	addq	$1, %r11
	cmpq	$96, %rax
	je	.L173
.L111:
	movb	%cl, 16(%r11)
	movzbl	(%rbx,%rax), %ecx
	testb	%cl, %cl
	jne	.L110
	leaq	192(%rsp,%rax), %rax
.L109:
	movb	$0, (%rax)
	movl	%r13d, %eax
	jmp	.L115
	.p2align 5
	.p2align 4,,10
	.p2align 3
.L112:
	addl	$1, %eax
	leal	256(%r13), %ecx
	cmpl	%eax, %ecx
	je	.L171
.L115:
	movzbl	%al, %ecx
	leaq	hash_slots(%rip), %rdi
	cmpw	$0, (%rdi,%rcx,2)
	jns	.L112
	movw	%r10w, (%rdi,%rcx,2)
	movslq	%r10d, %r10
	leaq	registry(%rip), %rax
	movq	56(%rsp), %rsi
	imulq	$304, %r10, %rcx
	addl	$1, diagnostics(%rip)
	leaq	(%rax,%rcx), %r11
	movl	$38, %ecx
	leaq	used(%rip), %rax
	movq	%r11, %rdi
	movb	$1, (%rax,%r10)
	rep movsq
	movq	24(%r8), %rax
	addq	%rax, 136+diagnostics(%rip)
	testq	%r9, %r9
	je	.L83
	movq	%r11, (%r9)
	jmp	.L83
.L102:
	movq	24(%r8), %rax
	movq	%rax, %rbx
	testq	%rax, %rax
	je	.L103
	movq	64(%r8), %rax
	movl	%edi, %r15d
	testq	%rax, %rax
	je	.L106
	cmpq	%rbx, %rax
	je	.L106
	.p2align 4
	.p2align 3
.L103:
	movl	64+diagnostics(%rip), %eax
	movl	$2, %edx
	addl	$1, %eax
	movl	%eax, 64+diagnostics(%rip)
.L83:
	leaq	-56(%rbp), %rsp
	movl	%edx, %eax
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
.L167:
	movl	64+diagnostics(%rip), %eax
	movl	$2, %edx
	addl	$1, %eax
	movl	%eax, 64+diagnostics(%rip)
.L166:
	movl	%edx, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L172:
	movswq	%ax, %rax
	leaq	registry(%rip), %rcx
	addl	$1, 60+diagnostics(%rip)
	xorl	%r8d, %r8d
	imulq	$304, %rax, %rax
	leaq	16(%rcx,%rax), %r9
	movzbl	(%r9), %eax
	testb	%al, %al
	jne	.L92
	jmp	.L174
	.p2align 4,,10
	.p2align 3
.L176:
	cmpb	%al, %cl
	jne	.L93
	leal	1(%r8), %r10d
	movzbl	(%r9,%r10), %eax
	movq	%r10, %r8
	testb	%al, %al
	je	.L175
.L92:
	movzbl	(%rdx,%r10), %ecx
	testb	%cl, %cl
	jne	.L176
.L93:
	movl	$9, %edx
	cmpb	%cl, %al
	je	.L83
.L99:
	movl	$3, %edx
	jmp	.L83
	.p2align 4,,10
	.p2align 3
.L170:
	addl	$1, 20+diagnostics(%rip)
	jmp	.L99
.L171:
	movl	$6, %edx
	jmp	.L83
.L175:
	movzbl	(%rdx,%r10), %ecx
	jmp	.L93
.L173:
	leaq	287(%rsp), %rax
	jmp	.L109
.L174:
	movzbl	(%rdx), %ecx
	jmp	.L93
	.p2align 4
	.globl	nova_resource_register
	.def	nova_resource_register;	.scl	2;	.type	32;	.endef
nova_resource_register:
	pushq	%rbp
	pxor	%xmm0, %xmm0
	movq	%rsp, %rbp
	andq	$-16, %rsp
	subq	$144, %rsp
	movq	48(%rbp), %rax
	movq	%rcx, 32(%rsp)
	leaq	32(%rsp), %rcx
	movq	%rax, 56(%rsp)
	movl	56(%rbp), %eax
	movl	%edx, 40(%rsp)
	xorl	%edx, %edx
	movl	%eax, 64(%rsp)
	movq	64(%rbp), %rax
	movups	%xmm0, 68(%rsp)
	movups	%xmm0, 84(%rsp)
	movl	$0, 132(%rsp)
	movl	%r8d, 44(%rsp)
	movq	%r9, 48(%rsp)
	movq	%rax, 72(%rsp)
	movl	$2, 84(%rsp)
	movups	%xmm0, 100(%