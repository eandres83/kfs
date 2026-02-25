# KFS-3 - Memory Management

![Arch](https://img.shields.io/badge/arch-x86-lightgrey)
![Memory](https://img.shields.io/badge/memory-Paging%20%2F%20VMM-red)
![Allocator](https://img.shields.io/badge/allocator-kmalloc-blue)
![Status](https://img.shields.io/badge/milestone-completed-success)

<br />
<p align="center">
  <h3 align="center">Phase 3: The Sweet World of Memory</h3>
</p>

## 🗣️ About The Project

**KFS-3** introduces a complete memory management subsystem into the kernel. Following the x86 architecture specifications, this phase moves the kernel from raw physical memory access to a structured, paged virtual memory environment. 

This update provides the necessary tools to allocate and free dynamic memory (`kmalloc` / `kfree`), handle kernel panics, and safely manage User vs. Kernel space memory boundaries.

### 🎯 Key Engineering Achievements
- **Physical Memory Manager (PMM):** Implemented a bitmap-based system to track available and used 4KB memory frames based on the Multiboot memory map.
- **Virtual Memory Manager (VMM):** Configured x86 Hardware Paging (Page Directories and Page Tables). The kernel now operates securely within its own virtual address space (Higher Half Kernel pattern).
- **Dynamic Heap Allocator:** Developed a custom `kmalloc` / `kfree` algorithm using a slab/zone approach to manage memory blocks dynamically and prevent fragmentation.
- **Kernel Panics:** Added graceful failure states (`panic.c`) to halt the CPU when encountering unrecoverable system errors.
- **Debugging Tools (Bonus):** Expanded the interactive shell with advanced memory inspection commands like `meminfo`, `virt2phys`, and memory hex dumping.

---

## 🏗️ Memory Architecture

The memory subsystem is divided into three abstraction layers:

~~~mermaid
graph TD
    A["Physical Memory (RAM)"] -->|Tracked by Bitmap| PMM["PMM (Physical Memory Manager)"]
    PMM -->|Provides 4KB Frames| VMM["VMM (Paging / MMU)"]
    VMM -->|Maps Virtual to Physical| HEAP["Heap Allocator (kmalloc)"]
    HEAP -->|Provides variable-sized chunks| KERNEL["Kernel Modules & Shell"]
~~~

### 1. PMM (Physical Memory Manager)
Uses a bitmap array (`pmm_bitmap`) to represent the entire physical RAM. Each bit represents a 4KB frame. It parses the Multiboot Header to avoid overwriting reserved hardware zones or the kernel code itself.

### 2. VMM (Virtual Memory Manager)
Enables the `CR0` paging bit and manages the `CR3` register.
* Uses a 32-bit two-level translation: **Page Directory (10 bits) -> Page Table (10 bits) -> Offset (12 bits)**.
* Assigns proper Access Flags (Present, Writable, User/Supervisor) to enforce memory protection.
* Contains TLB flushing logic (`invlpg`) to guarantee consistency.

### 3. Dynamic Allocator (`kmalloc`)
Built from scratch without `libc`. It splits the heap into optimized zones:
* **Tiny & Small Zones:** For objects under 1024 bytes. Uses block-splitting and coalescing.
* **Large Zone:** For objects > 1024 bytes, directly mapped to new pages.
* Includes `krealloc` and `kfree` with memory defragmentation (coalescing neighbor blocks).

---

## 📂 Repository Structure (KFS-3 additions)

~~~text
.
├── src/
│   ├── mm/
│   │   ├── pmm.c            # Physical bitmap allocator
│   │   ├── vmm.c            # Paging and Page Table management
│   │   ├── paging_asm.s     # Assembly to load CR3 and enable CR0 paging
│   │   ├── slab_malloc.c    # kmalloc implementation
│   │   └── slab_free.c      # kfree implementation with block coalescing
│   └── kernel/
│       └── panic.c          # System halt and error tracing
└── ...
~~~

---

## 🚀 Usage & Debugging Commands

Compile and run the kernel in QEMU:

~~~bash
make run
~~~

### Memory Inspection Commands (Shell)
This version includes special tools to debug the memory state in real-time:

| Command | Description |
| :--- | :--- |
| **`meminfo`** | Prints total, used, and free RAM in KB and pages. |
| **`malloc_test`** | Executes an allocation routine testing `kmalloc` / `kfree` across Tiny, Small, and Large boundaries. |
| **`virt2phys`** | Translates a virtual address into its exact physical hardware address by walking the Page Tables. |
| **`stack`** | Performs a live hex dump of the current kernel stack pointer. |

---
*Developed by Eleder Andres. KFS-3 Release.*
