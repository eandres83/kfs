# KFS-5 - Process Management & Multitasking

![Arch](https://img.shields.io/badge/arch-x86-lightgrey)
![Core](https://img.shields.io/badge/core-Scheduler%20%2F%20Multitasking-red)
![Memory](https://img.shields.io/badge/memory-mmap%20%2F%20Isolation-blue)
![Status](https://img.shields.io/badge/milestone-completed-success)

<br />
<p align="center">
  <h3 align="center">Phase 5: The Illusion of Concurrency</h3>
</p>

## 🗣️ About The Project

**KFS-5** transforms the kernel from a single-threaded execution model into a preemptive multitasking environment. By introducing a **Task Scheduler**, the kernel can now execute multiple processes seemingly simultaneously by rapidly sharing CPU time slices.

This phase implements the complex mechanisms required to pause a running process, save its exact CPU state (Registers, Stack, and Page Directory), and restore another process in a fraction of a millisecond.

### 🎯 Key Engineering Achievements
- **Task Control Block (TCB):** Designed the data structure representing a process, keeping track of its state (RUNNING, SLEEPING, DEAD), execution context, and memory pointers.
- **Context Switching:** Implemented highly critical assembly routines (`task_asm.s`) to swap the kernel stack (`esp`), base pointer (`ebp`), and instruction pointer (`eip`) between tasks.
- **Round-Robin Scheduler:** Hooked into the PIT (Programmable Interval Timer) on IRQ0 to forcefully preempt the running task and schedule the next one in the queue.
- **Bonuses Achieved:**
  - **Memory Mapping (`mmap`):** Implemented an interface allowing processes to dynamically acquire virtual memory securely mapped to their isolated Page Directory (`cr3`).
  - **Syscall Integration:** Linked the IDT (`INT 0x80`) to the process execution flow to handle future system calls dynamically based on the calling process.
  - **BSS/Data Sections:** Configured the process structure to properly allocate and zero out memory for its BSS and Data sectors dynamically.

---

## 🏗️ Multitasking Architecture

~~~mermaid
graph TD
    subgraph Hardware Layer
        PIT["PIT (Timer IRQ0)"] -->|Fires every tick| CPU
    end
    
    subgraph Kernel Scheduler
        CPU -->|Triggers| ISR["Timer Callback"]
        ISR --> SCHED["Scheduler (task.c)"]
        SCHED -->|Saves State| ASM["Context Switch (task_asm.s)"]
        
        ASM -->|Push Registers| TCB1["Task A (Paused)"]
        ASM -->|Pop Registers| TCB2["Task B (Running)"]
        
        TCB2 -->|Load CR3| VMM["Virtual Memory Space B"]
    end
~~~

### The Anatomy of a Context Switch
1. **Interrupt Occurs:** The PIT fires an IRQ0. The CPU automatically pushes `EFLAGS`, `CS`, and `EIP` to the current task's stack.
2. **State Preservation:** Our assembly wrapper pushes all General Purpose Registers (`eax`, `ebx`, etc.) to the stack.
3. **Scheduler Logic:** The C-level scheduler updates the current process state and selects the next TCB from the execution queue.
4. **Stack & Memory Swap:** The assembly routine swaps the `ESP` to point to the new task's stack and updates the `CR3` register to load the new task's Page Directory.
5. **Resume Execution:** The routine pops the registers of the new task and executes an `iret`, returning execution precisely where the new task was previously paused.

---

## 📂 Repository Structure (KFS-5 additions)

~~~text
.
├── src/
│   ├── task/
│   │   ├── task.c           # TCB management, queue logic, and Scheduler
│   │   ├── task.h           # Process structures and states
│   │   ├── task_asm.s       # Low-level context switching (Stack/CR3 swap)
│   │   └── test_task.c      # Proof-of-concept functions running concurrently
│   ├── arch/i386/
│   │   └── timer.c          # Modified to trigger the scheduler on ticks
│   └── mm/
│       └── vmm.c            # Upgraded to support `mmap` and process spaces
└── ...
~~~

---

## 🚀 Usage & Testing

Compile and run the kernel in QEMU:

~~~bash
make run
~~~

### Testing Concurrency
Upon boot, the kernel will load the shell alongside background test tasks (as defined in `test_task.c`). You will observe background processes outputting to the console while the interactive shell remains fully responsive, proving that preemptive multitasking is functional.

---
*Developed by Eleder Andres. KFS-5 Release.*
