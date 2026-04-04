.global jump_to_usermode
.global swtch

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

swtch:
	mov 4(%esp), %eax # %eax = struct context **old
	mov 8(%esp), %edx # %edx = struct context *new_proc

	push %ebp
	push %ebx
	push %esi
	push %edi

	mov %esp, (%eax) # guardamos el %esp actual dentro de la variable a la que apunta %eax (*old = esp)

	mov %edx, %esp # cargamos el nuevo %esp que estaba en %edx (esp = new_proc

	pop %edi
	pop %esi
	pop %ebx
	pop %ebp

	ret

