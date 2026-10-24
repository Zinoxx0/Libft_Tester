# Builds the runner, the framework, the student's libft objects and one test
# binary per libft function found in SRC_DIR. Normally driven by libt.sh.
#
#   make SRC_DIR=/path/to/libft     build everything for that libft
#   make ... SANITIZE=1             add AddressSanitizer and UBSan
#   make selftest                   test the tester itself (no libft needed)
#   make clean                      remove build/

SRC_DIR		?= .
CC			:= cc
# The subject requires -Wall -Wextra -Werror for the libft. The tester's own
# code is compiled without -Werror on students' machines, so a warning from a
# newer compiler can't stop a test from building; `make selftest` and the CI
# compile it with -Werror instead.
LIBFT_FLAGS	:= -Wall -Wextra -Werror -g
OWN_FLAGS	:= -Wall -Wextra -g
# Every malloc/free in the libft and in the tests goes to the framework's
# tracker (framework/memory.c), which catches leaks, overflows and bad frees.
TRACK		:= -Dmalloc=tester_malloc -Dfree=tester_free

ifeq ($(SANITIZE),1)
SAN_FLAGS	:= -fsanitize=address,undefined -fno-sanitize-recover=all \
			   -fno-omit-frame-pointer
LIBFT_FLAGS	+= $(SAN_FLAGS)
OWN_FLAGS	+= $(SAN_FLAGS)
endif

BUILD		:= build
LIBFT_OBJ	:= $(BUILD)/obj/libft
FRAME_OBJ	:= $(BUILD)/obj/framework
RUNNER_OBJ	:= $(BUILD)/obj/runner
TEST_OBJ	:= $(BUILD)/obj/tests
SUM_DIR		:= $(BUILD)/sums
BIN_DIR		:= $(BUILD)/bin
LOG_DIR		:= $(BUILD)/log
LIB			:= $(BUILD)/libft.a
RUNNER		:= $(BUILD)/runner

SRCS		:= $(wildcard $(SRC_DIR)/*.c)
OBJS		:= $(patsubst $(SRC_DIR)/%.c,$(LIBFT_OBJ)/%.o,$(SRCS))
FRAME_OBJS	:= $(patsubst framework/%.c,$(FRAME_OBJ)/%.o,$(wildcard framework/*.c))
RUNNER_OBJS	:= $(patsubst runner/%.c,$(RUNNER_OBJ)/%.o,$(wildcard runner/*.c))
TESTS		:= $(patsubst tests/%_test.c,%,$(wildcard tests/*_test.c))
# Only the functions that have both a test and a file in the libft
FOUND		:= $(filter $(TESTS),$(basename $(notdir $(SRCS))))
BINS		:= $(addprefix $(BIN_DIR)/,$(FOUND))

# Everything is rebuilt when this text changes: another libft directory,
# other flags, or a header whose content changed.
CONFIG		:= $(BUILD)/config
SETTINGS	:= $(abspath $(SRC_DIR)) $(CC) $(LIBFT_FLAGS) $(OWN_FLAGS) \
			   $(shell cat $(SRC_DIR)/*.h 2> /dev/null | cksum)
$(shell mkdir -p $(LIBFT_OBJ) $(FRAME_OBJ) $(RUNNER_OBJ) $(TEST_OBJ) \
	$(SUM_DIR) $(BIN_DIR) $(LOG_DIR); \
	echo '$(SETTINGS)' | cmp -s - $(CONFIG) || echo '$(SETTINGS)' > $(CONFIG))

RED			:= \033[31m
RESET		:= \033[0m

.PHONY: all clean re selftest FORCE
.SECONDARY:

all: $(RUNNER) $(BINS)

# ---------------------------------------------------------------- the tester

$(RUNNER): $(RUNNER_OBJS)
	@$(CC) $(OWN_FLAGS) $^ -o $@

$(RUNNER_OBJ)/%.o: runner/%.c $(CONFIG)
	@$(CC) $(OWN_FLAGS) -MMD -MP -c $< -o $@

# The framework itself is not tracked: it implements the tracking
$(FRAME_OBJ)/%.o: framework/%.c $(CONFIG)
	@$(CC) $(OWN_FLAGS) -MMD -MP -c $< -o $@

# ---------------------------------------------------------- the student's libft

# make only compares dates, so a file put back with an older date (mv, cp -p,
# a backup) would keep its stale object. Each source gets a checksum file that
# only changes when the content does, and the object depends on that instead.
$(SUM_DIR)/%.sum: $(SRC_DIR)/%.c FORCE
	@cksum < $< | cmp -s - $@ || cksum < $< > $@

# A file that doesn't compile is reported and left out, so the other functions
# can still be tested; the runner shows the errors from the log.
$(LIBFT_OBJ)/%.o: $(SUM_DIR)/%.sum $(CONFIG)
	@$(CC) $(LIBFT_FLAGS) $(TRACK) -MMD -MP -I$(SRC_DIR) \
		-c $(SRC_DIR)/$*.c -o $@ 2> $(LOG_DIR)/$*.c.log \
		|| { rm -f $@; printf "  $(RED)✗$(RESET) %s doesn't compile\n" $*.c; }

# Linking against an archive only pulls in the objects a test needs, so a
# stray main.c or an unrelated file can't get in the way.
$(LIB): $(OBJS)
	@rm -f $@
	@objs="$$(ls $(OBJS) 2> /dev/null)"; [ -z "$$objs" ] || ar rcs $@ $$objs

# ------------------------------------------------------------------ the tests

$(TEST_OBJ)/%.o: tests/%_test.c $(CONFIG)
	@$(CC) $(OWN_FLAGS) $(TRACK) -MMD -MP -Iframework -I$(SRC_DIR) \
		-c $< -o $@ 2> $(LOG_DIR)/$*.test.log \
		|| { rm -f $@; printf "  $(RED)✗$(RESET) the %s test doesn't compile with your libft.h\n" $*; }

$(BIN_DIR)/%: $(TEST_OBJ)/%.o $(LIB) $(FRAME_OBJS)
	@rm -f $@ && : > $(LOG_DIR)/$*.link.log
	@if [ -f $< ] && [ -f $(LIBFT_OBJ)/$*.o ]; then \
		$(CC) $(OWN_FLAGS) $< $(FRAME_OBJS) $(LIB) -o $@ \
			2> $(LOG_DIR)/$*.link.log \
		|| printf "  $(RED)✗$(RESET) the %s test doesn't link\n" $*; \
	fi

-include $(wildcard $(BUILD)/obj/*/*.d)

# ------------------------------------------------------------------- the rest

selftest:
	@CC="$(CC)" ./selftest/run.sh

clean:
	@rm -rf $(BUILD)

re: clean all
