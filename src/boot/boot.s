/*
KFS Kernel Entry Point.

This file handles the transition from the bootloader (GRUB) enviroment
to the High-Level C Kernel.

Rationale for Higher Half:
The kernel is linked to run at 3GB (0xC010000), but loaded by GRUB
at physical 1MB (0x00100000). We must setup a temporary page table to
translate virtual address to physical addresses before jumping to the
C code, otherwise the CPU will fault when accessing symbols.
*/

.set ALIGN,	1<<0		/* align loaded modules on page boundaries. */
.set MEMINFO,	1<<1		/* provide memory map */
.set FLAGS,	ALIGN | MEMINFO	/* this is the Multiboot 'flag' field */
.set MAGIC,	0x1BADB002	/* 'magic number' lets bootloader find the header */
.set CHECKSUM,	-(MAGIC + FLAGS)/* checksum of above, to prove we are multiboot */
.set KERNEL_VIRTUAL_BASE, 0xC0000000
.set KERNEL_PAGE_NUMBER, (KERNEL_VIRTUAL_BASE >> 22)

/*
Declare a multiboot header that marks the program as a kernel. There are maginc
values that are documented in the multiboot standard. The bootloader will
search for this signature in the first 8 KiB of the kernel file, aligned at a 
32-bit boundary. The signature is in its own section so the header can be 
forced to be within the first 8 KiB of the kernel file. */
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

/*
The multiboot standard does not define the value of the stack pointer register
(esp) and it is up to the kernel to provide a stack. This allocates room for a
small stack by creating s symbol at the bottom of it, the allocating 16384
bytes for it, and finally creating a symbol at the top. The stack grows
downwards on x86. The stack is in its own section so it can be marked nobits,
which means the kernel file is smaller because it does not contain an unitialized
stack. The stack on x86 must be 16-byte aligned according to the System V ABI
standard and de-facto extensions. The compiler will assume the stack is properly
aligned and failure to align the stack will result in undefined behavior.
*/
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

/* Preallocate pages used for paging. Don't hard-code addresses and assume they
are available, as the bootloader might have loaded its multiboot structure or
modules there. This lets the bootloader know it must avoid the addresses.
*/
.section .bss
.align 4096
boot_page_directory:
.skip 4096
boot_page_table1:
.skip 4096

/*
The linked script specifies _start as the entry point to the kernel and the 
bootloader will jump to this position once the kernel has been loaded. It
dosen't make sence to return from this function as the bootloader is gone.
*/
.section .text
.global _start
.type	_start, @function
_start:
	/* LOAD PHYSICAL ADDRESSES
	   Paginng is NOT enabled yet. We cannot access 'boot_page_table1' directly
	   because the linker assumes it is at 0xC0... (Virtual).
	   We must subtract KERNEL_VIRUTAL_BASE to obtain the physical address
	   that the CPU can use right now.
	*/
	movl $(boot_page_table1 - KERNEL_VIRTUAL_BASE), %edi
	/* Map the first 4MB of physical memory (Identity Mapping)
	   This covers: VGA, Multiboot Info, GDT, and the Kernel code itself. */
	movl $0, %esi
	movl $1023, %ecx

1:
#	/* Map logic: Physical Address | Present (1) | Writable (2)
#	   Note: We map everythig as writable for now. In the future, .text
#	   should be Read-Only for security.
#	*/
#	cmpl $(_kernel_start - KERNEL_VIRTUAL_BASE), %esi
#	jl 2f
#	cmpl $(_end - KERNEL_VIRTUAL_BASE), %esi
#	jge 3f

	movl %esi, %edx
	orl $0x003, %edx
	movl %edx, (%edi)

2:
	addl $4096, %esi
	# Size of entries in boot_page_table1 is 4 bytes.
	addl $4, %edi
	# Loop to the next entry if we haven't finished.
	loop 1b

3:
	/* VGA MAPPGIN TRICK:
	   Map the physical VGA buffer (0xB8000) to the last page of this table.
	   Virtual Address: 0xC0000000 + (1023 * 4096) = 0xC03FF000.
	*/
	movl $(0x000B8000 | 0x003), boot_page_table1 - 0xC0000000 + 1023 * 4

	/*
	RECURSIVE / DOUBLE MAPPING:
	1. Entry 0: Maps 0-4MB Virtual -> 0-4MB Physical.
	REQUIRED because EIP is currently in low memory (0x001...).
	If we don't do this, the very next instruction after enabling paging
	will cause a Page Fault.
	
	2. Entry 786: Maps 3GB-3GB+4MB Virtual -> 0-4MB Physical
	This is where the kernel "thinks" it lives.
	*/
	movl $(boot_page_table1 - KERNEL_VIRTUAL_BASE + 0x003), boot_page_directory - KERNEL_VIRTUAL_BASE + 0
	movl $(boot_page_table1 - KERNEL_VIRTUAL_BASE + 0x003), boot_page_directory - KERNEL_VIRTUAL_BASE + 768 * 4

	# Load Page Directory Physical Address into CR3
	movl $(boot_page_directory - KERNEL_VIRTUAL_BASE), %ecx
	movl %ecx, %cr3

	/* ENABLE PAING:
	Set PG (bit 31) in CR0
	The CPU is now in Paging Mode, but EIP is still physically low.
	*/
	movl %cr0, %ecx
	orl $0x80010000, %ecx
	movl %ecx, %cr0

	# Jump to higher half with an absolute jump.
	lea 4f, %ecx
	jmp *%ecx

.section .text

4:
	# At this point, paging is fully set up and enabled.

	# Reload cr3 to force a TLB flush so the change to take effect.
	movl %cr3, %ecx
	movl %ecx, %cr3

	/*
	To set up a stack, we set the esp register to point to the top of the
	stack (as it grows downwards on x86 systems). This is necessarily done
	in assembly as languages such as C cannot function without a stack.
	*/
	mov $stack_top, %esp
	/*
	This is a good place to initialize crucial processor state before the
	high-level kernel is entered. It's best to minimize the early enviroment
	where crucial features are offline. Nota that the processor is not fully
	initialized yet: Features such as floating point instructions and instruction
	set extensions are not initialized yet. The GDT should be loaded here. Paging
	should be enabled here. C++ features such as global constructors and exceptions
	will requier runtime support to work as well.
	*/
	push %ebx
	push %eax
	/*
	Enter the high-level kernel. The ABI requires the stack is 16-bytes
	aligned at the time of the call instructino (which afterwards pushes
	the return pointer of size 4 bytes). The stack was originally 16-byte
	aligned above and we've pushed a multiple of 16 bytes to the stack since
	( pushed 0 bytes so far), so the alignment has thus been preserved and the
	call is well defined.
	*/

	call kernel_main

	/*
	If the system has nothign more to do, put the computer into an infinite loop.
	To do that:
	1) Disable interrupts with cli (clear interrupt enable in eflags).
	   They are already disabled by the bootloader, so this is not needed.
	   Mind that you might later enable interrupts and return from
	   kernel_main (which is sort of nonsensical to do).
	2) Wait for the next interrupt to arrive with hlt (halt instruction).
	   Since they are disable, this will lock up the computer.
	3) Jump to the hlt instruction if it ever wakes up due to a 
	   non-maskable interrupt occurring or due to system management mode.
	*/
	cli
1:	hlt
	jmp 1b

/*
Set the size of the _start symbol to the current location '.' minus its start.
This is useful whe debugging or when you implement call tracing.
*/
.size _start, . - _start

