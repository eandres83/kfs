# KFS-2 - GDT & Interactive Shell

![Arch](https://img.shields.io/badge/arch-x86-lightgrey)
![Memory](https://img.shields.io/badge/memory-GDT%20%2F%20Segmentation-red)
![Interface](https://img.shields.io/badge/interface-Shell%20CLI-blue)
![Status](https://img.shields.io/badge/milestone-completed-success)

<br />
<p align="center">
  <h3 align="center">Phase 2: Memory Segmentation and User Interaction</h3>
</p>

## 🗣️ About The Project

**KFS-2** marks the transition from a passive bootloader-dependent binary to a kernel that manages its own memory segments. In this milestone, we abandon the GDT provided by GRUB and install our own **Global Descriptor Table (GDT)**.

Additionally, this version introduces the first **Interactive Shell**, allowing the user to execute commands and inspect the system state in real-time.

### 🎯 Key Engineering Achievements
- **Custom GDT:** Implementation of a "Flat Memory Model" defining Kernel Code, Kernel Data, and Stack segments covering the full 4GB address space.
- **Segment Registers:** Manual reloading of CPU segment registers (`CS`, `DS`, `ES`, `FS`, `GS`, `SS`) via Assembly trampoline.
- **Kernel Shell:** A command-line interface (CLI) running in the main kernel loop.
- **String Manipulation:** Added `kstrcmp` and command parsing logic to handle user inputs like `help`, `reboot`, or `halt`.

---

## 🛠️ Technical Implementation

### 1. Global Descriptor Table (GDT)
The x86 architecture in Protected Mode requires a GDT to define memory permissions.
* **Source:** `src/kernel/gdt.c` & `src/kernel/gdt_asm.s`
* **Logic:** We define 3 primary entries (plus the Null Descriptor):
    1.  **Null Descriptor:** 0x00 (Required by CPU).
    2.  **Kernel Code:** Base `0x0`, Limit `0xFFFFFFFF`, Access Byte `0x9A` (Exec/Read, Ring 0).
    3.  **Kernel Data:** Base `0x0`, Limit `0xFFFFFFFF`, Access Byte `0x92` (Read/Write, Ring 0).
* **Assembly Trampoline:** After loading the GDT pointer with `lgdt`, we must perform a "Far Jump" (`jmp 0x08:flush`) to reload the **Code Segment (CS)** register and update all data segment registers to point to our new Data Segment (`0x10`).

### 2. The Kernel Shell
Located in `src/kernel/shell.c`, the shell creates a feedback loop:
1.  **Prompt:** Prints `KFS > ` to the screen.
2.  **Input:** Waits for the keyboard driver to fill a buffer until `ENTER` is pressed.
3.  **Parse:** Uses `kstrcmp` to match the buffer against known commands.
4.  **Execute:** Triggers functions like rebooting (via keyboard controller pulse) or clearing the screen.

---

## 📂 Repository Structure (KFS-2)

New files added for segmentation and shell logic:

~~~text
.
├── src/
│   ├── kernel/
│   │   ├── kmain.c        # Initializes GDT and starts Shell
│   │   ├── gdt.c          # GDT structure definition and setup
│   │   ├── gdt_asm.s      # Assembly wrapper for lgdt and long jump
│   │   └── shell.c        # Command Line Interface logic
│   ├── drivers/
│   │   ├── keyboard.c     # Handles typing and buffer filling
│   │   └── ...
│   └── lib/
│       ├── kstrcmp.c      # Added for command comparison
│       └── ...
├── include/
│   └── utils.h
└── ...
~~~

---

## 🚀 Usage & Commands

Compile and run the kernel in QEMU:

### Prerequisites
1.  **Build the Cross-Compiler:** Ensure `i686-elf-gcc` is in your `$PATH`.
2.  **Tools:** `make`, `nasm`, `qemu-system-i386`, `xorriso`.

~~~bash
make run
~~~
*The Makefile is configured to automatically look for `i686-elf-gcc`.*

### Available Shell Commands
Once the system boots, you can type:

| Command | Description |
| :--- | :--- |
| **stack** | Displays the stack. |
| **reboot** | Restarts the machine (sends command `0xFE` to PS/2 port `0x64`). |
| **halt** | Stops the CPU execution (`hlt` instruction). |
| **clear** | Clears the VGA screen. |

---
*Developed by Eleder Andres. KFS-2 Release.*
