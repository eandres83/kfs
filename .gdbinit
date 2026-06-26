#layout asm
layout regs

target remote localhost:1234
break listmod
#ignore 1 2
continue

