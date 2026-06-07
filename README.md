# KFS-7 - User Space, ELF Loading & Memory Abstraction Safety

This development module establishes complete isolation between application logic and kernel space. By separating CPU execution levels into Ring 0 (Supervisor) and Ring 3 (User), the system ensures that user-space programs cannot directly manipulate kernel memory structures or physical hardware interfaces.

## The Core Problem: The Ring 0 / Ring 3 Memory Boundary

When a user program invokes a system call (e.g., sys_read or sys_write), it passes pointers referencing memory locations inside its own virtual address space. Because the kernel runs in Ring 0, it has the technical capability to read and write anywhere within the mapped memory pages.

However, accepting arbitrary user-space pointers introduces critical security flaws:
1. The Poisoned Pointer Vulnerability: A malicious or erroneous user space application could pass a pointer pointing directly into kernel memory space (e.g., above 0xC0000000). If the kernel dereferences this pointer directly without verification, it could overwrite its own structures, leading to a system crash or privilege escalation.
2. Unmapped Address Panics: If the user application passes a valid user-space address that happens to be currently unmapped or swapped out, the CPU hardware MMU triggers a Page Fault (#PF) immediately upon dereference. If this occurs inside kernel space execution flow without specific safety hooks, the kernel enters an unrecoverable Panic state.

## The Architectural Solution: Safe uaccess via Page Fault Interception

To maximize efficiency and avoid checking every single byte limit manually via software loops prior to copying, KFS-7 utilizes hardware-assisted software protection routines modeled after enterprise-grade monolithic production kernels.

Instead of pre-validating addresses through expensive software verifications, the kernel attempts the copy directly but intercepts any hardware exceptions if the pointer proves to be invalid or forbidden.

The Verification Flow Sequence:
1. System Call Invocation: A user application triggers INT 0x80, passing a data memory pointer.
2. Assembly Trampoline: The kernel initiates a direct memory copy using specialized low-level assembly functions (copy_to_user_asm.s).
3. Hardware MMU Intervention: If the pointer points to an invalid, unmapped, or supervisor-only page, the CPU instantly generates a Page Fault Exception (#PF).
4. IDT Interception: The Interrupt Descriptor Table (Vector 14) catches the fault and extracts the saved Instruction Pointer (EIP) from the stack.
5. Exception Cache Range Check: The handler cross-references this faulting EIP against the exact instruction boundaries reserved for our memory copy routines.
6. Graceful Error Injection: Since a match is confirmed, the handler rewrites the saved EIP context on the stack to point directly to a safe recovery stub. The execution resumes at this stub, forcing the operation to exit cleanly and return an internal error code (-EFAULT) instead of crashing the operating system.

## Implemented Infrastructure

### 1. ELF Binary Parser & Loader
The execution framework parses standard binary files using the native 32-bit ELF specification:
* Validates the ELF magic identity sequence (0x7F 'E' 'L' 'F').
* Loops through the compiled Program Headers to identify loadable segments (PT_LOAD).
* Maps code, data, and uninitialized BSS space allocations directly into isolated process page tables using virtual memory flags matching requested access configurations (Executable, Writable, User).

### 2. User Space LibC (userland/)
To allow programs to run natively in Ring 3, an internal, isolated userland static runtime environment was constructed from scratch:
* System Call Wrappers: Custom assembly functions that wrap direct hardware context preparation, moving arguments to registers before calling the INT 0x80 hardware gateway.
* Independent User Heap Allocator: A freestanding user-space heap implementation completely decoupled from the kernel's allocator, allowing sandboxed data mutations.

### 3. Verification Suite & Userland Shell
The filesystem contains a suite of test binaries executed from a dedicated user-space application loop (init and minishell):
* test_memory_security: Intentional validation binary that attempts to read from protected kernel regions or write to unmapped pages from Ring 3. The system demonstrates resilience by terminating the offending process while keeping the root kernel shell alive.
* test_pipe & test_socket: Inter-Process Communication validation testing data replication safety using memory structures mapped across scheduling operations.
* test_permissions: Verifies access right checks based on initialized structures.

---
*Developed by Eleder Andres.*
