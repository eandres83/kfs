# Program name
NAME = minishell

RM = rm -rf

OBJECTS_DIR = .obj

SRCS = $(wildcard *.c) $(wildcard buildtins/*.c) $(wildcard execute/*.c)

# Generate object files list
OBJS = $(SRCS:%.c=$(OBJECTS_DIR)/%.o)

# Default target
all: $(NAME)

# Link the final executable
$(NAME): $(OBJS) $(LIBFT)
	@$(CC) $(USER_CFLAGS) $(OBJS) -o $(NAME)
	@echo "-> Compilation completed: $(NAME)"

# Build object files
$(OBJECTS_DIR)%.o: %.c
	@mkdir -p $(dir $@)
	@$(CC) $(USER_CFLAGS) -c $< -o $@

# Clean object files and the objects directory
clean:
	@$(RM) $(OBJECTS_DIR)
	@echo "-> All .o files removed"

# Remove everything, including the executable
fclean: clean
	@$(RM) $(NAME)
	@echo "-> $(NAME) removed"

# Rebuild everything from scratch
re: fclean all

# Specify targets that aren't files
.PHONY: all clean fclean re
