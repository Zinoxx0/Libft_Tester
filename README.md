# Libft Tester

A fast, terminal-friendly tester for the **42 libft** project (subject v19.3: Part 1, Part 2 and the Part 3 linked lists).

Point it at your libft, and it builds one small test program per function, runs them, and tells you exactly which call failed and why. It also catches the problems that usually only show up during evaluation: crashes, infinite loops, leaks, writing one byte past a `malloc` block, and functions that fall over when `malloc` fails.

> Made for the 42 community by **samercad** (42 Intra).

## Installation

```sh
cd ~
git clone git@github.com:Zinoxx0/Libft_Tester.git
chmod +x ~/Libft_Tester/libt.sh
alias libt='~/Libft_Tester/libt.sh'    # add this line to your ~/.zshrc or ~/.bashrc
```

## Usage

Run it from the directory that holds your `ft_*.c` files and `libft.h`:

```sh
libt                 # interactive menu
libt all             # every function
libt part1           # one part of the subject: part1, part2, part3 (or p1, p2, p3)
libt strlen split.c  # some functions; the ft_ prefix and .c are optional
libt -v all          # show every case, not only the failures
libt --asan all      # also build with AddressSanitizer and UBSan
```

`libt` exits with 0 when everything passed, so it also works in scripts.

Running a single function shows every case. Running a group shows one line per function, and the details only for failures:

```
 Part 2
  ✓ ft_substr
  ✗ ft_itoa            6/7 cases passed
      ✗ INT_MAX and INT_MIN
          ft_itoa(-2147483648)
            result: expected "-2147483648", got "-"
```

## What it checks

Every function is compared with the subject and with the real libc (or the BSD behaviour for `strlcpy`, `strlcat` and `strnstr`, which glibc doesn't have), over edge cases and hundreds of generated inputs. On top of the results, it checks:

- **Crashes and infinite loops.** Each case runs in its own process with a 5-second limit (`LIBT_TIMEOUT` changes it). A segfault is reported along with the call that caused it, and the remaining cases still run.
- **Memory.** Every `malloc` and `free` in your code goes through the tester. It reports leaks, double frees, `free` on pointers `malloc` never returned, and writes past the end of a block (the classic missing `+ 1` for the `'\0'`). Fresh blocks are filled with garbage, so a missing terminator or a `calloc` that doesn't zero always shows. It also warns when a result takes more memory than it needs.
- **Malloc failures.** For every function that allocates, each `malloc` it makes is failed in turn. The function must return `NULL` without leaking: `ft_split` has to free the words it already made, and `ft_lstmap` has to free its new list.
- **Out-of-bounds access.** Buffers are placed right against memory that can't be touched, so reading or writing even one byte too far (past `n` in `ft_memcpy`, past `size` in `ft_strlcpy`, before the start in `ft_strrchr`...) crashes instead of silently passing.
- **File descriptors.** The `ft_put*_fd` output is captured and compared byte for byte, including writing to stdout instead of the given fd.
- **Prototypes.** Each function's prototype is compared with the subject's; a mismatch shows as a warning.

## Good to know

- A file that doesn't compile is reported and skipped: the other functions are still tested. Select it in the menu, or run `libt ft_name`, to see the compiler errors.
- Missing functions are shown with a `·` in the menu, so you can track what's left to write.
- Your files are compiled with `-Wall -Wextra -Werror` into `build/` inside this repository; nothing is written to your project.
- The tester needs a Unix system with `bash`, `make` and `cc` (gcc or clang). It is developed on Linux.
- Passing every test is a good sign, but it doesn't replace the norm check, peer evaluation, or your own tests.

## How it is organised

```
libt.sh              launcher: builds the tests against your directory, then runs them
Makefile             builds the runner, your objects, and one test binary per function
runner.c             menu and command line; runs the test binaries
framework/tester.[ch]  checks, process isolation, malloc tracking, guarded buffers, fd capture
tests/ft_*_test.c    one test program per function
tests/lists.h        list helpers shared by the Part 3 tests
```

Adding a test means writing `tests/ft_name_test.c` (copy one of the existing files: a list of `test("title", function)` cases, each using `doing(...)` and `check_*()` from `framework/tester.h`), then adding the name to the table at the top of `runner.c`.

Have fun, read the failures carefully, and remember: every tiny function is one more brick in your libft.

**Good luck at 42.**
