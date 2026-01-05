CC = i686-elf-gcc
AS = i686-elf-as

CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -Werror -g \
	 -fno-builtin -fno-exceptions -fno-stack-protector \
	 -nostdlib -nodefaultlibs

LDFLAGS = -T linker.ld

NAME = kernel.bin

SRCS_C = $(wildcard src/kernel/*.c) $(wildcard src/lib/*.c)
SRCS_S = $(wildcard src/boot/*.s)

OBJS = $(SRCS_S:.s=.o) $(SRCS_C:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	@echo "Linking kernel..."
	$(CC) $(LDFLAGS) -o $(NAME) $(OBJS) -lgc
	@echo "!Kernel compiled"

%.o: %.c
	@echo "Compiling C: $<"
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	@echo "Compiling ASM: $<"
	$(AS) -o $@ $<

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
