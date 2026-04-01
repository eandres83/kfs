.global jump_to_usermode

jump_to_usermode:
	cli

	# cargar User Data (Ring 3)
	mov $0x2B, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	mov 8(%esp), %ebx # ebx = user_stack
	mov 4(%esp), %ecx # ecx = entry_point

	push $0x2B	# SS Segmento de pila (User Data)
	push %ebx	# ESP puntero a la pila de Usuario
	push $0x202	# EFLAGS: Activamos interrupciones
	push $0x23	# CS Segmento de Codigo (User Code)
	push %ecx	# EIP la direccion de memoria donde queremos saltar

	iret

