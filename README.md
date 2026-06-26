# KFS - Kernel From Scratch

Monolithic 32-bit x86 Operating System kernel built from bare metal. This repository details the step-by-step engineering process of constructing a Unix-like operating system environment, enforcing strict hardware isolation, memory abstraction, and preemptive process multitasking.

## Architectural Overview

KFS implements a classic monolithic architecture where core system infrastructure—including memory management, interrupt handling, persistent filesystems, and process scheduling—runs inside the highly privileged CPU Ring 0 execution state. The system guarantees stability and security by executing applications within an unprivileged Ring 3 state, sanitizing all boundary transitions via a single software interrupt vector gateway.

The system features a hybrid approach through a modular kernel architecture, allowing core functionalities to be expanded dynamically without expanding the permanent boot-time footprint. 

**Execution Hierarchy:**
* **User Space (Ring 3):** User Applications and custom static LibC.
* **The Boundary:** Syscall Gateway (`INT 0x80`) equipped with real-time `uaccess` memory verification.
* **Kernel Space (Ring 0):** Kernel Core, Virtual Memory Manager, Preemptive Scheduler, and the Ext2/VFS Storage Subsystem.
* **Modular Extensions (Ring 0):** Dynamically loaded kernel modules managed via an internal API.

## Project Roadmap

The milestones within this project track the development sequence necessary to move from bare hardware control to a multi-user, interactive system.

| Milestone | Target Domain | Status | Core Architectural Focus |
| :--- | :--- | :--- | :--- |
| **[KFS-1](https://github.com/eandres83/kfs/tree/kfs-1)** | Boot & Primitive I/O | Completed | Multiboot compliance, stack establishment, low-level VGA buffer. |
| **[KFS-2](https://github.com/eandres83/kfs/tree/kfs-2)** | CPU Segmentation | Completed | Global Descriptor Table (GDT), kernel segment registers. |
| **[KFS-3](https://github.com/eandres83/kfs/tree/kfs-3)** | Memory Architecture | Completed | Physical Abstraction (PMM), Virtual Paging (VMM), Zone Heap. |
| **[KFS-4](https://github.com/eandres83/kfs/tree/kfs-4)** | Asynchronous I/O | Completed | Interrupt Descriptor Table (IDT), 8259 PIC remapping, IRQs. |
| **[KFS-5](https://github.com/eandres83/kfs/tree/kfs-5)** | Process Subsystem | Completed | Task Control Blocks (TCB), hardware state virtualization, Scheduler. |
| **[KFS-6](https://github.com/eandres83/kfs/tree/kfs-6)** | Non-Volatile Storage | Completed | Virtual File System (VFS), Ext2 specification, IDE/PATA driver. |
| **[KFS-7](https://github.com/eandres83/kfs/tree/kfs-7)** | User Space & Security | Completed | Ring 3 transition, uaccess memory fault recovery. |
| **[KFS-8](https://github.com/eandres83/kfs/tree/kfs-8)** | Modular Kernel | Completed | Internal module API, creation and destruction of modules, and communication callbacks. |
| **[KFS-9](https://github.com/eandres83/kfs/tree/kfs-9)** | ELF Execution | Completed | ELF file parser and loader, `execve` syscall implementation, and runtime ELF module linking. |

## Current System Capabilities

* **Dynamic Modular Architecture:** Includes an internal API to register, create, and destroy kernel modules. Modules can be loaded, listed, and unloaded at runtime directly as ELF files.
* **Event-Driven Kernel Callbacks:** Configurable communication pathways allowing the kernel to trigger module functions, such as notifying a keyboard module upon key presses or requesting data from a time module. This infrastructure is supported by custom technical wrappers for system calls and kernel functions, allowing seamless utilization of specific hardware interrupts.
* **ELF Execution Engine:** A complete interface capable of reading, parsing, and storing ELF binary files. This integrates directly with an `execve`-like syscall that automatically associates the loaded ELF file with a newly created process.
* **Hardware Acknowledged Memory Safety (`uaccess`):** Real-time isolation verification during kernel/user boundary copies (`copy_to_user` / `copy_from_user`) using specialized Page Fault interception stubs.
* **Virtual File System & Ext2 Driver:** Multi-partition Master Boot Record (MBR) layout processing, dynamic mounting support, and complete asynchronous read/write interfaces over native Ext2 structures.
* **Preemptive Concurrency Control:** Continuous scheduler ticks managed via the Programmable Interval Timer (PIT IRQ0), tracking task states and saving thread registers into distinct kernel-level stacks.

## Compilation and Deployment

### Cross-Compilation Toolchain
To build the monolithic image without relying on host distribution runtime libraries, an independent cross-compilation environment targeting bare-metal execution is required.

* **Target Engine:** `i686-elf-gcc` / `i686-elf-ld`
* **Supported Assembler:** `nasm`

### Execution Commands

Compile the system image and userland binaries:
~~~bash
make
~~~

Launch the kernel image within a virtualized platform using a hardware-isolated disk image:
~~~bash
make run && make modules
~~~

Initiate source-level remote debugging sessions via GDB loops:
~~~bash
make debug
~~~

---
*Systems Engineering Repository maintained by Eleder Andres.*
