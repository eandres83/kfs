#layout asm
layout regs

target remote localhost:1234
break sys_open
ignore 1 2
continue

