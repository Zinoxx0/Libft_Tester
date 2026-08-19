# Libft Tester

A small, terminal-friendly test bench for the **42 libft project**.

This tester was built to make the first round of libft debugging less repetitive: point it at your project, let it compile the files it finds, then run one function, an entire group, or everything from a simple menu.

> Made for the 42 community by **samercad** (42 Intra).

## What it does

`libt` is a launcher around the tester. When you run it from your libft directory, it:

1. Looks for `.c` files in the current directory.
2. Compiles those files with `-Wall -Wextra -Werror -g`.
3. Builds only the tests matching the source files that exist.
4. Starts the interactive tester or runs the command you provided.
5. Keeps the generated objects and test binaries inside this repository, not in your libft project.

The tests are deliberately independent: you can fix one function and immediately rerun only that function instead of waiting through the whole suite.

## Installation

Clone the repository in your home directory:

```sh
cd ~
git clone git@github.com:Zinoxx0/Libft_Tester.git
```

Make the launcher executable:

```sh
chmod +x ~/Libft_Tester/libt.sh
```

Create the `libt` alias for your shell:

```sh
# bash
alias libt='bash \"$HOME/Libft_Tester/libt.sh\"' >> ~/.bashrc
source ~/.bashrc

# zsh
alias libt='bash \"$HOME/Libft_Tester/libt.sh\"' >> ~/.zshrc
source ~/.zshrc
```

## Basic use

Go to the directory containing your libft source files, then run:

```sh
libt
```

You can also skip the menu and run a specific test or group directly:

```sh
libt ft_strlen.c
libt -a "for all files available"
```

## What counts as a test?

A test is available when both of these exist:

- a tester such as `tests/ft_strlen_tester.c` in this repository;
- the matching source file, such as `ft_strlen.c`, in your current libft directory.

## Before you start

When launching `libt`, the launcher scans only the current directory, not nested folders.

The tester expects a Unix-like environment with:

- `bash`
- `make`
- a C compiler available as `cc`
- standard POSIX tools and functions such as `fork`, `waitpid`, and `mkstemp`

## A few things to keep in mind

- Run `libt` **from your libft directory**, not from `~/Libft_Tester`.
- The tester links your available source files into each matching test binary. A broken shared function can therefore make several tests fail.
- Passing these tests is useful feedback, but it is not a replacement for the official 42 evaluation, peer review, norm checks, memory checks, or careful edge-case testing.
- Tests write temporary captured output under `/tmp` and remove it after each run.

## Build it manually

Normally `libt` handles this for you. For development or troubleshooting, from this repository you can run:

```sh
make SRC_DIR=/absolute/path/to/your/libft
make clean
make fclean
```

Have fun, read the failures carefully, and remember: every tiny function is one more brick in your libft.

**Good luck at 42.**
