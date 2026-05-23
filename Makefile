export CC = i686-elf-gcc
AS = i686-elf-as
LD = i686-elf-ld

INCLUDES = -I include -I src
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -Werror -g \
	 -fno-builtin -fno-exceptions -fno-stack-protector \
	 -nostdlib -nodefaultlibs $(INCLUDES)

export USER_CFLAGS = -m32 -ffreestanding -Wall -Wextra -Werror -g -fno-builtin -nostdlib -nodefaultlibs -I userland
LDFLAGS = -T linker.ld

export PROJECT_ROOT := $(CURDIR)

DISK_IMG = disk.img
FS_DIR = rootfs

NAME = kernel.bin

export BUILD_DIR = .obj

SRCS_MINISHELL = $(wildcard bin/minishell/*.c) $(wildcard bin/minishell/builtins/*.c) $(wildcard bin/minishell/execute/*.c)

SRCS_C = $(wildcard src/drivers/*.c) $(wildcard src/drivers/ide/*.c) $(wildcard src/kernel/*.c) \
	$(wildcard src/lib/*.c) $(wildcard src/mm/*.c) $(wildcard src/arch/i386/*.c) $(wildcard src/task/*.c) \
	$(wildcard src/fs/ext2/*.c) $(wildcard src/fs/vfs/*.c) $(wildcard src/fs/*.c) $(wildcard src/drivers/tty/*.c) \
	$(wildcard src/arch/i386/lib/*.c)
SRCS_S = $(wildcard src/boot/*.s) $(wildcard src/mm/*.s) $(wildcard src/arch/i386/*.s) $(wildcard src/task/*.s) 

KERNEL_OBJS = $(patsubst src/%.c, $(BUILD_DIR)/src/%.o, $(SRCS_C)) \
	$(patsubst src/%.s, $(BUILD_DIR)/src/%.o, $(SRCS_S))

USER_SRCS_C = $(wildcard userland/libc/*.c) $(wildcard userland/malloc/*.c) $(wildcard userland/*.c)
export USER_OBJS_C = $(patsubst userland/%.c, $(BUILD_DIR)/userland/%.o, $(USER_SRCS_C))
export CRT0_OBJ = $(BUILD_DIR)/userland/crt0.o

APPS_SRCS = $(wildcard bin/*.c)
APPS_OBJ = $(patsubst bin/%.c, bin/%, $(APPS_SRCS))

all: $(DISK_IMG) $(NAME)

# KERNEL
$(NAME): $(KERNEL_OBJS)
	@echo "Linking kernel..."
	$(CC) $(LDFLAGS) -o $@ -ffreestanding -O2 -nostdlib $^ -lgcc

$(BUILD_DIR)/src/%.o: src/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling C: $<"
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/src/%.o: src/%.s
	@mkdir -p $(dir $@)
	@echo "Compiling ASM: $<"
	$(AS) -o $@ $<

# USERLAND
$(BUILD_DIR)/userland/%.o: userland/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(USER_CFLAGS) -c $< -o $@

$(BUILD_DIR)/userland/crt0.o: userland/crt0.s
	@mkdir -p $(dir $@)
	@$(AS) $< -o $@

bin/%: bin/%.c $(CRT0_OBJ) $(USER_OBJS_C)
	@mkdir -p $(dir $(BUILD_DIR)/apps/$*.o)
	@echo "Compiling user apps: $*"
	@$(CC) $(USER_CFLAGS) -c $< -o $(BUILD_DIR)/apps/$*.o
	@echo "Linking app: $*"
	@$(LD) -T userland/userland.ld -o $@ $(CRT0_OBJ) $(BUILD_DIR)/apps/$*.o $(USER_OBJS_C)

bin/minishell/minishell: $(CRT0_OBJ) $(USER_OBJS_C) $(SRCS_MINISHELL)
	@$(MAKE) -C bin/minishell

$(DISK_IMG): $(NAME) $(APPS_OBJ) bin/minishell/minishell
	@echo "Creating a temporary directory structure"
	@mkdir -p $(FS_DIR)/home/kfs/fs
	@mkdir -p $(FS_DIR)/etc
	@echo -n "root:x:0:0:root:/root/kfs/src:/bin/minishell\neandres:x:1000:1000:user:/home/eandres:/bin/minishell" > $(FS_DIR)/etc/passwd
	@echo -n "root:hash\neandres:hash" > $(FS_DIR)/etc/shadow
	@mkdir -p $(FS_DIR)/root/kfs/src
	@mkdir -p $(FS_DIR)/home/eandres
	@mkdir -p $(FS_DIR)/dev
	@echo -n "Mierdon\n" > $(FS_DIR)/dev/file.txt
	@mkdir -p $(FS_DIR)/proc
	@echo "Hola desde el diso duro -> funciona el vfs y ext2" > $(FS_DIR)/home/kfs/file.txt
	@mkdir -p $(FS_DIR)/bin
	@find bin -type f ! -name "*.c" ! -name "*.h" ! -name "Makefile" ! -name "*.o" ! -name "*.s" -exec cp {} $(FS_DIR)/bin/ \;
	@~/genext2fs/genext2fs -N 1024 -b 4096 -d $(FS_DIR) part.img
	@mkdir -p empty_dir
	@~/genext2fs/genext2fs -N 1024 -b 4096 -d empty_dir part2.img
	@dd if=/dev/zero of=$(DISK_IMG) bs=1M count=10 status=none
	@parted -s $(DISK_IMG) mklabel msdos
	@parted -s $(DISK_IMG) mkpart primary ext2 1MiB 25%
	@parted -s $(DISK_IMG) mkpart primary ext2 25% 50%
	@parted -s $(DISK_IMG) mkpart primary ext2 50% 75%
	@parted -s $(DISK_IMG) mkpart primary ext2 75% 100%
	@dd if=part.img of=$(DISK_IMG) bs=1M seek=1 conv=notrunc status=none
	@dd if=part2.img of=$(DISK_IMG) bs=512 seek=5120 conv=notrunc status=none
	@rm -f part.img part2.img
	@rm -rf empty_dir $(FS_DIR)
	@echo "$(DISK_IMG) listo!"

clean:
	@rm -rf $(BUILD_DIR) $(FS_DIR) empty_dir
	@find bin -type f ! -name '*.c' ! -name '*.h' ! -name 'Makefile' -delete

fclean: clean 
	rm -f $(NAME) $(DISK_IMG)
	rm -rf isodir kfs.iso

iso: $(NAME)
	@mkdir -p isodir/boot/grub
	@cp $(NAME) isodir/boot/$(NAME)
	@echo 'menuentry "kfs" {' > isodir/boot/grub/grub.cfg
	@echo ' multiboot /boot/$(NAME)' >> isodir/boot/grub/grub.cfg
	@echo '}' >> isodir/boot/grub/grub.cfg
	@grub-mkrescue -o kfs.iso isodir
	@echo "kfs.iso created"

run:
	qemu-system-i386 -kernel $(NAME) -curses -drive file=disk.img,format=raw,if=ide

debug:
	qemu-system-i386 -kernel $(NAME) -curses -hda disk.img -s -S -d int,cpu_reset -no-reboot

re: fclean all

.PHONY: all clean fclean re run debug iso
