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
| **[KFS-1](https://github.com/eandres83/kfs/tree/kfs-1)** | **Boot & I/O** | ✅ Completed | Multiboot, Stack Setup, VGA Driver, Polling I/O. |
| **[KFS-2](https://github.com/eandres83/kfs/tree/kfs-2)** | **GDT & Shell** | ✅ Completed | Memory Segmentation (GDT), Flat Model, Interactive Shell. |
| **KFS-3** | **Interrupts** | 🚧 In Progress | IDT, ISRs, PIC Remapping, Keyboard IRQs. |
| **KFS-4** | **Memory** | ⏳ Pending | Virtual Memory, Paging, Heap (kmalloc). |
| **KFS-5** | **Processes** | ⏳ Pending | Multitasking, Scheduler, User Space. |

---

### 📂 Directory Structure
~~~text
.
├── src/
│   ├── boot/          # Assembly entry points and Multiboot headers
│   ├── kernel/        # Core kernel logic (GDT, Shell, Main)
│   ├── drivers/       # Hardware drivers (VGA, Keyboard, Serial)
│   └── lib/           # Custom standard library (kprintf, strings, memory)
├── include/           # System-wide header files
├── linker.ld          # Memory layout definition (1MB load address)
└── Makefile           # Build automation and QEMU integration
~~~

---

## 🚀 Current Capabilities (Main Branch)

Running the latest build allows you to:
* **Interactive Shell:** A CLI environment supporting commands like `help`, `reboot`, `halt`, and `clear`.
* **Memory Segmentation:** Custom **GDT** implementation enforcing a Flat Memory Model (Code/Data/Stack segments).
* **Boot via GRUB:** Compliant with Multiboot specifications.
* **Video Output:** Custom `kprintf` implementation writing to VGA `0xB8000`.
* **Input:** PS/2 Keyboard driver handling Scancode Set 1.
* **Library:** A minimalist C library implementation (`kmemset`, `kmemcpy`, `kstrcmp`) running without standard system headers.

---

## 🛠️ Installation & Usage

### ⚠️ Critical Requirement: Cross-Compiler
You **cannot** compile this kernel with your system's standard GCC. You must use a cross-compiler targeting `i686-elf` to avoid linking against host OS libraries.

* **Compiler:** `i686-elf-gcc`
* **Linker:** `i686-elf-ld`

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
