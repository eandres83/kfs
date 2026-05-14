# Program name
NAME = minishell

RM = rm -rf

OBJECTS_DIR = .object

SRCS = $(wildcard *.c) $(wildcard builtins/*.c) $(wildcard execute/*.c)
MINISHELL_OBJS = $(SRCS:/%.c=$(OBJECTS_DIR)/%.o)

ABS_USER_OBJS = $(addprefix $(PROJECT_ROOT)/, $(USER_OBJS_C))
ABS_CRT0 = $(PROJECT_ROOT)/$(CRT0_OBJ)

# Default target
all: $(NAME)

# Link the final executable
$(NAME): $(MINISHELL_OBJS)
	@$(CC) $(USER_CFLAGS) $(MINISHELL_OBJS) $(ABS_USER_OBJS) $(ABS_CRT0) -o $(NAME)
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
