#layout asm
layout regs

target remote localhost:1234
break insmod
ignore 1 2
continue

