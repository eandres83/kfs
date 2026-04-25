#layout asm
#layout regs

target remote localhost:1234
break get_vfs_node_path
continue

