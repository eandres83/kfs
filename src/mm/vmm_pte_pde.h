#ifndef VMM_PTE_PDE_H
#define VMM_PTE_PDE_H

#include "vmm.h"

typedef uint32_t pd_entry;
typedef uint32_t pt_entry;

enum PAGE_PTE_FLAGS
{
	PTE_PRESENT 		= 1,		// 0000000000000000000000000000001
	PTE_WRITABLE 		= 2,		// 0000000000000000000000000000010
	PTE_USER 		= 4,		// 0000000000000000000000000000100
	PTE_WRITETHOUGH 	= 8,		// 0000000000000000000000000001000
	PTE_NOT_CACHEABLE 	= 0x10,		// 0000000000000000000000000010000
	PTE_ACCESSED		= 0x20,		// 0000000000000000000000000100000
	PTE_DIRTY 		= 0x40,		// 0000000000000000000000001000000
	PTE_PAT 		= 0x80,		// 0000000000000000000000010000000
	PTE_CPU_GLOBAL 		= 0x100,	// 0000000000000000000000100000000
	PTE_LV4_GLOBAL 		= 0x200,	// 0000000000000000000001000000000
	PTE_FRAME 		= 0x7FFFF000	// 1111111111111111111000000000000
};

enum PAGE_PDE_FLAGS
{
	PDE_PRESENT		= 1,		// 0000000000000000000000000000001
	PDE_WRITABLE		= 2,		// 0000000000000000000000000000010
	PDE_USER		= 4,		// 0000000000000000000000000000100
	PDE_PWD			= 8,		// 0000000000000000000000000001000
	PDE_PCD			= 0x10,		// 0000000000000000000000000010000
	PDE_ACCESSED		= 0x20,		// 0000000000000000000000000100000
	PDE_DIRTY		= 0x40,		// 0000000000000000000000001000000
	PDE_4MB			= 0x80,		// 0000000000000000000000010000000
	PDE_CPU_GLOBAL		= 0x100,	// 0000000000000000000000100000000
	PDE_LV4_GLOBAL		= 0x200,	// 0000000000000000000001000000000
	PDE_FRAME		= 0x7FFFF000	// 1111111111111111111000000000000
};

static inline void pt_entry_add_attrib(pt_entry *e, uint32_t attrib)
{
	*e |= attrib;
}

static inline void pt_entry_del_attrib(pt_entry *e, uint32_t attrib)
{
	*e &= ~attrib;
}

static inline void pt_entry_set_frame(pt_entry *e, uint32_t addr)
{
	*e = (*e & ~PTE_FRAME) | addr;
}

static inline bool pt_entry_is_present(pt_entry e)
{
	return (e & PTE_PRESENT);
}

static inline bool pt_entry_is_writable(pt_entry e)
{
	return (e & PTE_WRITABLE);
}

static inline bool pt_entry_frame(pt_entry e)
{
	return (e & PTE_FRAME);
}

// Page directory functions
static inline void pd_entry_add_attrib(pd_entry *e, uint32_t attrib)
{
	*e |= attrib;
}

static inline void pd_entry_del_attrib(pd_entry *e, uint32_t attrib)
{
	*e &= ~attrib;
}

static inline void pd_entry_set_frame(pd_entry *e, uint32_t addr)
{
	*e = (*e & ~PDE_FRAME) | addr;
}

static inline bool pd_entry_is_present(pd_entry e)
{
	return (e & PDE_PRESENT);
}

static inline bool pd_entry_is_writable(pd_entry e)
{
	return (e & PDE_WRITABLE);
}

static inline bool pd_entry_is_user(pd_entry e)
{
	return (e & PDE_USER);
}

#endif
