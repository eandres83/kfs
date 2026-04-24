#layout asm
#layout regs

target remote localhost:1234
break ext2_finddir
continue

