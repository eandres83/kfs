#layout asm
#layout regs

target remote localhost:1234
break command.c:35
continue

