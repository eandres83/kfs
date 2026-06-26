# KFS - Testing and Modification Guide

This document outlines the standard procedures for modifying user-space initialization and testing system components within the KFS environment.

## 1. Modifying Initialization (`bin/init.c`)

The `init` process is the first user-space program executed by the kernel after entering Ring 3. Its primary responsibility is to spawn the main user environment, typically a shell.

To configure the kernel to launch the `minishell` upon boot, you must modify the execution target within `bin/init.c`.

### Procedure:
1. Locate `bin/init.c` in the source tree.
2. Identify the system call responsible for spawning the child process (typically an `execve` or equivalent custom syscall).
3. Update the path argument passed to the execution function to point directly to the minishell binary:
   * **Target Path:** `"/bin/minishell"`
4. Ensure that the `minishell` binary is being correctly built and placed into the initial ramdisk or root filesystem by the Makefile.

## 2. Testing Kernel Modules

The KFS-8 architecture introduces dynamically loadable modules. Testing these components requires validating both their registration and callback mechanisms.

### Testing the Keyboard Module
* Ensure the module is loaded during initialization.
* Press physical keys to verify the kernel callback function triggers the module's handler accurately.

### Testing the Timer Module
* Request the current system time from user-space using the appropriate syscall or ioctl mapped to the timer module.
* Verify the returned structure matches the expected PIT ticks.

## 3. Recompilation and Execution

After making any changes to `bin/init.c` or module source files, you must rebuild the filesystem and the kernel image.

~~~bash
# Clean previous object files
make clean

# Recompile the kernel, and user-space binaries
make

# Compile modules
make modules

# Launch QEMU with the updated image
make run
~~~
