.global load_page_directory
.global enable_paging
.global reload_tlb

load_page_directory:
	push %ebp
	mov %esp, %ebp
	mov 8(%ebp), %eax
	mov %eax, %cr3
	pop %ebp
	ret

enable_paging:
	push %ebp
	mov %esp, %ebp
	mov %cr0, %eax
	or $0x80000000, %eax
	mov %eax, %cr0
	pop %ebp
	ret

reload_tlb:
	push %ebp
	mov %esp, %ebp

	mov 8(%ebp), %eax # cogemos el primer argumento
	invlpg (%eax)

	pop %ebp
	ret

