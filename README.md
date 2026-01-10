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
| **KFS-2** | **Interrupts** | 🚧 In Progress | GDT, IDT, ISRs, PIC Remapping, Keyboard Interrupts. |
| **KFS-3** | **Memory** | ⏳ Pending | Virtual Memory, Paging, Heap (kmalloc). |
| **KFS-4** | **Processes** | ⏳ Pending | Multitasking, Scheduler, User Space, Signals. |

---

### 📂 Directory Structure
~~~text
.
├── src/
│   ├── boot/          # Assembly entry points and Multiboot headers
│   ├── kernel/        # Core kernel logic (kmain, interrupts, panic)
│   ├── drivers/       # Hardware drivers (VGA, Keyboard, Serial)
│   └── lib/           # Custom standard library (kprintf, strings, memory)
├── include/           # System-wide header files
├── linker.ld          # Memory layout definition (1MB load address)
└── Makefile           # Build automation and QEMU integration
~~~

---

## 🚀 Current Capabilities (Main Branch)

Running the latest build allows you to:
* **Boot via GRUB:** Compliant with Multiboot specifications.
* **Video Output:** Custom `kprintf` implementation writing to VGA `0xB8000`.
* **Input:** PS/2 Keyboard driver handling Scancode Set 1.
* **Library:** A minimalist C library implementation (`kmemset`, `kmemcpy`, `kstrlen`) running without standard system headers.

---

*Author: Eleder Andres. "Where there is a shell, there is a way."*
