#layout asm
layout regs

target remote localhost:1234
break create_init_process
#ignore 1 2
continue

