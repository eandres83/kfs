layout asm
layout regs

target remote localhost:1234
break proceso_test_syscall
continue

