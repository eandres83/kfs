export CC = i686-elf-gcc
AS = i686-elf-as
LD = i686-elf-ld

INCLUDES = -I include -I src
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -Werror -g \
	 -fno-builtin -fno-exceptions -fno-stack-protector \
	 -nostdlib -nodefaultlibs $(INCLUDES)

# flag to print debug msg or not
ifeq ($(DEBUG), 1)
	CFLAGS += -DKERNEL_DEBUG endif
endif

# check if user has cross compiler
REQUIRED_BINS := i686-elf-gcc i686-elf-as i686-elf-ld genext2fs
ifneq ($(MAKECMDGOALS),toolchain)
        MISSING_BINS := $(foreach bin,$(REQUIRED_BINS),$(if $(shell command -v $(bin) 2> /dev/null),,$(bin)))
        ifneq ($(strip $(MISSING_BINS)),)
                $(error "Error: no $(MISSING_BINS) in your system. Execute make toolchain, this process may take a few minutes, but it's essential")
        endif
endif

export USER_CFLAGS = -m32 -ffreestanding -Wall -Wextra -Werror -g -fno-builtin -nostdlib -nodefaultlibs -I userland
LDFLAGS = -T linker.ld

export PROJECT_ROOT := $(CURDIR)

DISK_IMG = disk.img
TMP_DIR = .tmp_build
FS_DIR = $(TMP_DIR)/sysroot
PART1_IMG = $(TMP_DIR)/part.img
PART2_IMG = $(TMP_DIR)/part2.img
GENEXT2FS_BIN = $(HOME)/sgoinfre/cross/bin/genext2fs

KERNEL_DRAFT = kernel_draft.bin
KERNEL_FINAL = kernel.bin

SYM_SCRIPT = script/generate_table.sh
SYM_C = src/symbol_table.c
SYM_OBJ = $(BUILD_DIR)/src/symbol_table.o
DUMMY_OBJ = $(BUILD_DIR)/src/modules/dummy_symbols.o

export BUILD_DIR = .obj

SRCS_MINISHELL = $(foreach dir, $(shell find bin/minishell -type d), $(wildcard $(dir)/*.c))

SRCS_C = $(foreach dir, $(shell find src -type d), $(wildcard $(dir)/*.c))
SRCS_S = $(foreach dir, $(shell find src -type d), $(wildcard $(dir)/*.s))

ALL_RAW_OBJS = $(patsubst src/%.c, $(BUILD_DIR)/src/%.o, $(SRCS_C)) $(patsubst src/%.s, $(BUILD_DIR)/src/%.o, $(SRCS_S))
CORE_OBJS := $(filter-out %dummy_symbols.o %symbol_table.o,$(ALL_RAW_OBJS))

SRCS_MODULE = $(shell find modules -type f -name "*.c")
OBJS_MODULE = $(patsubst modules/%.c, $(BUILD_DIR)/modules/%.o, $(SRCS_MODULE))

USER_SRCS_C = $(foreach dir, $(shell find userland -type d), $(wildcard $(dir)/*.c))
export USER_OBJS_C = $(patsubst userland/%.c, $(BUILD_DIR)/userland/%.o, $(USER_SRCS_C))
export CRT0_OBJ = $(BUILD_DIR)/userland/crt0.o

APPS_SRCS = $(wildcard bin/*.c)
APPS_OBJ = $(patsubst bin/%.c, bin/%, $(APPS_SRCS))

all: $(KERNEL_FINAL) $(DISK_IMG)

# KERNEL
$(KERNEL_DRAFT): $(CORE_OBJS) $(DUMMY_OBJ)
	$(CC) $(LDFLAGS) -o $@ -ffreestanding -O2 -nostdlib $^ -lgcc

$(SYM_C): $(KERNEL_DRAFT)
	nm $< | $(SYM_SCRIPT) > $@

$(SYM_OBJ): $(SYM_C)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_FINAL): $(CORE_OBJS) $(SYM_OBJ)
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

.PHONY: toolchain
toolchain:
	@bash script/build_cross_compiler.sh

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

$(FS_DIR): $(KERNEL_FINAL) $(APPS_OBJ) bin/minishell/minishell
	@echo "Creating a temporary directory structure"
	@sh script/sysroot.sh $(FS_DIR)
	@find bin -type f ! -name "*.c" ! -name "*.h" ! -name "Makefile" ! -name "*.o" ! -name "*.s" -exec cp {} $(FS_DIR)/bin/ \;

$(GENEXT2FS_BIN):
	@bash script/build_cross_compiler.sh genext2fs

$(PART1_IMG): $(FS_DIR) $(GENEXT2FS_BIN)
	@mkdir -p $(TMP_DIR)
	@genext2fs -N 1024 -b 4096 -d $(FS_DIR) $(PART1_IMG)

$(PART2_IMG):
	@mkdir -p $(TMP_DIR)/empty_dir
	@genext2fs -N 1024 -b 4096 -d $(TMP_DIR)/empty_dir $(PART2_IMG)

$(DISK_IMG): $(PART1_IMG) $(PART2_IMG)
	@dd if=/dev/zero of=$(DISK_IMG) bs=1M count=10 status=none
	@parted -s $(DISK_IMG) mklabel msdos
	@parted -s $(DISK_IMG) mkpart primary ext2 1MiB 25%
	@parted -s $(DISK_IMG) mkpart primary ext2 25% 50%
	@parted -s $(DISK_IMG) mkpart primary ext2 50% 75%
	@parted -s $(DISK_IMG) mkpart primary ext2 75% 100%
	@dd if=$(PART1_IMG) of=$(DISK_IMG) bs=1M seek=1 conv=notrunc status=none
	@dd if=$(PART2_IMG) of=$(DISK_IMG) bs=512 seek=5120 conv=notrunc status=none
	@echo "$(DISK_IMG) done!"

$(BUILD_DIR)/modules/%.o: modules/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling modules :$*"
	@$(CC) $(CFLAGS) -c $< -o $@

.PHONY: modules
modules: $(OBJS_MODULE)
	@cp $^ $(FS_DIR)/bin/

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR) $(TMP_DIR)
	@find bin -type f ! -name '*.c' ! -name '*.h' ! -name 'Makefile' -delete

.PHONY: fclean
fclean: clean 
	@rm -f $(KERNEL_FINAL) $(DISK_IMG)
	@rm -f $(KERNEL_DRAFT)
	@rm -f $(SYM_C) $(SYM_OBJ)
	@rm -rf isodir kfs.iso

.PHONY: iso
iso: $(KERNEL_FINAL)
	@mkdir -p isodir/boot/grub
	@cp $(KERNEL_FINAL) isodir/boot/$(KERNEL_FINAL)
	@echo 'menuentry "kfs" {' > isodir/boot/grub/grub.cfg
	@echo ' multiboot /boot/$(KERNEL_FINAL)' >> isodir/boot/grub/grub.cfg
	@echo '}' >> isodir/boot/grub/grub.cfg
	@grub-mkrescue -o kfs.iso isodir
	@echo "kfs.iso created"

.PHONY: run
run:
	qemu-system-i386 -kernel $(KERNEL_FINAL) -curses -drive file=disk.img,format=raw,if=ide

.PHONY: debug
debug:
	qemu-system-i386 -kernel $(KERNEL_FINAL) -curses -hda disk.img -s -S -d int,cpu_reset -no-reboot

.PHONY: re
re: fclean all

