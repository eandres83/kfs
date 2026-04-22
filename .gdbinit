layout asm
layout regs

target remote localhost:1234
break ide_read_sector
continue

