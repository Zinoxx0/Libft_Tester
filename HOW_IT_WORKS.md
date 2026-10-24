# How the Libft Tester works

This document explains how the tester is built: what each part does, why it is done that way, and the system calls and C features behind it. It is written for 42 students who have finished (or are writing) libft, so it assumes you know C, pointers and `malloc`, but not `fork`, `mmap` or how `make` really decides what to rebuild. Those are explained as they come up, and summed up in the [glossary](#10-glossary) at the end.

Read it top to bottom the first time: section 1 gives the big picture, and each following section zooms into one part.

## Contents

1. [The big picture](#1-the-big-picture)
2. [The launcher: libt.sh](#2-the-launcher-libtsh)
3. [The build: Makefile](#3-the-build-makefile)
4. [The runner: runner/](#4-the-runner-runner)
5. [The framework: framework/](#5-the-framework-framework)
6. [The tests: tests/](#6-the-tests-tests)
7. [Testing the tester: selftest/](#7-testing-the-tester-selftest)
8. [Recipes](#8-recipes)
9. [Design decisions in one place](#9-design-decisions-in-one-place)
10. [Glossary](#10-glossary)

---

## 1. The big picture

The tester is made of four parts, plus a fifth that tests the tester itself:

| Part | Files | Job |
|---|---|---|
| Launcher | `libt.sh` | What you run. Checks you're in a libft directory, calls `make`, starts the runner. |
| Build | `Makefile` | Compiles your libft, the framework and the tests into `build/`, and links one test program per function. |
| Runner | `runner/` | The menu and the command line. Decides which test programs to run, runs them, prints the summary. |
| Framework | `framework/` | The library every test is built on: the `check_*` functions, running each case in its own process, the malloc tracker, guarded buffers, output capture. |
| Tests | `tests/` | One small C program per libft function: `ft_strlen_test.c`, `ft_split_test.c`... |
| Self-test | `selftest/` | Tests for the tester: `make selftest`. |

### What happens when you type `libt ft_split`

```
 you ─► libt.sh ─────► make ──────────────────────► build/
         │              compiles your ft_*.c        ├── libft.a         (your objects)
         │              compiles the framework      ├── obj/framework/  (the framework)
         │              compiles + links the tests  └── bin/ft_split    (one program per function)
         │
         └─ exec ─► build/runner ft_split
                       │
                       └─ fork + exec ─► build/bin/ft_split
                                            │
                                            ├─ fork ─► case "words separated by spaces"
                                            ├─ fork ─► case "separators at the ends..."
                                            ├─ ...
                                            └─ fork ─► case "returns NULL ... when malloc fails"
```

Step by step:

1. **`libt.sh`** checks that the current directory has `ft_*.c` files, then runs `make SRC_DIR=<your directory>` inside the tester's directory.
2. **`make`** compiles each of your `.c` files into `build/obj/libft/`, packs them into `build/libft.a`, compiles `tests/ft_split_test.c`, and links it with the framework and your `libft.a` into `build/bin/ft_split`. It only redoes what changed since last time.
3. **`libt.sh`** replaces itself with **`build/runner`**, passing your arguments (`ft_split`) and two environment variables saying where your libft and the build are.
4. **The runner** finds `ft_split` in its table of the 43 subject functions, sees that `build/bin/ft_split` exists, and starts it as a new process.
5. **`build/bin/ft_split`** is a normal C program. Its `main` lists the cases with `test("title", function)`. For each case, the framework **forks**: the case runs in a child process with a 5-second alarm. If your `ft_split` crashes, only that child dies; the framework reports it and moves on to the next case.
6. Each test program exits with 0 if every case passed and 1 otherwise. The runner collects these and prints the summary.

The rest of this document follows that same path.

---

## 2. The launcher: libt.sh

`libt.sh` is short, and it deliberately contains no build logic: everything about compiling lives in the Makefile, so `make` gives the same result when you run it by hand.

**Finding the tester's directory.** The alias points at `~/Libft_Tester/libt.sh`, but someone might instead symlink the script into `~/bin`. `${BASH_SOURCE[0]}` is the path the script was started from; the `while [ -L "$script" ]` loop follows symlinks until it reaches the real file, and its directory is `TESTER_DIR`. `USER_DIR` is simply `$PWD`, where you ran the command.

**Arguments.** `--asan` is the only option `libt.sh` handles itself, because it changes how things are *compiled*: it becomes `SANITIZE=1` for `make`, and sets `ASAN_OPTIONS` / `UBSAN_OPTIONS` so a sanitizer error exits with status 86 (the framework recognises that number, see [5.1](#51-every-case-runs-in-its-own-process)). Every other argument is passed to the runner untouched.

**Sanity checks.** No `ft_*.c` in the current directory means you're not in a libft directory: it stops with a message. A path with spaces is refused, because `make` splits words on spaces and can't handle such paths.

**Building.** `make -s -j<number of CPUs> -C <tester dir> SRC_DIR=<your dir>`:

- `-C` runs make inside the tester's directory, wherever you are;
- `-j` compiles several files at once (`getconf _NPROCESSORS_ONLN` gives the number of processors);
- `-s` (silent) hides make's own messages, such as "Entering directory"; the Makefile prints its own short lines instead.

**Starting the runner.** The last line is

```sh
LIBT_SRC="$USER_DIR" LIBT_BUILD="$TESTER_DIR/build" exec "$TESTER_DIR/build/runner" "${args[@]}"
```

`VAR=value command` sets environment variables for that command only. `exec` *replaces* the shell process with the runner instead of starting a child: there's nothing left for the script to do, and the runner's exit status becomes `libt`'s exit status directly.

---

## 3. The build: Makefile

### 3.1 What gets built

```
build/
├── config            what the current build was made for (see 3.4)
├── sums/             one checksum file per libft source (see 3.4)
├── obj/
│   ├── libft/        your ft_*.o, compiled with -Werror and the malloc tracking
│   ├── framework/    the framework's objects, compiled without tracking
│   ├── runner/       the runner's objects
│   └── tests/        the tests' objects, compiled with tracking
├── libft.a           an archive of your objects
├── runner            the runner program
├── bin/              one test program per function: ft_strlen, ft_split, ...
└── log/              compiler output: ft_split.c.log, ft_split.test.log, ft_split.link.log
```

Nothing is ever written to your libft directory, and `make clean` simply deletes `build/`.

### 3.2 A short primer on make

A rule says how to make a **target** from its **prerequisites**:

```make
build/obj/libft/%.o: build/sums/%.sum build/config
	cc ... -c $(SRC_DIR)/$*.c -o $@
```

- `%` is a pattern: this one rule works for every `ft_something.o`. Inside the recipe, `$*` is what `%` matched (`ft_split`), `$@` is the target, `$<` the first prerequisite and `$^` all of them.
- make rebuilds a target when it doesn't exist, or when one of its prerequisites is **newer** (by modification date) than it. That's the whole algorithm, and section 3.4 is about its blind spots.
- Every recipe line runs in its own shell. A line starting with `@` isn't printed.
- `.PHONY: clean` says `clean` is a name, not a file, so it always runs.

### 3.3 The variables, with real values

For a libft in `~/libft` with all 43 files:

| Variable | Value | How |
|---|---|---|
| `SRCS` | `~/libft/ft_atoi.c ~/libft/ft_bzero.c ...` | `$(wildcard $(SRC_DIR)/*.c)` |
| `OBJS` | `build/obj/libft/ft_atoi.o ...` | the same names, moved to `build/obj/libft/` |
| `TESTS` | `ft_atoi ft_bzero ...` | every `tests/*_test.c`, without the suffix |
| `FOUND` | the functions that have **both** a test and a file in your libft | `$(filter $(TESTS), names of your files)` |
| `BINS` | `build/bin/ft_atoi ...` | one test program per `FOUND` function |

`all` builds `$(RUNNER)` and `$(BINS)`. If you haven't written `ft_split.c` yet, `ft_split` isn't in `FOUND`, so its test is never built, and the runner shows it as missing.

### 3.4 Rebuilding exactly what changed

A tester that silently runs an *old* version of your code is worse than no tester, so this part matters. make's date comparison has three blind spots, and each one has a fix.

**Blind spot 1: headers.** `ft_split.o` is built from `ft_split.c`, but it also depends on `libft.h`: change a prototype there and `ft_split.o` must be rebuilt. Listing every header by hand is error-prone, so the compiler does it: with `-MMD -MP`, compiling `ft_split.c` also writes `ft_split.d`, a little makefile:

```make
build/obj/libft/ft_split.o: /home/you/libft/ft_split.c /home/you/libft/libft.h
/home/you/libft/libft.h:
```

The Makefile reads all of them with `-include $(wildcard $(BUILD)/obj/*/*.d)` (the `-` means "no error if there are none yet"). The second line, added by `-MP`, is an empty rule for the header, so deleting a header doesn't break the build with "no rule to make target".

**Blind spot 2: things that aren't files.** If you run `libt` in another libft directory, or with `--asan`, every object must be rebuilt, but no file date changed. The Makefile keeps a description of the current build in `build/config`:

```make
SETTINGS := $(abspath $(SRC_DIR)) $(CC) $(LIBFT_FLAGS) $(OWN_FLAGS) \
            $(shell cat $(SRC_DIR)/*.h 2> /dev/null | cksum)
$(shell ... echo '$(SETTINGS)' | cmp -s - $(CONFIG) || echo '$(SETTINGS)' > $(CONFIG))
```

`$(shell ...)` runs a command while make *reads* the Makefile, before any rule. `cmp -s` compares the new description with the saved one; only when they differ (`||`) is the file rewritten, which makes it newer than every object. Every compile rule lists `$(CONFIG)` as a prerequisite, so a new directory, new flags or a changed header (the `cksum` of all headers is part of the text) rebuilds everything, and an unchanged setup rebuilds nothing.

**Blind spot 3: files put back with an older date.** `mv` keeps a file's date, and so do `cp -p` and restoring a backup. Put back yesterday's `ft_split.c` and it is *older* than today's `ft_split.o`: make thinks the object is up to date, and you test the wrong code. So objects don't depend on the source's date but on its **content**:

```make
$(SUM_DIR)/%.sum: $(SRC_DIR)/%.c FORCE
	@cksum < $< | cmp -s - $@ || cksum < $< > $@

$(LIBFT_OBJ)/%.o: $(SUM_DIR)/%.sum $(CONFIG)
	...compile $(SRC_DIR)/$*.c...
```

`FORCE` is a phony target, so the `.sum` recipe runs on every build. It rewrites the checksum file only when the content changed, and make checks the file's date again after running a recipe, so the object is rebuilt exactly when the content changed: not on a plain `touch`, but always on a real change, whatever the dates say.

### 3.5 A file that doesn't compile doesn't stop the build

You're halfway through `ft_split.c`, it doesn't compile yet, and you want to test `ft_strlen`. A normal build would stop at the first error. This one doesn't:

```make
$(LIBFT_OBJ)/%.o: $(SUM_DIR)/%.sum $(CONFIG)
	@$(CC) $(LIBFT_FLAGS) $(TRACK) -MMD -MP -I$(SRC_DIR) \
		-c $(SRC_DIR)/$*.c -o $@ 2> $(LOG_DIR)/$*.c.log \
		|| { rm -f $@; printf "  ✗ %s doesn't compile\n" $*.c; }
```

- `2> file` sends the compiler's errors to `build/log/ft_split.c.log` instead of the screen.
- `a || b` runs `b` only if `a` failed. Here `b` deletes the object and prints one line. Since the recipe as a whole succeeds (its last command worked), make carries on with the other files.
- Why `rm -f $@`: if an older version of `ft_split.c` compiled, its object is still there. Leaving it would test the old code; deleting it means "this function currently has no object".

The test object and the link step use the same pattern, with `ft_split.test.log` and `ft_split.link.log`. Later, the runner sees a function with a source file but no test program, calls it "didn't build", and prints the first non-empty log when you select it.

### 3.6 libft.a, and why an archive

```make
$(LIB): $(OBJS)
	@rm -f $@
	@objs="$$(ls $(OBJS) 2> /dev/null)"; [ -z "$$objs" ] || ar rcs $@ $$objs
```

- `ls $(OBJS) 2> /dev/null` lists only the objects that exist: the ones that failed to compile were deleted (3.5). `$$` is how you write a shell `$` inside a make recipe.
- `rm -f $@` first, because `ar` adds to an existing archive: a function you deleted would otherwise stay in it.
- **Why an archive instead of passing all the objects to the linker?** The linker treats an archive as a shelf it takes from: it only pulls in the members that define a symbol it still needs. Linking `ft_strlen`'s test pulls `ft_strlen.o` (and whatever *it* calls) and nothing else. So a `main.c` you left in your directory doesn't clash with the test's `main`, and an unrelated broken function can't stop `ft_strlen`'s test from linking.

### 3.7 Linking a test program

```make
$(BIN_DIR)/%: $(TEST_OBJ)/%.o $(LIB) $(FRAME_OBJS)
	@rm -f $@ && : > $(LOG_DIR)/$*.link.log
	@if [ -f $< ] && [ -f $(LIBFT_OBJ)/$*.o ]; then \
		$(CC) $(OWN_FLAGS) $< $(FRAME_OBJS) $(LIB) -o $@ 2> $(LOG_DIR)/$*.link.log \
		|| printf "  ✗ the %s test doesn't link\n" $*; \
	fi
```

The test is only linked when both its own object and your function's object exist; otherwise the earlier log already explains why. `: > file` empties the old link log, so the runner never shows errors from a previous build. The order matters: test object, framework, then `libft.a`, because the linker reads left to right and an archive only provides symbols that are *already* missing.

`.SECONDARY:` (with nothing after it) tells make to keep every file it builds. Without it, make treats objects built through a chain of pattern rules (`.c → .o → program`) as temporary "intermediate" files, deletes them after the build, and the next build would redo them.

### 3.8 Two sets of flags

```make
LIBFT_FLAGS := -Wall -Wextra -Werror -g
OWN_FLAGS   := -Wall -Wextra -g
TRACK       := -Dmalloc=tester_malloc -Dfree=tester_free
```

- **Your libft** gets `-Werror`: the subject requires it, so a warning in your code is a real problem.
- **The tester's own code** doesn't, on your machine. Compilers add new warnings with every version; with `-Werror`, a new warning in `ft_split_test.c` would stop the test from building, and the message would blame your `libft.h`. The tester's code is still held to `-Werror`, by `make selftest` and the CI (section 7), which catch these warnings before they reach you.
- **`TRACK`** is added to your libft and to the tests, but not to the framework. It is the malloc tracking, explained in [5.4](#54-the-malloc-tracker).
- **`-g`** keeps debug information, so you can run a test program under `gdb` (see [8.3](#83-debug-a-crash-with-gdb)).
- **`SANITIZE=1`** (`libt --asan`) adds AddressSanitizer and UndefinedBehaviorSanitizer to everything; `-fno-sanitize-recover=all` makes the first error stop the case instead of printing and carrying on.

---

## 4. The runner: runner/

| File | Job |
|---|---|
| `runner.h` | The types and the shared state. |
| `functions.c` | The table of the 43 subject functions, and where each one stands. |
| `select.c` | Turns what you typed into a selection. |
| `run.c` | Runs the selected test programs and prints the summary. |
| `menu.c` | The interactive menu. |
| `main.c` | Reads the arguments: names → run them; nothing → open the menu. |

### 4.1 The table and the states

`functions.c` holds one `t_func` per function, in the order of the subject:

```c
typedef struct s_func
{
	const char	*name;		/* "ft_split" */
	t_group		group;		/* PART1, PART2 or PART3 */
	t_state		state;		/* MISSING, BROKEN or READY */
	t_result	result;		/* NOT_RUN, PASSED or FAILED, for the menu */
	int			selected;
}	t_func;
```

`load_states()` sets each state from two files:

| `ft_x.c` in your libft? | `build/bin/ft_x`? | State | Shown as |
|---|---|---|---|
| no | (doesn't matter) | `MISSING` | `·`, greyed out |
| yes | no | `BROKEN` | `!`: it didn't compile or link |
| yes | yes | `READY` | can be run |

The runner gets your directory from `LIBT_SRC` and the build directory from `LIBT_BUILD`, the variables `libt.sh` set.

### 4.2 Selecting

`select_word()` accepts, in this order: `all` (or `-a`); `part1`/`p1`, `part2`/`p2`, `part3`/`p3`; a menu number (`12`); a function name, where the `ft_` prefix and the `.c` are optional (`strlen`, `ft_strlen`, `ft_strlen.c` all work). Each match sets `selected` in the table. On the command line every argument is one word; in the menu, `select_line()` splits the line on spaces, so `1 5 split` selects three functions.

### 4.3 Running a test program

`run_one()` in `run.c`:

- `MISSING`: prints "not found" and runs nothing.
- `BROKEN`: prints the first non-empty log of `build/log/ft_x.c.log`, `ft_x.test.log`, `ft_x.link.log`, so you see the compiler's errors.
- `READY`: starts the program:

```c
pid = fork();
if (pid == 0)
{
	execl(path, f->name, quiet ? "-q" : (char *)NULL, (char *)NULL);
	perror(path);
	_exit(127);
}
waitpid(pid, &status, 0);
```

`fork()` makes a copy of the runner; in the copy (where `fork` returned 0), `execl` replaces the program with the test program. The parent waits for it with `waitpid` and reads its exit status: 0 = every case passed, 1 = something failed. Anything else (a signal, another status) means the test program itself broke, which the framework is built to prevent, and is reported as "the test itself stopped unexpectedly".

When several functions are selected, the runner passes `-q` (quiet): each test program then prints a single line, plus the details of the cases that failed. A single function gets the full list of its cases. `libt -v` turns quiet mode off for groups.

### 4.4 The summary and the exit status

After a group, `print_summary()` prints how many functions passed, then lists the failed, broken and missing ones. In command-line mode, `main()` returns 0 only if everything selected passed, so `libt all && echo ok` works in a script.

### 4.5 The menu

`interactive()` clears the screen (only when the output is a terminal), prints the menu and reads lines with `fgets` until `q` or Ctrl-D. `print_menu()` draws the three parts side by side. Each part is shorter than the one before it (23, 11 and 9 functions), so row `n` always has a prefix of the columns, which keeps the drawing loop simple. Each cell shows the number, the name and a mark: `·` missing, `!` broken, `✓`/`✗` the result of the last run in this session. Because `✓` takes several bytes but one column on screen, the padding is computed from the name's length, not with `printf`'s field width.

Colours are only used when the output is a terminal (`isatty(STDOUT_FILENO)`) and the `NO_COLOR` environment variable isn't set. The test programs make the same decision, so `libt all > results.txt` gives a clean text file.

---

## 5. The framework: framework/

The framework is what makes a test file short. Here is a complete one:

```c
#include "libft.h"
#include "tester.h"

static void	empty_string(void)
{
	doing("ft_strlen(\"\")");
	check_size(ft_strlen(guard_str("")), 0, "return value");
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_strlen");
	PROTOTYPE(size_t, ft_strlen, (const char *));
	test("empty string", empty_string);
	return (done());
}
```

`tester.h` is the public interface: the only header tests include. The `.c` files each do one job, and share their internal state through `internal.h`:

| File | Job | Section |
|---|---|---|
| `suite.c` | `suite()`, `test()`, `done()`: runs each case in a child process | [5.1](#51-every-case-runs-in-its-own-process) |
| `report.c` | Collects failures in the child, prints them in the parent | [5.2](#52-the-report-a-letter-from-the-child) |
| `checks.c` | The `check_*()` functions | [5.3](#53-checks-and-messages) |
| `show.c` | Printable versions of strings, buffers and chars | [5.3](#53-checks-and-messages) |
| `memory.c` | The malloc tracker and the malloc failure sweeps | [5.4](#54-the-malloc-tracker), [5.5](#55-malloc-failure-sweeps) |
| `guard.c` | Buffers placed against untouchable memory | [5.6](#56-guarded-buffers) |
| `capture.c` | Capturing what `ft_put*_fd` write | [5.7](#57-output-capture) |

### 5.1 Every case runs in its own process

If `ft_split` dereferences a NULL pointer, the operating system kills the process with a signal (`SIGSEGV`, "segmentation fault"). If the test and your function lived in the same process, the whole test program would die at the first crash and you'd see nothing from the remaining cases. So `test()` does this:

```c
void	test(const char *title, void (*fn)(void))
{
	memset(g_suite.report, 0, ...);		/* fresh report, see 5.2 */
	fflush(stdout);
	pid = fork();
	if (pid == 0)
		run_child(fn);					/* never returns */
	waitpid(pid, &status, 0);
	why = abnormal_end(status);			/* NULL if the child ended normally */
	...print the case...
}
```

**`fork()`** creates a new process that is a copy of the current one: same code, same variables, same open files. It returns twice: 0 in the new process (the child), and the child's process id in the original (the parent). The child gets a *copy* of the memory: what it changes, the parent never sees. That's why a crash in the child can't hurt the parent, and also why the child needs a special way to send its results back (5.2).

**The child** (`run_child`):

```c
memory_reset();				/* empty malloc tracker */
alarm(g_suite.timeout);		/* SIGALRM in 5 seconds */
fn();						/* the case: calls your function */
alarm(0);					/* cancel the alarm */
if (!g_suite.report->failures)
	report_leaks();			/* see 5.4 */
_exit(failures ? EXIT_FAILED : EXIT_PASSED);
```

- **`alarm(5)`** asks the system to send the signal `SIGALRM` to this process in 5 seconds. Nobody handles that signal, so it kills the process. That's the timeout: an infinite loop in your code ends there instead of hanging the tester forever. `LIBT_TIMEOUT=10 libt` changes the delay.
- **`_exit()` instead of `exit()`**: `exit()` would run cleanup meant for the parent, such as flushing stdio buffers that were copied from it. `_exit()` just ends the process.
- **`fflush(stdout)` before `fork()`**: `printf` doesn't write right away, it fills a buffer. If the buffer weren't empty at the fork, the child would get a copy of it and the same text would be printed twice.

**The parent** decodes the exit status with the `<sys/wait.h>` macros:

| Status | Meaning | Reported as |
|---|---|---|
| exited with `EXIT_PASSED` (0) | every check passed | ✓ |
| exited with `EXIT_FAILED` (10) | some check failed | ✗ plus the failures |
| killed by `SIGALRM` | the alarm went off | "timed out after 5 s (infinite loop?)" |
| killed by another signal | a crash | "crashed: Segmentation fault" (`strsignal()` gives the name) |
| exited with 86 | AddressSanitizer or UBSan stopped it | "stopped by the sanitizer (report above)" |
| exited with anything else | your code called `exit()` | "exited with status 3 (calls exit()?)" |

`WIFSIGNALED(status)` is true when a signal killed the child, and `WTERMSIG(status)` says which one; `WEXITSTATUS(status)` is the number passed to `exit`. The failure code is 10 rather than 1 so that a function calling `exit(1)` can't be mistaken for a normal failed check.

A second benefit of one process per case: every case starts from the same clean state, with an empty malloc tracker and no leftovers from the previous case.

The one time you don't want this is under a debugger, which follows a single process: `LIBT_NO_FORK=1` makes `test()` call the case directly, without fork and alarm (see [8.3](#83-debug-a-crash-with-gdb)).

### 5.2 The report: a letter from the child

The child knows what went wrong; the parent has to print it. But the child's memory is a copy, and when the child crashes, its memory is gone. The solution is a piece of memory that both processes *share*, created once in `suite()`, before any fork:

```c
g_suite.report = mmap(NULL, sizeof(t_report), PROT_READ | PROT_WRITE,
		MAP_SHARED | MAP_ANON, -1, 0);
```

`mmap` asks the system for memory directly. `MAP_ANON` means it isn't backed by a file; `MAP_SHARED` means that after a `fork`, parent and child see the *same* memory instead of separate copies. What the child writes there is still readable after it crashed.

```c
typedef struct s_report
{
	int		failures;		/* failed checks */
	int		warnings;
	int		shown;			/* failures + warnings written to text */
	int		doing_shown;	/* has the current doing() line been written? */
	char	doing[512];		/* the call being tested, set by doing() */
	size_t	len;
	char	text[16384];	/* the lines to print */
}	t_report;
```

**`doing(...)`** stores a description of the call about to be made, `printf`-style: `doing("ft_strlcpy(dst, %s, %zu)", show(src), size)`. It's used twice:

- as a header above the failures that follow it, written only once, the first time something fails under it;
- when the case crashes or times out, the parent prints "while running" plus the last `doing` text. The crash happened during that call, and it's the most useful clue you can get.

**Failures and warnings** go through `note()` in `report.c`, which appends a line to `text`. A case can produce thousands of failures (a loop over 257 characters, all wrong), so only the first 6 are written (2 in quiet mode); the rest are only counted, and the parent prints "... and 1994 more".

**The parent's side.** After `waitpid`, `print_case()` prints the case's line (✓, ✗ or the crash reason), the "while running" line if needed, and the lines of `text` indented. Everything the parent prints goes through `emit()`: in normal mode it goes straight to the screen; in quiet mode it is kept in a growing buffer, because the one-line summary of the function (`✗ ft_itoa   6/7 cases passed`) can only be printed at the end, by `done()`, but has to come *before* the details. Quiet mode also shows at most 3 failing cases per function.

### 5.3 Checks and messages

Each `check_*` compares one value and, when it's wrong, records a failure that describes it:

| Function | Compares | Example message |
|---|---|---|
| `check(ok, fmt, ...)` | anything: you write the message | `returned s itself, not a copy` |
| `check_int(got, want, what)` | `long` values | `return value: expected 5, got 3` |
| `check_size` | `size_t` values | same |
| `check_bool` | truth values | `expected true (non-zero), got 0` |
| `check_sign` | the sign only, for `strncmp`, `memcmp` | `expected a negative value, got 0` |
| `check_str` | strings (NULL allowed) | `result: expected "hello", got "hell"` |
| `check_mem` | `n` bytes, `'\0'` included | `dst: expected "a\0c", got "a\0b"` |
| `check_ptr(got, want, base, what)` | pointers, shown as offsets from `base` | `expected s + 5, got s + 3` |
| `check_alloc` | a malloc block's size | see 5.4 |

Each returns 1 when it passed, so a test can stop early: `if (!check_str(result, want, "result") || !result) return ;`.

**`show()`, `show_mem()`, `show_char()`** turn values into readable text: strings get quotes and C escapes (`"tab\there\n"`, `"\xff"`), long strings are cut after about 200 characters with their length, characters print as `'a'`, `'\n'`, `'\x80'` or `EOF`. They return pointers into one of **8 rotating static buffers** (`slot()` in `show.c`), so a single message can contain several of them: `doing("ft_strjoin(%s, %s)", show(a), show(b))` needs two buffers alive at the same time. With a single static buffer, the second call would overwrite the first.

### 5.4 The malloc tracker

**The trick.** Your libft and the tests are compiled with `-Dmalloc=tester_malloc -Dfree=tester_free`. `-D` defines a macro on the command line, as if each file started with `#define malloc tester_malloc`. The preprocessor runs before the compiler and replaces the *word* `malloc` everywhere, so when your `ft_strdup` calls `malloc(len + 1)`, it actually calls `tester_malloc(len + 1)`. Even `free` passed as a function pointer, as in `ft_lstclear(&list, free)`, becomes `tester_free`.

Three things make this work well:

- The framework itself is compiled **without** the macro, so inside `memory.c`, `malloc` is still the real one, and `tester_malloc` can use it.
- Only *your* code and the tests are redirected. The C library's own internal allocations (inside `printf`, for example) are untouched, so they never show up as leaks.
- It needs nothing special from the system: no valgrind, no linker tricks. It works with any C compiler.

**What `tester_malloc(size)` does:**

```
 the real block, from the real malloc(size + 16)
┌────────────────────────────────────┬──────────────────┐
│ size bytes for your code           │ 16 canary bytes  │
│ filled with 0xBE 0xBE 0xBE ...     │ 0xCA 0xCA ...    │
└────────────────────────────────────┴──────────────────┘
 ▲ returned to your code
```

and it records `{pointer, size, serial number, live}` in a table of up to 16384 blocks.

- **Garbage fill (0xBE).** Real `malloc` often hands out memory that happens to be zero, so forgetting the `'\0'` at the end of a string goes unnoticed: the next byte is already 0. Here it never is. A missing terminator shows as `got "hello\xbe\xbe..."`, and an `ft_calloc` that forgets to zero always fails.
- **The canary (0xCA).** Writing past the end of the block overwrites some canary bytes. They are checked when the block is freed and in `check_alloc()`. If the first byte after the block was changed to `'\0'`, that's the classic missing `+ 1`, and the message says so: `wrote a '\0' right after the end of a 5-byte malloc block: forgot the +1?`. The canary is repaired after being reported, so each overflow is reported once.

**What `tester_free(ptr)` does:**

| `ptr` is... | Result |
|---|---|
| `NULL` | nothing, like the real `free` |
| a live block | canary checked, block marked freed, filled with 0xDF |
| a block already freed | `double free of a 5-byte block` |
| inside a live block | `free() of a pointer into the middle of a 8-byte block` |
| anything else | `free() of a pointer malloc never returned` |

Freed blocks are not given back to the real `free` right away: they stay in the table, which is how a double free is recognised. This is called a **quarantine**. When freed blocks take more than 64 MB, or the table is full, they are really released.

**Leaks.** At the end of a case, if no check failed, `report_leaks()` lists the blocks still live: `leaked 6 bytes in 1 block: 6 bytes: "hello\0"`. The beginning of the content usually tells you which allocation it was. Leaks aren't reported when a check already failed, because a test that found a broken result often can't free it properly, and the leak would only be noise.

**`check_alloc(p, need, what)`** verifies that `p` is the start of a live block of at least `need` bytes. A smaller block is a failure (`malloc'd 10 bytes, needs 12`). A bigger one is only a warning (`malloc'd 101 bytes, 4 would be enough`), because the subject doesn't forbid it, but it usually means a size computed wrong (`ft_substr("hello", 2, 100)` allocating `len + 1`).

**Limits.** Memory from libc functions that allocate internally (the real `strdup`, `calloc`...) isn't tracked; freeing it with the tracked `free` is reported as "never returned by malloc". The subject only allows `malloc` and `free` in your libft, so this only matters for test code, which never uses them.

### 5.5 Malloc failure sweeps

`malloc` can return NULL, and evaluators check that your functions handle it: return NULL and free what was already allocated. `check_malloc_failures(fn, arg, release, null_required)` tests every possible failure point:

1. Call `fn(arg)` once normally, and count how many times it called `malloc`: say `ft_split("  hello wonderful world  ", ' ')` makes 4 calls (the array and 3 words). Release the result.
2. For `k` = 1, 2, 3, 4: arrange for the `k`-th malloc call to return NULL (`g_mem.fail_at`), call `fn(arg)` again, and check that:
   - the result is NULL (when `null_required`);
   - every block allocated during the call has been freed (**leak** after the failure);
   - no block that existed before the call was freed (**freed memory it didn't allocate**).

How do we know which blocks were allocated during the call? Every block gets an increasing **serial number**. Before the call, the sweep remembers the current serial; after it, any live block with a higher serial was allocated by this call and not freed. Those blocks are reported, then forgotten, so the next iteration starts clean.

While it runs, the sweep updates `doing` to say which malloc is failing: `ft_split(...) with malloc #3 of 4 failing`. If your function crashes because it used a NULL pointer, the crash report tells you exactly which allocation it didn't check.

`null_required` is 0 only for `ft_lstmap` with an allocating `f`: if `f` itself fails and returns NULL, the subject doesn't say whether `ft_lstmap` must stop or store a NULL content. Either is accepted, as long as nothing leaks.

### 5.6 Guarded buffers

Reading one byte past the end of a buffer usually *doesn't* crash: the next byte belongs to something else, and you read garbage. That makes "reads past the end" bugs invisible. Guarded buffers make them crash, every time.

Memory is managed in **pages** (4096 bytes on most systems), and each page has access rights. `mprotect(page, size, PROT_NONE)` removes all of them: any read or write to that page kills the process with `SIGSEGV`. `guard_after(src, n)` maps fresh pages with `mmap`, protects the last one, and places the `n` bytes right against it:

```
 guard_after("abc\0", 4)

           accessible page                     PROT_NONE page
 ┌─────────────────────────────────────┬──────────────────────────┐
 │ ...unused...           a  b  c  \0  │ any access → SIGSEGV      │
 └─────────────────────────────────────┴──────────────────────────┘
                          ▲ returned pointer
```

`guard_before(src, n)` does the opposite: the protected page comes first and the data starts right after it, which catches loops that walk backwards one step too far (`ft_strrchr` checking `s[-1]`). `guard_str(s)` is `guard_after(s, strlen(s) + 1)`.

The tests use them everywhere it matters:

- `ft_strlen(guard_str(s))`: can't read past the terminator.
- `ft_memcpy` with `src` and `dst` both guarded with exactly `n` bytes: one byte too many crashes.
- `ft_strlcpy` with a `dst` of exactly `size` bytes: writing past `size` crashes.
- `ft_bzero(guard_after(NULL, 0), 0)`: the pointer is *on* the protected page, so with `n = 0` the function must not touch it at all.
- `ft_substr(guard_str("hello"), 10, 3)`: a `start` past the end must not be read.

### 5.7 Output capture

To test `ft_putstr_fd(s, fd)` we need to see what it wrote. `capture()`:

1. creates two temporary files with `tmpfile()` (deleted automatically when closed);
2. saves the real standard output with `dup(1)`, then makes file descriptor 1 point to the second temporary file with `dup2`;
3. returns the first file's descriptor, which the test passes to your function.

`captured(&len)` puts standard output back, reads both files, and returns what was written to the fd. If anything landed in the second file, your function wrote to the screen instead of the fd it was given, and that's a failure.

### 5.8 Prototype checks

```c
PROTOTYPE(unsigned int, ft_lstsize, (t_list *));
```

expands to

```c
prototype("unsigned int ft_lstsize(t_list *)",
	__builtin_types_compatible_p(__typeof__(&ft_lstsize), unsigned int (*)(t_list *)));
```

- `#name` in a macro turns the argument into a string, so the expected prototype is written only once.
- `__typeof__(&ft_lstsize)` is the type of a pointer to *your* function, as declared in your `libft.h`.
- `__builtin_types_compatible_p(A, B)` is 1 when the two types are the same. It's evaluated by the compiler, so it costs nothing at run time.

Both are gcc and clang extensions. Parameter names don't matter, and neither do qualifiers that don't change the function's type (`int ft_isalpha(const int c)` is still `int (int)`). A mismatch is a warning, not a failure: the behaviour may be right while the prototype differs from the subject.

### 5.9 One case, from start to end

Here is `test("truncates", truncates)` in `ft_strlcpy_test.c`, with your `ft_strlcpy` returning the wrong value:

```
parent                                   child
──────                                   ─────
clear the shared report
fflush(stdout)
fork() ───────────────────────────────►  memory_reset(); alarm(5)
                                         truncates():
                                           dst = guard_after(NULL, 3)
                                           doing("ft_strlcpy(dst, \"hello\", 3)")
                                           ret = ft_strlcpy(dst, guard_str("hello"), 3)
                                           check_size(ret, 5, "return value")
                                             → failures = 1, text gets:
                                               ft_strlcpy(dst, "hello", 3)
                                                 return value: expected 5, got 2
                                         alarm(0)
waitpid() ◄──────────────────────────── _exit(EXIT_FAILED)
abnormal_end(status) → NULL (normal end)
failures = 1 → print:
  ✗ truncates
      ft_strlcpy(dst, "hello", 3)
        return value: expected 5, got 2
```

---

## 6. The tests: tests/

### 6.1 Anatomy of a test file

Every `tests/ft_name_test.c` follows the same shape:

1. A helper that tests **one call** completely: prepares guarded inputs, calls `doing()`, calls your function, checks everything about the result (value, memory, allocation size), frees what it got.
2. Small case functions that call the helper with groups of inputs: "empty string", "truncates", "bytes above 127"...
3. A malloc failure case for every function that allocates.
4. `main`: `suite()`, `PROTOTYPE()`, the list of `test()` calls, `return (done());`.

`ft_strlcpy_test.c` shows the pattern:

```c
static void	check_strlcpy(const char *src, size_t size)
{
	char	*dst;
	...
	dst = guard_after(NULL, size);			/* exactly size bytes, garbage */
	doing("ft_strlcpy(dst, %s, %zu)", show(src), size);
	check_size(ft_strlcpy(dst, guard_str(src), size), strlen(src), "return value");
	if (size == 0)
		return ;
	...build the expected bytes in want...
	check_mem(dst, want, copied + 1, "dst");
}

static void	truncates(void)
{
	check_strlcpy("hello", 5);
	check_strlcpy("hello", 3);
	...
}
```

### 6.2 Where expected values come from

- **The real libc**, when the function exists there and behaves like the subject: `strlen`, `memcmp`, `strchr`, `atoi`... The test compares your result with libc's on the same input.
- **A reference written in the test**, when glibc doesn't have the function (it's a BSD function): `ref_strlcat` in `ft_strlcat_test.c`, `ref_strnstr` in `ft_strnstr_test.c`. They follow the BSD man pages, which is what the subject refers to.
- **The subject itself**, when it's stricter than libc: the `is*` functions must return exactly 1 or 0 (libc's `isalpha` returns 1024 on glibc), and `ft_calloc(0, 0)` must return a unique pointer `free()` accepts.
- **Explicit expected values**, for the Part 2 and Part 3 functions, which have no libc version.

The "random" tests (`ft_atoi`, `ft_itoa`, `ft_strnstr`...) call `srand(42)` first, so they use the same inputs on every run: a failure you see once, you'll see again.

### 6.3 Part 3 tests and tests/lists.h

The list tests never use your `ft_lstnew` to build their lists: `lists.h` has its own `new_node()`, `new_list(n)` (nodes whose contents are malloc'd `"0"`, `"1"`...), `nth()`, `list_str()` (the contents joined by spaces, for messages: `"0 1 2"`) and `free_list()`. That way, a broken `ft_lstnew` only fails the `ft_lstnew` test, instead of every list test at once. The helpers are `static inline` so a file that doesn't use one of them gets no "unused function" warning.

---

## 7. Testing the tester: selftest/

A tester has bugs too, and a broken check is dangerous: it says ✓ while your code is wrong. `make selftest` runs `selftest/run.sh`, which needs no libft at all:

1. **Everything compiles cleanly.** The framework, the runner and every test are compiled with `-Werror`. The tests are compiled against `selftest/libft/libft.h`, which contains only the subject's prototypes and no code. So this step also checks that every test calls the functions the way the subject declares them.
2. **The framework catches what it should.** `selftest/framework_test.c` is a suite of cases that each do one bad thing on purpose: crash, loop forever, call `exit`, leak, forget the `+ 1`, free twice, read past a guarded buffer, leak after a malloc failure, write to stdout instead of the fd... It runs in both normal and quiet mode, and the output is compared with `selftest/expected.txt`, a file that was checked by hand. Any difference fails the self-test and shows the diff.

When you change a message on purpose, `UPDATE=1 make selftest` rewrites `expected.txt`. Read the diff (`git diff selftest/expected.txt`) before committing it: that diff *is* the review of your change.

`.github/workflows/selftest.yml` makes GitHub run `make selftest` with gcc and clang on every push and pull request, so a change that breaks the tester shows a red cross on GitHub before it reaches anyone.

**Mutation testing.** The self-test checks the framework, but not whether the 43 test files are strict enough. The way to check that is mutation testing: take a libft that passes, break one function on purpose (a `<=` instead of `<`, a forgotten `+ 1`, a missing `free`), and check that the tester fails it. When the tester was rewritten, 45 such broken versions were tried, and every one was caught by the case meant for it. If you write a new test, do the same: break the function the way a student would, and make sure your test notices.

---

## 8. Recipes

### 8.1 Add a test for a new function

1. Create `tests/ft_name_test.c`; start from the test of a similar function.
2. Add `{"ft_name", PART2, 0, 0, 0}` to the table in `runner/functions.c`, in the subject's order.
3. Run `libt ft_name` against a libft that has the function, then break the function to see your test fail (section 7).
4. Run `make selftest` before pushing.

The Makefile needs no change: it builds every `tests/*_test.c` it finds.

### 8.2 Add a case to an existing test

Write a `static void my_case(void)` that calls `doing()` before each call to the libft function and `check_*()` after it, and add `test("what it checks", my_case);` to `main`. Use guarded inputs (`guard_str`, `guard_after`) whenever the function reads or writes a buffer, and `check_alloc()` on anything it allocates.

### 8.3 Debug a crash with gdb

The test programs are built with `-g`, so they can run under gdb. One problem: each case runs in its own child process, and gdb follows a single process. `LIBT_NO_FORK=1` switches that off: the cases then run one after the other in the test program's own process, without the time limit, and the first crash stops everything, right where gdb can show it.

```sh
cd ~/Libft_Tester
LIBT_NO_FORK=1 gdb build/bin/ft_split
(gdb) run
```

gdb stops on the faulty line of your code; `bt` shows the chain of calls that led there. Run `libt ft_split` first, so the test program is up to date.

### 8.4 Other knobs

| You want | Do |
|---|---|
| a longer timeout | `LIBT_TIMEOUT=20 libt ...` |
| cases in a single process, for debuggers | `LIBT_NO_FORK=1 build/bin/ft_name` |
| no colours | `NO_COLOR=1 libt ...` |
| every case of every function | `libt -v all` |
| sanitizers | `libt --asan ...` |
| a clean rebuild | `make -C ~/Libft_Tester clean` |

---

## 9. Design decisions in one place

| Decision | Why | Alternative not taken |
|---|---|---|
| One process per case (`fork`) | A crash or infinite loop only ends that case, and each case starts from a clean state | Signal handlers with `setjmp`/`longjmp`: fragile, and memory can be left corrupted |
| Results through shared memory | They survive a crash of the child, and the parent reads them once, after the child ended | A pipe: the parent would have to keep reading while the child runs, or the child blocks when the pipe is full |
| malloc tracking with `-D` | Works with any compiler, no valgrind, no linker tricks; only the student's code is tracked | `--wrap=malloc` (GNU linker only), `LD_PRELOAD` (also catches libc's own allocations, and works differently on macOS), valgrind (slow, not always installed) |
| Guard pages | An out-of-bounds access crashes immediately, at the faulty line | Only comparing results: most one-byte over-reads give the right result by luck |
| One test program per function | You can rebuild and run one function in a fraction of a second; a function that doesn't build only removes its own test | One big program: a single compile error blocks everything |
| Tolerant build, logs per file | Lets you test while some functions are unfinished | Stopping at the first error |
| Content checksums for sources | Never test stale code, even with old file dates | Trusting dates only |
| `-Werror` for the libft only | The subject requires it; the tester's own warnings are caught by the self-test instead of breaking students' builds | `-Werror` everywhere |
| Prototype mismatch = warning | Behaviour can be right while the prototype differs | A failure: too harsh for a declaration detail |

---

## 10. Glossary

| Term | Meaning |
|---|---|
| `fork()` | Creates a copy of the current process. Returns 0 in the copy (the child) and the child's id in the original (the parent). |
| `execl(path, name, args..., NULL)` | Replaces the current program with another one; doesn't return unless it failed. |
| `waitpid(pid, &status, 0)` | Waits for a child to end and gives its exit status. |
| `WIFEXITED` / `WEXITSTATUS` | Did the child end with `exit`, and with which number. |
| `WIFSIGNALED` / `WTERMSIG` | Was the child killed by a signal, and which one. |
| `_exit(n)` | Ends the process immediately, without the cleanup `exit()` does. |
| signal | A notification the system sends a process: `SIGSEGV` (invalid memory access), `SIGALRM` (alarm), `SIGABRT` (abort). Unhandled, most of them kill the process. |
| `alarm(n)` | Sends `SIGALRM` to the process in `n` seconds; `alarm(0)` cancels it. |
| `mmap` | Asks the system for memory pages directly. `MAP_ANON`: not backed by a file. `MAP_SHARED`: shared with child processes after `fork`. |
| page | The unit the system manages memory in, usually 4096 bytes. Access rights are per page. |
| `mprotect(addr, len, PROT_NONE)` | Removes all access rights from pages: touching them causes `SIGSEGV`. |
| `dup(fd)` / `dup2(old, new)` | Copy a file descriptor; `dup2` makes `new` refer to the same file as `old`. |
| `tmpfile()` | Creates a temporary file that is deleted when closed. |
| `va_list`, `va_start`, `va_end` | How a function with `...` arguments reads them; `vsnprintf` takes a `va_list`. |
| `__attribute__((format(printf, 1, 2)))` | Tells the compiler a function takes a `printf` format, so it checks the arguments like it does for `printf`. |
| `-D name=value` | Defines a macro from the command line. |
| `-MMD -MP` | Makes the compiler write a `.d` file listing the headers each object depends on. |
| static archive (`.a`) | A bundle of object files made with `ar`. The linker only takes the members it needs. |
| `cksum` | Prints a checksum of a file's content: two files with the same content give the same number. |
| canary | Bytes with a known value placed after a block; if they change, something wrote past the end. |
| quarantine | Freed blocks kept for a while instead of being released, to recognise double frees. |
| golden file | A saved output that a test compares against (`selftest/expected.txt`). |
| mutation testing | Breaking working code on purpose to check that the tests notice. |
