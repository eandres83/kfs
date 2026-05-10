.global crt0

crt0:
	movl (%esp), %eax
	leal 4(%esp), %ebx

	# sintaxis offset(registro_base, registro_indice, multiplicador)
	leal 8(%esp, %eax, 4), %ecx

	pushl %ecx
	pushl %ebx
	pushl %eax

	call main

	movl %eax, %ebx
	movl $1, %eax
	int $0x80 
