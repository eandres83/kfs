# KFS - Kernel From Scratch

![Arch](https://img.shields.io/badge/arch-x86-lightgrey)
![Kernel](https://img.shields.io/badge/kernel-Monolithic-red)
![Language](https://img.shields.io/badge/language-C%20%2F%20Assembly-blue)
![Status](https://img.shields.io/badge/status-In%20Development-yellow)

<br />
<p align="center">
  <h3 align="center">Writing a 32-bit Unix-like Operating System from scratch</h3>
</p>

## 🗣️ About The Project

**KFS** is a comprehensive systems engineering project focused on building a fully functional Operating System kernel starting from bare metal. Unlike standard application development, this project requires managing every bit of hardware, memory, and CPU execution state manually.

The goal is to progress from a simple bootloader to a multitasking system capable of running user-space shell programs, following the evolution of the **x86 architecture**.

---

## 🗺️ Project Roadmap & Modules

Development is divided into strict milestones (branches). This `main` branch contains the latest development snapshot.

| Module | Focus | Status | Key Engineering Concepts |
| :--- | :--- | :--- | :--- |
| **[KFS-2](https://github.com/eandres83/kfs/tree/kfs-2)** | **GDT & Shell** | ✅ Completed | Memory Segmentation (GDT), Flat Model, Interactive Shell. |
| **[KFS-3](https://github.com/eandres83/kfs/tree/kfs-3)** | **Memory** | ✅ Completed | Virtual Memory (Paging), PMM, Custom Heap (`kmalloc`), Panics. |
| **[KFS-4](https://github.com/eandres83/kfs/tree/kfs-4)** | **Interrupts** | ✅ Completed | IDT, ISRs, PIC Remapping, Hardware IRQs. |
| **[KFS-5](https://github.com/eandres83/kfs/tree/kfs-5)** | **Processes** | ✅ Completed | Multitasking, Context Switch, Scheduler, TCB, Syscalls. |
| **KFS-6** | **User Space** | 🚧 In Progress | Ring 3 Execution, VFS (Virtual File System), ELF Loader. |

---

### 📂 Directory Structure
~~~text
.
├── src/
│   ├── arch/i386/     # Architecture-specific (IDT, PIC, Timer, Syscalls)
│   ├── boot/          # Assembly entry points and Multiboot headers
│   ├── kernel/        # Core kernel logic (kmain, shell, panic)
│   ├── mm/            # Memory Management (PMM, VMM, kmalloc/slab)
│   ├── task/          # Process Management (Scheduler, Context Switch)
│   ├── drivers/       # Hardware drivers (VGA, Keyboard, I/O)
│   └── lib/           # Custom standard library (kprintf, strings)
├── include/           # System-wide header files
├── linker.ld          # Memory layout definition (Higher Half Kernel)
└── Makefile           # Build automation and QEMU integration
~~~

---

## 🚀 Current Capabilities (Main Branch)

Running the latest build allows you to:
* **Preemptive Multitasking:** A custom Round-Robin scheduler driven by the PIT (IRQ0). Can concurrently run, pause, and schedule multiple processes.
* **Interrupt-Driven Architecture:** Complete x86 **IDT** implementation. Handles CPU exceptions, hardware interrupts, and software interrupts (`int 0x80`).
* **Memory Management:** Full physical and virtual memory managers (x86 Paging) supporting isolation, custom block-based Heap Allocator (`kmalloc`), and memory mapping (`mmap`).
* **Interactive Shell:** A CLI environment supporting advanced commands (`malloc_test`, `virt2phys`, `meminfo`, `stack`, `layout`).
* **Boot via GRUB:** Compliant with Multiboot specifications and initialized as a Higher-Half Kernel.

---

## 🛠️ Installation & Usage

### ⚠️ Critical Requirement: Cross-Compiler
You **cannot** compile this kernel with your system's standard GCC. You must use a cross-compiler targeting `i686-elf` to avoid linking against host OS libraries.

* **Compiler:** `i686-elf-gcc`
* **ASM:** `i686-elf-as`

### Build Instructions

1.  **Clone the repository:**
    ~~~bash
    git clone https://github.com/eandres83/kfs.git
    cd kfs
    ~~~

2.  **Compile the Kernel:**
    ~~~bash
    make
    ~~~

3.  **Run the OS (QEMU):**
    ~~~bash
    make run
    ~~~

4.  **Debug (GDB Connection):**
    ~~~bash
    make debug
    ~~~

---
*Author: Eleder Andres. "Where there is a shell, there is a way."*
