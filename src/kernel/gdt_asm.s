.global gdt_flush
.global get_stack_pointer

gdt_flush:
	mov 4(%esp), %eax

	lgdt (%eax)

	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss

	ljmp $0x08, $.flush

.flush:
	ret

get_stack_pointer:
	mov %esp, %eax
	ret


