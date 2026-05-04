#ifndef ELF_H
#define ELF_H

#include <utils.h>

typedef uint32_t 	Elf32_Addr; // Unsigned program address
typedef uint16_t 	Elf32_Half; // Unsigned medium integer
typedef uint32_t	Elf32_Off; // Unsigned file offset
typedef int32_t		Elf32_Sword;  // Signed large integer
typedef uint32_t	Elf32_Word; // Unsigned large integer

#define EI_MAG0		0  // first 4 bytes hold a magic number
#define EI_MAG1		1  // first 4 bytes hold a magic number
#define EI_MAG2		2  // first 4 bytes hold a magic number
#define EI_MAG3		3  // first 4 bytes hold a magic number
#define EI_CLASS	4  // identifies the file's class or capacity
#define EI_DATA		5  // data encoding of the processor-specific data
#define EI_VERSION	6  // ELF header version number
#define EI_PAD		7  // mark the beginning of the unused bytes in e_ident
#define EI_NIDENT	16 // unused bytes

struct elf32_ehdr
{
	unsigned char	e_ident[EI_NIDENT];	// 
	Elf32_Half	e_type; 		// object file type
	Elf32_Half	e_machine;		// required architecture for an individual file
	Elf32_Word	e_version;		// object file version
	Elf32_Addr	e_entry;		// virtual address, thus starting process
	Elf32_Off	e_phoff;		// program header table's file offset in bytes
	Elf32_Off	e_shoff;		// section header tabls's file offset in bytes
	Elf32_Word	e_flags;		// processor-specific flags
	Elf32_Half	e_phentsize;		// size of one entry in the file's program header table
	Elf32_Half	e_phnum;		// number of entries in the program header table
	Elf32_Half	e_shentsize;		// section header size in bytes
	Elf32_Half	e_shnum;		// number of entries in the section header table
	Elf32_Half	e_shstrndx;		// section header table index of the entry associated with the section name string table
};

struct elf32_phdr
{
	Elf32_Word	p_type;		// what kind of segment this array element describes
	Elf32_Off	p_offset;	// offset from the beginning of the file at which the first byte of the segment
	Elf32_Addr	p_vaddr;	// virtual address at which the first byte of the segment resides in memory
	Elf32_Addr	p_paddr;	// segment's physical address, on system for which physical addr is relevant
	Elf32_Word	p_filesz;	// number of bytes in the file image of the segment
	Elf32_Word	p_memsz;	// number of bytes in the memory image ot the segment
	Elf32_Word	p_flags;	// flags relevant to the segment
	Elf32_Word	p_align;	// value to which the segment are aligned in memory and in the file
};

// segment types -> p_type
#define PT_NULL		0		// array element is unused
#define PT_LOAD		1		// array element specifies a loadable segment, described by p_filesz
#define PT_DYNAMIC	2		// dynamic linking info
#define PT_INTERP	3		// location and size of a null-terminated path name
#define PT_NOTE		4		// location and size of auxiliary info
#define PT_SHLIB	5		// unspecified semantics
#define PT_PHDR		6		// if present specifies the location and size of the program header table itself
#define PT_LOPROC	0x70000000	// values in this inclusive range are reserved for processor-specific semantics
#define PT_HIPROC	0x7fffffff	// same that PT_LOPROC

struct elf32_shdr
{
	Elf32_Word	sh_name;
	Elf32_Word	sh_type;
	Elf32_Word	sh_flags;
	Elf32_Addr	sh_addr;	// address at which the section's first byte should reside
	Elf32_Off	sh_offset;	// byte offset from the beginning of the file to the first byte in section
	Elf32_Word	sh_size;	// section size in bytes
	Elf32_Word	sh_link;	// section header table index link
	Elf32_Word	sh_info;	// exta info depends on the section type
	Elf32_Word	sh_addralign;	// some sections have address alignment constraints, 0 and 1 mean section has no alignment constraints
	Elf32_Word	sh_entsize;	// some sections hold a table of fixed-size entries
};

// section types -> sh_type
#define SHT_NULL	0		// this value marks the section header as inactive
#define SHT_PROGBITS	1		// section holds information defined by the program
#define SHT_SYMTAB	2		// these sections hold a symbol table
#define SHT_STRTAB	3		// string table
#define SHT_RELA	4		// relocation entries with explicit addends
#define SHT_HASH	5		// symbol hash table
#define SHT_DYNAMIC	6		// info for dynamic linking
#define SHT_NOTE	7		// info that marks the file in some way
#define SHT_NOBITS	8		// a section of this type occupies no space in the file
#define SHT_REL		9		// relocation entries without explicit addends
#define SHT_SHLIB	10		// this section type is reserved but has unspecified semantics
#define SHT_DYNSYM	11		// these sections hold a symbol table
#define SHT_LOPROC	0x70000000	// values in this inclusive range are reserved for procesessor-specific
#define SHT_HIPROC	0x7fffffff	// values in this inclusive range are reserved for procesessor-specific
#define SHT_LOUSER	0x80000000	// lower bound of the range of indexes reserved for application programs
#define SHT_HIUSER	0xffffffff	// upper bound of the range of indexes reserved for application programs

#endif 
