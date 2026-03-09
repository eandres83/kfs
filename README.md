# KFS-4 - Interrupts & IDT

![Arch](https://img.shields.io/badge/arch-x86-lightgrey)
![Core](https://img.shields.io/badge/core-IDT%20%2F%20PIC-red)
![Drivers](https://img.shields.io/badge/drivers-Interrupt--Driven-blue)
![Status](https://img.shields.io/badge/milestone-completed-success)

<br />
<p align="center">
  <h3 align="center">Phase 4: Interrupts, Signals and Fun</h3>
</p>

## 🗣️ About The Project

**KFS-4** completely changes how the kernel interacts with the hardware. Previously, drivers like the keyboard had to be continuously checked via *polling*. Now, the kernel uses an **Interrupt-Driven Architecture**.

By implementing the **Interrupt Descriptor Table (IDT)**, the CPU can now safely halt and wait for asynchronous hardware signals, handle division-by-zero errors gracefully, and establish the gateway for User-Space applications via Software Interrupts (System Calls).

### 🎯 Key Engineering Achievements
- **Interrupt Descriptor Table (IDT):** Created and loaded the 256-entry table via the `lidt` assembly instruction.
- **CPU Exceptions (ISRs):** Registered the first 32 Interrupt Service Routines to catch hardware faults (Page Fault, Double Fault, General Protection Fault).
- **8259 PIC Remapping:** Reprogrammed the Programmable Interrupt Controller (Master/Slave) to map IRQs to interrupt vectors `0x20` to `0x2F`, preventing conflicts with CPU exceptions.
- **Hardware IRQs:** - **IRQ0 (PIT):** Configured the Programmable Interval Timer as the system clock.
  - **IRQ1 (Keyboard):** Replaced polling with a true interrupt-driven PS/2 handler.
- **Panic & Stack Traces:** Advanced error handling that cleans registers, dumps the execution state, and safely halts the CPU.
- **Bonuses Achieved:**
  - **Multi-Layout Keyboard:** Added dynamic support for both `US (QWERTY)` and `FR (AZERTY)` layouts.
  - **Syscall Gateway:** Configured `INT 0x80` (ISR 128) as the standard software interrupt for future system calls (`sys_read`, `sys_write`).

---

## 🏗️ Interrupt Architecture

~~~mermaid
graph TD
    subgraph Hardware Layer
        KBD["Keyboard (IRQ1)"] --> PIC
        PIT["Timer (IRQ0)"] --> PIC
    end
    
    subgraph Kernel Space
        PIC["8259 PIC"] -->|Vector 0x20 - 0x2F| CPU
        Software["Software (INT 0x80)"] --> CPU
        CPU["x86 CPU"] -->|Checks| IDT["IDT (Interrupt Descriptor Table)"]
        
        IDT -->|0-31| ISRs["CPU Exceptions (Panic)"]
        IDT -->|32-47| IRQs["IRQ Handlers (Drivers)"]
        IDT -->|128| Syscalls["Syscall Dispatcher"]
    end
~~~

---

## 📂 Repository Structure (KFS-4 additions)

~~~text
.
├── src/
│   ├── arch/i386/
│   │   ├── idt.c            # IDT initialization and PIC remapping
│   │   ├── idt_asm.s        # Assembly wrappers for ISRs and IRQs
│   │   ├── timer.c          # PIT (Programmable Interval Timer) logic
│   │   └── syscall.c        # Int 0x80 Syscall dispatcher (Bonus)
│   ├── drivers/
│   │   └── keyboard.c       # Refactored to be interrupt-driven + Multi-layout
│   └── kernel/
│       └── panic.c          # Kernel panic handling and stack dumping
└── ...
~~~

---

## 🚀 Usage & Debugging Commands

Compile and run the kernel in QEMU:

~~~bash
make run
~~~

### New Shell Commands (KFS-4)
This version introduces new commands to test the interrupt subsystem:

| Command | Description |
| :--- | :--- |
| **`interrupt`** | Forces a Software Breakpoint (`INT 3`) to test the IDT exception handler. |
| **`layout us`** | Switches the keyboard map to US QWERTY (Bonus). |
| **`layout fr`** | Switches the keyboard map to FR AZERTY (Bonus). |
| **`test_syscall`** | Triggers `INT 0x80` with `EAX=4` to test the `sys_write` stub (Bonus). |
| **`stack`** | Performs a live hex dump of the kernel stack, crucial for panic analysis. |

---
*Developed by Eleder Andres. KFS-4 Release.*
