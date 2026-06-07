.global _start

_start:
	movl (%esp), %eax
	leal 4(%esp), %ebx
	# ebx = esp + 4

	# sintaxis offset(registro_base, registro_indice, multiplicador)
	# eax = argc, to skip argv
	leal 8(%esp, %eax, 4), %ecx
	# ecx = %esp + (%eax * 4) + 8

	pushl %ecx
	pushl %ebx
	pushl %eax

	call main

	movl %eax, %ebx
	movl $1, %eax
	int $0x80 

