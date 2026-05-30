.section .text
.global copy_from_user
.global copy_to_user

copy_from_user:
	push %ebp
	mov %esp, %ebp

	push %edi
	push %esi

	mov 8(%ebp), %edi
	mov 12(%ebp), %esi
	mov 16(%ebp), %ecx

1:
	rep movsb
2:
	mov %ecx, %eax
	pop %esi
	pop %edi
	pop %ebp
	ret

	.section .fixup, "ax"
3:
	jmp 2b
	.section __ex_table, "a"
	.align 4
	.long 1b, 3b

copy_to_user:
	push %ebp
	mov %esp, %ebp

	push %edi
	push %esi

	mov 8(%ebp), %edi
	mov 12(%ebp), %esi
	mov 16(%ebp), %ecx

1:
	rep movsb
2:
	mov %ecx, %eax
	pop %esi
	pop %edi
	pop %ebp
	ret

	.section .fixup, "ax"
3:
	jmp 2b
	.section __ex_table, "a"
	.align 4
	.long 1b, 3b

