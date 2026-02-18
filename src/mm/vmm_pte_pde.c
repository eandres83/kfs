#include "vmm_pte_pde.h"

// Page table functions
inline void pt_entry_add_attrib(pt_entry *e, uint32_t attrib)
{
	*e |= attrib;
}

inline void pt_entry_del_attrib(pt_entry *e, uint32_t attrib)
{
	*e &= ~attrib;
}

inline void pt_entry_set_frame(pt_entry *e, uint32_t addr)
{
	*e = (*e & ~PTE_FRAME) | addr;
}

inline bool pt_entry_is_present(pt_entry e)
{
	return (e & PTE_PRESENT);
}

inline bool pt_entry_is_writable(pt_entry e)
{
	return (e & PTE_WRITABLE);
}

inline uint32_t pt_entry_frame(pt_entry e)
{
	return (e & PTE_FRAME);
}

// Page directory functions
inline void pd_entry_add_attrib(pd_entry *e, uint32_t attrib)
{
	*e |= attrib;
}

inline void pd_entry_del_attrib(pd_entry *e, uint32_t attrib)
{
	*e &= ~attrib;
}

inline void pd_entry_set_frame(pd_entry *e, uint32_t addr)
{
	*e = (*e & ~PDE_FRAME) | addr;
}

inline bool pd_entry_is_present(pd_entry e)
{
	return (e & PDE_PRESENT);
}

inline bool pd_entry_is_writable(pd_entry e)
{
	return (e & PDE_WRITABLE);
}

inline bool pd_entry_is_user(pd_entry e)
{
	return (e & PDE_USER);
}

