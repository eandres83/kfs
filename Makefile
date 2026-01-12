CC = i686-elf-gcc
AS = i686-elf-as

INCLUDES = -I include -I src

CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -Werror -g \
	 -fno-builtin -fno-exceptions -fno-stack-protector \
	 -nostdlib -nodefaultlibs $(INCLUDES)

LDFLAGS = -T linker.ld

NAME = kernel.bin

BUILD_DIR = .obj

SRCS_C = $(wildcard src/drivers/*.c) $(wildcard src/kernel/*.c) $(wildcard src/lib/*.c)
SRCS_S = $(wildcard src/boot/*.s)

OBJS = $(patsubst src/%.c, $(BUILD_DIR)/%.o, $(SRCS_C)) \
	$(patsubst src/%.s, $(BUILD_DIR)/%.o, $(SRCS_S))

all: $(NAME)

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

fclean: clean
	rm -f $(NAME)
	rm -f kfs.iso
	rm -rf isodir

iso: $(NAME)
	@mkdir -p isodir/boot/grub
	@cp $(NAME) isodir/boot/$(NAME)
	@echo 'menuentry "kfs" {' > isodir/boot/grub/grub.cfg
	@echo ' multiboot /boot/$(NAME)' >> isodir/boot/grub/grub.cfg
	@echo '}' >> isodir/boot/grub/grub.cfg
	@grub-mkrescue -o kfs.iso isodir
	@echo "kfs.iso created"

run:
	qemu-system-i386 -kernel $(NAME) -curses

debug:
	qemu-system-i386 -kernel $(NAME) -curses -s -S

re: fclean all

.PHONY: all clean fclean re run debug iso
