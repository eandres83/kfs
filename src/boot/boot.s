/* Decalre constants for the multiboot header. */
.set ALIGN,	1<<0		/* align loaded modules on page boundaries. */
.set MEMINFO,	1<<1		/* provide memory map */
.set FLAGS,	ALIGN | MEMINFO	/* this is the Multiboot 'flag' field */
.set MAGIC,	0x1BADB002	/* 'magic number' lets bootloader find the header */
.set CHECKSUM,	-(MAGIC + FLAGS)/* checksum of above, to prove we are multiboot */

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
	# Physical address of boot_page_table1.
	movl $(boot_page_table1 - 0xC0000000), %edi
	# First address to map is address 0.
	movl $0, %esi
	# Map 1023 pages. The 1024th will be the VGA text buffer.
	movl $1023, %ecx

1:
	# Only map the kernel
	cmpl $_kernel_start, %esi
	jl 2f
	cmpl $(_end - 0xC0000000), %esi
	jge 3f

	# Map physical address as "present, writable". Note that this maps
	# .text and .rodata as writable. Mind security and map them as non-writable.
	movl %esi, %edx
	orl $0x003, %edx
	movl %edx, (%edi)

2:
	# Size of page is 4096 bytes.
	addl $4096, %esi
	# Size of entries in boot_page_table1 is 4 bytes.
	addl $4, %edi
	# Loop to the next entry if we haven't finished.
	loop 1b

3:
	# Map VGA video memory to 0xC03FF000 as "present, wirtable".
	movl $(0x000B8000 | 0x003), boot_page_table1 - 0xC0000000 + 1023 * 4

	/*
	The page table is used at both page directory entry 0 (virtually form 0x0
	to 0x3FFFFF) (thus identity mapping the kernel) and page directory entry
	768 (virtually form 0xC0000000 to 0xC03FFFFF) (thus mapping it in the
	higher half). The kernel is identity mapped because enablgin paging does
	not change the next instruction, which continues to be physical. The CPU
	would instead page fault if there was no identity mapping.
	*/

	# Map the page table to both virtual address 0x00000000 and 0xC0000000.
	movl $(boot_page_table1 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 0
	movl $(boot_page_table1 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 768 * 4

	# Set cr3 the address of the boot_page_directory.
	movl $(boot_page_directory - 0xC0000000), %ecx
	movl %ecx, %cr3

	# Enable paging and the write-protected bit.
	movl %cr0, %ecx
	orl $0x80010000, %ecx
	movl %ecx, %cr0

	# Jump to higher half with an absolute jump.
	lea 4f, %ecx
	jmp *%ecx

.section .text

4:
	# At this point, paging is fully set up and enabled.

	# Unmap the identity paggind as it is now unnecessary
	movl $0, boot_page_directory + 0

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

