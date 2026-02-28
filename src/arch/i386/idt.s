.global idt_flush

idt_flush:
	mov eax, [esp + 4] ; get teh pointer to the IDT, passed as parameter
	lidt [eax]
	ret
