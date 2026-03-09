.extern isr_handler
.extern irq_handler
.global idt_flush

idt_flush:
	mov 4(%esp), %eax # get teh pointer to the IDT, passed as parameter
	lidt (%eax)
	ret

.macro ISR_NOERRCODE code
	.global isr\code
	isr\code:
		cli		# Disable interrupts
		push $0		# Push a dummy error code
		push $\code	# Push the interrupt number
		jmp isr_common
.endm

.macro ISR_ERRCODE code
	.global isr\code
	isr\code:
		cli
		push $\code	# Push the interrupt number
		jmp isr_common
.endm

.macro IRQ original, remapped
	.global irq\original
	irq\original:
		cli
		push $0
		push $\remapped
		jmp irq_common
.endm

IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_ERRCODE   17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_ERRCODE   21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31
ISR_NOERRCODE 128

isr_common:
	pusha

	mov %ds, %ax	# lower 16 bits of eax=ax
	push %eax	# save teh ds

	mov $0x10, %ax # kernel code segment
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	push %esp	# pasa la pila (struct) como puntero C
	call isr_handler
	add $4, %esp	# limpiar el parametro que acabo de empujar

	pop %eax 	# reload the original ds
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	popa
	add $8, %esp	# cleans up the pushed error code and pushed ISR number
	sti		# set interrupt flag
	iret		# interrupt return

irq_common:
	pusha

	mov %ds, %ax	# lower 16 bits of eax=ax
	push %eax	# save teh ds

	mov $0x10, %ax # kernel code segment
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	push %esp	# pasa la pila (struct) como puntero C
	call irq_handler
	add $4, %esp	# limpiar el parametro que acabo de empujar

	pop %eax 	# reload the original ds
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	popa
	add $8, %esp	# cleans up the pushed error code and pushed ISR number
	sti		# set interrupt flag
	iret		# interrupt return
