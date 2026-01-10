# KFS-1 - Kernel Boot & I/O

![Arch](https://img.shields.io/badge/arch-x86-lightgrey)
![Boot](https://img.shields.io/badge/boot-Multiboot%20%2F%20GRUB-orange)
![Language](https://img.shields.io/badge/language-Assembly%20%26%20C-blue)
![Features](https://img.shields.io/badge/features-VGA%20%7C%20Keyboard%20%7C%20kprintf-brightgreen)

<br />
<p align="center">
  <h3 align="center">Phase 1: Booting the Metal and talking to the Hardware</h3>
</p>

## 🗣️ About The Project

**KFS-1** is the first milestone of the Kernel From Scratch project. The goal is to build a minimal, freestanding kernel capable of being loaded by a Multiboot-compliant bootloader (GRUB), establishing a stack, and interacting with the user via keyboard and screen **without any standard library**.

In this version, the kernel operates in **32-bit Protected Mode** (as set by GRUB) and manages Input/Output via direct port manipulation (`inb`/`outb`).

### 🎯 Key Engineering Achievements
- **Multiboot Compliance:** Assembly header (`boot.s`) with the magic fields required for GRUB recognition.
- **Stack Setup:** Manual initialization of the Stack Pointer (`esp`) to allow C function calls.
- **Custom LibC:** Implementation from scratch of `kprintf`, `kmemset`, `kmemcpy`, and string manipulation functions.
- **VGA Driver:** Complete text-mode driver with support for **colors**, **scrolling**, and hardware **cursor** movement.
- **Keyboard Polling:** PS/2 Keyboard driver that reads scancodes and translates them to ASCII (QWERTY) using a state machine for Shift/Caps.

---

## 📂 Repository Structure (KFS-1)

This version follows a modular structure separating architecture-specific code, kernel logic, and drivers:

~~~text
.
├── src/
│   ├── boot/
│   │   └── boot.s         # Entry point, Multiboot Header & Stack
│   ├── kernel/
│   │   └── kmain.c        # Main C entry point
│   ├── drivers/
│   │   ├── vga.c          # Video memory (0xB8000) manipulation
│   │   ├── keyboard.c     # PS/2 Controller (0x60) Polling logic
│   │   └── io.h           # Port I/O wrappers (inb, outb)
│   └── lib/               # Custom C Library Implementation
│       ├── kprintf.c      # Variadic function for formatted output
│       ├── kmemcpy.c      # Memory movement
│       └── kstrlen.c      # String utilities
├── include/               # Header files (.h)
├── linker.ld              # Linker script (Entry: start, Base: 1MB)
└── Makefile               # Build automation
~~~

---

## 🛠️ Technical Details

### 1. The Entry Point (`boot.s`)
The assembly file defines the **Multiboot Header** within the first 8KB of the binary. It reserves 16KB for the kernel stack (`stack_top`) and passes control to `kmain`.

### 2. VGA Text Mode Driver
Mapped at physical address `0xB8000`.
* **Scrolling:** When the terminal is full, `kmemcpy` shifts the entire buffer up by one line (160 bytes per row).
* **Cursor:** The hardware cursor is updated by writing to the VGA Command ports `0x3D4` and `0x3D5`.
* **kprintf:** A custom implementation handling `%s`, `%d`, `%x`, and `%c` to format output directly to the video buffer.

### 3. PS/2 Keyboard (Polling)
Since Interrupts (IDT) are not yet configured in this phase, the keyboard is handled via **Polling**.
* The kernel continuously checks the Status Register of the PS/2 controller (`0x64`).
* When the "Output Buffer Full" bit is set, it reads the Scancode from Data Port `0x60`.
* A lookup table maps scancodes to ASCII characters.

---

## 🚀 Build & Run

### Requirements
* `gcc` (i386-elf cross-compiler recommended) or `clang`
* `nasm`
* `qemu-system-i386`
* `make`

### Commands

1.  **Compile the Kernel:**
    ~~~bash
    make
    ~~~
    *Generates `kernel.bin`.*

2.  **Run in QEMU:**
    ~~~bash
    make run
    ~~~
    *Boots the kernel directly using QEMU's `-kernel` flag.*

3.  **Debug Mode (GDB):**
    ~~~bash
    make debug
    ~~~
    *Starts QEMU in suspended mode (`-S -s`) waiting for GDB connection.*

---
*Developed by Eleder Andres. KFS-1 Release.*
