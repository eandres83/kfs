# KFS - Kernel From Scratch

![Arch](https://img.shields.io/badge/arch-x86-lightgrey)
![Kernel](https://img.shields.io/badge/kernel-Monolithic-red)
![Language](https://img.shields.io/badge/language-C%20%2F%20Assembly-blue)
![Status](https://img.shields.io/badge/status-In%20Development-yellow)

## Overview

KFS is a comprehensive systems engineering project focused on building a fully functional Operating System kernel starting from bare metal. This project requires managing hardware, memory, and CPU execution state without reliance on external standard libraries.

The primary objective is the progressive development from a simple bootloader to a multitasking system capable of executing user-space shell programs, adhering to the specifications of the x86 architecture. The system currently supports dynamic runtime expansions through a modular architecture.

---

## Project Roadmap

Development is structured into specific milestones. The current main branch reflects the implementation of a modular kernel architecture.

| Module | Focus | Status | Key Engineering Concepts |
| :--- | :--- | :--- | :--- |
| **[KFS-3](https://github.com/eandres83/kfs/tree/kfs-3)** | **Memory** | Completed | Virtual Memory (Paging), PMM, Custom Heap (`kmalloc`), Panics. |
| **[KFS-4](https://github.com/eandres83/kfs/tree/kfs-4)** | **Interrupts** | Completed | IDT, ISRs, PIC Remapping, Hardware IRQs. |
| **[KFS-5](https://github.com/eandres83/kfs/tree/kfs-5)** | **Processes** | Completed | Multitasking, Context Switch, Scheduler, TCB, Syscalls. |
| **[KFS-6](https://github.com/eandres83/kfs/tree/kfs-6)** | **Filesystem** | Completed | VFS, Ext2 Driver, IDE/PATA Controller, MBR Partitions. |
| **[KFS-7](https://github.com/eandres83/kfs/tree/kfs-7)** | **User Space** | Completed | Ring 3 Execution, ELF Loader, uaccess isolation, Advanced Syscalls. |
| **[KFS-8](https://github.com/eandres83/kfs/tree/kfs-8)** | **Modular Kernel** | Completed | Internal module API, dynamic loading, module creation/destruction, kernel callbacks. |

---

## Directory Structure
~~~text
.
├── bin/               # User space applications (minishell, cat, ls, testing binaries)
├── include/           # System-wide header files
├── modules/           # Dynamically loadable kernel modules (keyboard, timer, process, memory)
├── src/
│   ├── arch/i386/     # Architecture-specific (IDT, PIC, Timer, Syscall Gateway)
│   ├── boot/          # Assembly entry points and Multiboot headers
│   ├── drivers/       # Hardware drivers (VGA, Keyboard, IDE/Storage, TTY)
│   ├── fs/            # VFS abstraction, Ext2 parsing, MBR, and Mount points
│   ├── kernel/        # Core kernel initialization (kmain) and panic states
│   ├── mm/            # Memory Management (PMM, VMM, kmalloc/slab allocators)
│   ├── modules/       # Core module loader logic, symbols, and event tracking API
│   └── task/          # Process Management (Scheduler, Context Switch, ELF loader)
├── userland/          # Custom static LibC and malloc implementation for Ring 3
├── linker.ld          # Memory layout definition (Higher Half Kernel)
└── Makefile           # Build automation and QEMU integration
~~~

---

## System Capabilities

* **Modular Kernel Interface:** Dynamically load and unload independent modules at runtime via an internal API. Includes event-driven callbacks for specific kernel events (e.g., Keyboard and Timer modules).
* **Persistent Storage:** IDE (PATA) driver interacting with a Virtual File System (VFS) and parsing Ext2 formatted disks.
* **Preemptive Multitasking:** Round-Robin scheduler driven by the PIT (IRQ0). Manages concurrent execution, pausing, and scheduling of multiple processes.
* **Interrupt-Driven Architecture:** x86 IDT implementation handling CPU exceptions, hardware interrupts, and software interrupts (`int 0x80`).
* **Memory Management:** Physical and virtual memory managers (x86 Paging) supporting isolation, block-based Heap Allocator (`kmalloc`), and memory mapping (`mmap`).
* **User-Space Execution:** ELF binary parsing, Ring 3 isolation, and standard system call interfaces.

---

## Build and Deployment Requirements

### Cross-Compiler
Compilation with standard host GCC is unsupported. An `i686-elf` cross-compiler is required to prevent linkage against host OS libraries.

* **Compiler:** `i686-elf-gcc`
* **Assembler:** `i686-elf-as` (or `nasm`)

### Build Instructions

1.  **Clone the repository:**
    ~~~bash
    git clone https://github.com/eandres83/kfs.git
    cd kfs
    ~~~

2.  **Compile the Kernel and Userland:**
    ~~~bash
    make
    ~~~

3.  **Execute the Virtual Environment (QEMU):**
    ~~~bash
    make run
    ~~~
