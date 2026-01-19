.global gdt_flush

gdt_flush:
	mov 4(%esp), %eax

	lgdt 4(%eax)

	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss

	ljmp $0x08, $.flush

.flush:
	ret

