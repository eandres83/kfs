#include "idt.h"
#include "task/task.h"

extern void idt_flush(uint32_t);

idt_entry_t 	idt_entries[256];
idt_ptr_t	idt_ptr;
isr_t		interrupts[256];

static char *error_msg[22] = {"Divide Error", "Debug Exception", "NMI Interrupt", "Breakpoint", "Overflow", "Out of Bounds",
		"Invalid Opcode", "Device Not Available", "Double Fault", "Coprocessor Segment Overrun", "Invalid TSS",
		"Segment Not Present", "Stack-Segment Fault", "General Protection", "Page Fault", "Inter reserved", 
		"Math Fault", "Alignment Check", "Machine Check", "SIMD Floating-Point Exception", "Virtualization Exception",
		"Control Protection Exception"};

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t seg, uint8_t flags)
{
	idt_entries[num].base_lo = base & 0xFFFF;
	idt_entries[num].base_hi = (base >> 16) & 0xFFFF;
	idt_entries[num].seg = seg;
	idt_entries[num].always0 = 0;
	idt_entries[num].flags = flags;
}

void	init_idt()
{
	// pone los PICs en modo escucha
	outb(0x20, 0x11);
	outb(0xA0, 0x11);

	// enviar el vector offset (el nuevo numero de inicio)
	outb(0x21, 0x20);
	outb(0xA1, 0x28);

	// configurar la conexion entre master and slave
	outb(0x21, 0x04);
	outb(0xA1, 0x02);

	// working on 8086/x86 normal
	outb(0x21, 0x01);
	outb(0xA1, 0x01);

	outb(0x21, 0x00);
	outb(0xA1, 0x00);

	idt_ptr.limit = sizeof(idt_entry_t) * 256 -1;
	idt_ptr.base = (uint32_t)&idt_entries;

	kmemset(&idt_entries, 0, sizeof(idt_entry_t)*256);

	idt_set_gate(0, (uint32_t)isr0, 0x08, 0x8E);
	idt_set_gate(1, (uint32_t)isr1, 0x08, 0x8E);
	idt_set_gate(2, (uint32_t)isr2, 0x08, 0x8E);
	idt_set_gate(3, (uint32_t)isr3, 0x08, 0x8E);
	idt_set_gate(4, (uint32_t)isr4, 0x08, 0x8E);
	idt_set_gate(5, (uint32_t)isr5, 0x08, 0x8E);
	idt_set_gate(6, (uint32_t)isr6, 0x08, 0x8E);
	idt_set_gate(7, (uint32_t)isr7, 0x08, 0x8E);
	idt_set_gate(8, (uint32_t)isr8, 0x08, 0x8E);
	idt_set_gate(9, (uint32_t)isr9, 0x08, 0x8E);
	idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E);
	idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E);
	idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E);
	idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E);
	idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E);
	idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E);
	idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E);
	idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E);
	idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E);
	idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E);
	idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8E);
	idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8E);
	idt_set_gate(22, (uint32_t)isr22, 0x08, 0x8E);
	idt_set_gate(23, (uint32_t)isr23, 0x08, 0x8E);
	idt_set_gate(24, (uint32_t)isr24, 0x08, 0x8E);
	idt_set_gate(25, (uint32_t)isr25, 0x08, 0x8E);
	idt_set_gate(26, (uint32_t)isr26, 0x08, 0x8E);
	idt_set_gate(27, (uint32_t)isr27, 0x08, 0x8E);
	idt_set_gate(28, (uint32_t)isr28, 0x08, 0x8E);
	idt_set_gate(29, (uint32_t)isr29, 0x08, 0x8E);
	idt_set_gate(30, (uint32_t)isr30, 0x08, 0x8E);
	idt_set_gate(31, (uint32_t)isr31, 0x08, 0x8E);

	idt_set_gate(32, (uint32_t)irq0, 0x08, 0x8E);
	idt_set_gate(33, (uint32_t)irq1, 0x08, 0x8E);
	idt_set_gate(34, (uint32_t)irq2, 0x08, 0x8E);
	idt_set_gate(35, (uint32_t)irq3, 0x08, 0x8E);
	idt_set_gate(36, (uint32_t)irq4, 0x08, 0x8E);
	idt_set_gate(37, (uint32_t)irq5, 0x08, 0x8E);
	idt_set_gate(38, (uint32_t)irq6, 0x08, 0x8E);
	idt_set_gate(39, (uint32_t)irq7, 0x08, 0x8E);
	idt_set_gate(40, (uint32_t)irq8, 0x08, 0x8E);
	idt_set_gate(41, (uint32_t)irq9, 0x08, 0x8E);
	idt_set_gate(42, (uint32_t)irq10, 0x08, 0x8E);
	idt_set_gate(43, (uint32_t)irq11, 0x08, 0x8E);
	idt_set_gate(44, (uint32_t)irq12, 0x08, 0x8E);
	idt_set_gate(45, (uint32_t)irq13, 0x08, 0x8E);
	idt_set_gate(46, (uint32_t)irq14, 0x08, 0x8E);
	idt_set_gate(47, (uint32_t)irq15, 0x08, 0x8E);

	// set syscall gate, 0xEE ring 3
	idt_set_gate(128, (uint32_t)isr128, 0x08, 0xEE);

	idt_flush((uint32_t)&idt_ptr);

	register_interrupt_handler(128, &syscall_callback);
}

void	isr_handler(registers_t *regs)
{
	if (interrupts[regs->int_no] != NULL)
	{
		isr_t action = interrupts[regs->int_no];
		action(regs);
		return ;
	}

	if ((regs->cs & 3) == 3)
	{
		char *motivo = "Unknown Exception";
		if (regs->int_no < 22)
			motivo = error_msg[regs->int_no];
		kill_process(motivo);
		return ;
	}

	terminal_initialize();

	kprintf("EAX: 0x%x, ECX: 0x%x, EDX: 0x%x, EBX: 0x%x\n", regs->eax, regs->ecx, regs->edx, regs->ebx);
	kprintf("ESP: 0x%x, EBP: 0x%x, ESI: 0x%x, EDI: 0x%x\n", regs->esp, regs->ebp, regs->esi, regs->edi);
	kprintf("EIP: 0x%x, CS: 0x%x, EFLAGS: 0x%x, err_code: %d\n", regs->eip, regs->cs, regs->eflags, regs->err_code);

	if (regs->int_no < 21 && error_msg[regs->int_no] != NULL)
		PANIC(error_msg[regs->int_no]);
	else
		PANIC("Critical External Interrupt\n");
}

void	irq_handler(registers_t *regs)
{
	// send EOI (End of Interrupt)
	if (regs->int_no >= 40)
	{
		// send reset signal to slave
		outb (0xA0, 0x20);
	}
	// send reset signal to master
	outb(0x20, 0x20);

	if (interrupts[regs->int_no] != 0)
	{
		isr_t action = interrupts[regs->int_no];
		action(regs);
	}
}

void	register_interrupt_handler(uint8_t n, isr_t action)
{
	interrupts[n] = action;
}

