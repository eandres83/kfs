CC = i686-elf-gcc
AS = i686-elf-as

INCLUDES = -I include -I src
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -Werror -g \
	 -fno-builtin -fno-exceptions -fno-stack-protector \
	 -nostdlib -nodefaultlibs $(INCLUDES)

LDFLAGS = -T linker.ld

DISK_IMG = disk.img
FS_DIR = rootfs

NAME = kernel.bin

BUILD_DIR = .obj

SRCS_C = $(wildcard src/drivers/*.c) $(wildcard src/drivers/ide/*.c) $(wildcard src/kernel/*.c) \
	$(wildcard src/lib/*.c) $(wildcard src/mm/*.c) $(wildcard src/arch/i386/*.c) $(wildcard src/task/*.c) \
	$(wildcard src/fs/ext2/*.c) $(wildcard src/fs/vfs/*.c) $(wildcard src/fs/*.c) 
SRCS_S = $(wildcard src/boot/*.s) $(wildcard src/mm/*.s) $(wildcard src/arch/i386/*.s) $(wildcard src/task/*.s)

OBJS = $(patsubst src/%.c, $(BUILD_DIR)/%.o, $(SRCS_C)) \
	$(patsubst src/%.s, $(BUILD_DIR)/%.o, $(SRCS_S))

all: $(DISK_IMG) $(NAME)

$(NAME): $(OBJS)
	@echo "Linking kernel..."
	$(CC) $(LDFLAGS) -o $(NAME) $(OBJS) -nostdlib -lgcc
	@echo "!Kernel compiled"

$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling C: $<"
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: src/%.s
	@mkdir -p $(dir $@)
	@echo "Compiling ASM: $<"
	$(AS) -o $@ $<

clean:
	rm -rf $(BUILD_DIR)

fclean: clean clean-disk
	rm -f $(NAME)
	rm -f kfs.iso 
	rm -rf isodir
	rm -rf part.img part2.img

iso: $(NAME)
	@mkdir -p isodir/boot/grub
	@cp $(NAME) isodir/boot/$(NAME)
	@echo 'menuentry "kfs" {' > isodir/boot/grub/grub.cfg
	@echo ' multiboot /boot/$(NAME)' >> isodir/boot/grub/grub.cfg
	@echo '}' >> isodir/boot/grub/grub.cfg
	@grub-mkrescue -o kfs.iso isodir
	@echo "kfs.iso created"

$(DISK_IMG): test_bin
	@echo "Creating a temporary directory structure"
	@mkdir -p $(FS_DIR)/home/kfs
	@mkdir -p $(FS_DIR)/home/kfs/fs
	@mkdir -p $(FS_DIR)/etc
	@echo -n "root:root1\neandres:1234\nfuck:oian\n1:1\n" > $(FS_DIR)/etc/passwd
	@cp test_bin $(FS_DIR)/home/kfs
	@mkdir -p $(FS_DIR)/sys
	@mkdir -p $(FS_DIR)/var
	@mkdir -p $(FS_DIR)/dev
	@echo -n "Mierdon\n" > $(FS_DIR)/dev/file.txt
	@mkdir -p $(FS_DIR)/proc
	@echo "Hola desde el diso duro -> funciona el vfs y ext2" > $(FS_DIR)/home/kfs/file.txt
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
	@rm -rf empty_dir part.img part2.img $(FS_DIR)
	@echo "$(DISK_IMG) listo!"

clean-disk:
	rm -rf $(DISK_IMG)

test_bin:
	$(CC) -m32 -ffreestanding -Wall -Wextra -Werror -nostdlib -fno-builtin -nostartfiles -nodefaultlibs test_bin.c -o test_bin

run:
	qemu-system-i386 -kernel $(NAME) -curses -drive file=disk.img,format=raw,if=ide

debug:
	qemu-system-i386 -kernel $(NAME) -curses -hda disk.img -s -S -d int,cpu_reset -no-reboot

re: fclean all

.PHONY: all clean fclean re run debug iso clean-disk
