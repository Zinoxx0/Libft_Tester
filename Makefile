# Builds the runner and one test binary per libft function found in SRC_DIR.
# Normally driven by libt.sh. By hand: make SRC_DIR=/path/to/libft [SANITIZE=1]

SRC_DIR		?= .
CC			:= cc
CFLAGS		:= -Wall -Wextra -Werror -g
# Every malloc/free in the libft sources and in the tests goes through the
# framework, which checks for leaks, overflows and bad frees.
TRACK		:= -Dmalloc=tester_malloc -Dfree=tester_free

ifeq ($(SANITIZE),1)
CFLAGS		+= -fsanitize=address,undefined -fno-sanitize-recover=all \
			   -fno-omit-frame-pointer
endif

BUILD		:= build
OBJ_DIR		:= $(BUILD)/obj
TEST_DIR	:= $(BUILD)/tests
BIN_DIR		:= $(BUILD)/bin
LOG_DIR		:= $(BUILD)/log
LIB			:= $(BUILD)/libft.a
FRAMEWORK	:= $(BUILD)/tester.o
RUNNER		:= $(BUILD)/runner

SRCS		:= $(wildcard $(SRC_DIR)/*.c)
OBJS		:= $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
TESTS		:= $(patsubst tests/%_test.c,%,$(wildcard tests/*_test.c))
FOUND		:= $(filter $(TESTS),$(basename $(notdir $(SRCS))))
BINS		:= $(addprefix $(BIN_DIR)/,$(FOUND))

# Everything is rebuilt when the libft directory or the flags change.
CONFIG		:= $(BUILD)/config
SETTINGS	:= $(abspath $(SRC_DIR)) $(CC) $(CFLAGS)
$(shell mkdir -p $(OBJ_DIR) $(TEST_DIR) $(BIN_DIR) $(LOG_DIR); \
	echo '$(SETTINGS)' | cmp -s - $(CONFIG) || echo '$(SETTINGS)' > $(CONFIG))

RED			:= \033[31m
RESET		:= \033[0m

.PHONY: all clean re
.SECONDARY:

all: $(RUNNER) $(BINS)

$(RUNNER): runner.c $(CONFIG)
	@$(CC) $(CFLAGS) $< -o $@

$(FRAMEWORK): framework/tester.c framework/tester.h $(CONFIG)
	@$(CC) $(CFLAGS) -c $< -o $@

# A libft file that doesn't compile is reported and left out, so the other
# functions can still be tested. The runner shows the errors from the log.
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(CONFIG)
	@$(CC) $(CFLAGS) $(TRACK) -MMD -MP -I$(SRC_DIR) -c $< -o $@ \
		2> $(LOG_DIR)/$*.c.log \
		|| { rm -f $@; printf "  $(RED)✗$(RESET) %s doesn't compile\n" $*.c; }

$(LIB): $(OBJS)
	@rm -f $@
	@objs="$$(ls $(OBJS) 2> /dev/null)"; [ -z "$$objs" ] || ar rcs $@ $$objs

$(TEST_DIR)/%.o: tests/%_test.c $(CONFIG)
	@$(CC) $(CFLAGS) $(TRACK) -MMD -MP -Iframework -I$(SRC_DIR) -c $< -o $@ \
		2> $(LOG_DIR)/$*.test.log \
		|| { rm -f $@; printf "  $(RED)✗$(RESET) the %s test doesn't compile with your libft.h\n" $*; }

$(BIN_DIR)/%: $(TEST_DIR)/%.o $(LIB) $(FRAMEWORK)
	@rm -f $@ && : > $(LOG_DIR)/$*.link.log
	@if [ -f $< ] && [ -f $(OBJ_DIR)/$*.o ]; then \
		$(CC) $(CFLAGS) $< $(FRAMEWORK) $(LIB) -o $@ 2> $(LOG_DIR)/$*.link.log \
		|| printf "  $(RED)✗$(RESET) the %s test doesn't link\n" $*; \
	fi

-include $(wildcard $(OBJ_DIR)/*.d $(TEST_DIR)/*.d)

clean:
	@rm -rf $(BUILD)

re: clean all
