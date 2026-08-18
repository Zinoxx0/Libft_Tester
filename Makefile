NAME        := libft_tester
CC          := cc
CFLAGS      := -Wall -Wextra -Werror -g

# The '?=' allows this to be overridden by the command line (e.g., SRC_DIR=$PWD)
SRC_DIR     ?= src

TEST_DIR    := tests
BIN_DIR     := $(TEST_DIR)/bin
OBJ_DIR     := $(TEST_DIR)/obj

# Get all .c files from the target directory
SRC         := $(wildcard $(SRC_DIR)/*.c)

# Strip the path so we just have "ft_split.c", "ft_itoa.c", etc.
SRC_FILES   := $(notdir $(SRC))

# Map them to our local object directory: tests/obj/ft_split.o
OBJ         := $(addprefix $(OBJ_DIR)/, $(SRC_FILES:.c=.o))

TEST_SRC    := $(wildcard $(TEST_DIR)/*_tester.c)
TEST_NAMES  := $(patsubst $(TEST_DIR)/%_tester.c,%,$(TEST_SRC))

VALID_NAMES := $(foreach t,$(TEST_NAMES),$(if $(wildcard $(SRC_DIR)/$(t).c),$(t)))
TEST_BINS   := $(addprefix $(BIN_DIR)/,$(VALID_NAMES))

.PHONY: all clean fclean re tests list

all: $(NAME) tests

$(NAME): main_tester.c
	@$(CC) $(CFLAGS) -o $@ $<

tests: $(TEST_BINS)

# Create both bin and obj directories
$(BIN_DIR) $(OBJ_DIR):
	@mkdir -p $@

# Compile the user's source files into our local OBJ_DIR
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@printf "  CC  %-24s\n" "$@"
	@$(CC) $(CFLAGS) -I$(SRC_DIR) -c $< -o $@

$(BIN_DIR)/%: $(TEST_DIR)/%_tester.c $(OBJ) | $(BIN_DIR)
	@printf "  LD  %-24s\n" "$@"
	@$(CC) $(CFLAGS) -I$(SRC_DIR) -o $@ $< $(OBJ)

list:
	@printf "Available testers:\n"
	@for t in $(VALID_NAMES); do printf "  %s\n" "$$t"; done

clean:
	@rm -rf $(BIN_DIR) $(OBJ_DIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all