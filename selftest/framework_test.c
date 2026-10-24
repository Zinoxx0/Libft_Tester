/*
** The framework's own test. Each case does one thing the framework must
** notice (or must accept); run.sh compares the output with expected.txt.
** No libft involved: the "functions under test" are written here.
*/

#include "tester.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Hidden from the compiler, so it can't warn about the deliberate bugs */
static volatile size_t	g_five = 5;

static void	checks_pass(void)
{
	char	buf[4];

	doing("comparing equal values");
	check(1, "never shown");
	check_int(-4, -4, "int");
	check_size(4, 4, "size");
	check_bool(42, 1, "bool");
	check_sign(-3, -1, "sign");
	check_str("abc", "abc", "str");
	check_str(NULL, NULL, "NULL str");
	check_mem("a\0b", "a\0b", 3, "mem");
	check_ptr(buf + 1, buf + 1, buf, "ptr");
}

static void	checks_fail(void)
{
	doing("comparing different values");
	check(0, "a %s message", "generic");
	check_int(-4, 4, "int");
	check_size(4, 5, "size");
	check_bool(0, 1, "bool");
	check_sign(0, -1, "sign");
	check_str("abc\n", NULL, "str");
}

static void	checks_fail_more(void)
{
	char	buf[4];

	doing("more different values");
	check_mem("a\0b", "a\0c", 3, "mem");
	check_ptr(NULL, buf + 2, buf, "ptr");
	check_ptr(buf + 1, buf + 2, buf, "ptr");
	check(0, "%s %s %s %s", show_char('a'), show_char('\n'), show_char(EOF),
		show_char(300));
	check(0, "%s", show_mem("\xff\t\"", 3));
	check(0, "%s", show(NULL));
	check(0, "past the limit of 6: only counted");
	check(0, "past the limit of 6: only counted");
}

static void	crash(void)
{
	doing("dereferencing NULL");
	*(volatile int *)NULL = 1;
}

static void	infinite_loop(void)
{
	doing("looping forever");
	while (1)
		;
}

static void	calls_exit(void)
{
	exit(3);
}

static void	leak(void)
{
	char	*p;

	p = malloc(6);
	memcpy(p, "hello", 6);
}

static void	missing_plus_one(void)
{
	char	*p;

	p = malloc(g_five);
	memcpy(p, "hello", 6);
	check_alloc(p, 5, "p");
	free(p);
}

static void	overflow(void)
{
	char	*p;

	p = malloc(g_five);
	memcpy(p, "hello!", 6);
	free(p);
}

static void	double_free(void)
{
	char	*p;

	p = malloc(5);
	free(p);
	free(p);
}

static void	bad_frees(void)
{
	char	stack[4];
	char	*p;

	free(stack);
	p = malloc(8);
	free(p + 2);
	free(p);
	free(NULL);
}

static void	garbage_fill(void)
{
	unsigned char	*p;

	p = malloc(4);
	check(p[0] == 0xBE && p[3] == 0xBE, "fresh blocks aren't filled with 0xBE");
	free(p);
}

static void	alloc_sizes(void)
{
	char	*p;
	char	stack[4];

	p = malloc(10);
	check_alloc(p, 10, "exact");
	check_alloc(p, 4, "bigger");
	check_alloc(p, 12, "smaller");
	check_alloc(stack, 1, "stack");
	check_alloc(p + 1, 1, "inside");
	free(p);
}

static void	guards_in_bounds(void)
{
	char	*after;
	char	*before;

	after = guard_str("abc");
	before = guard_before("xyz", 3);
	check(after[3] == '\0' && before[0] == 'x' && before[2] == 'z',
		"guarded copies are wrong");
}

static void	guard_after_overrun(void)
{
	char	*s;

	s = guard_str("abc");
	doing("reading one byte past a guarded string");
	check(((volatile char *)s)[4] == 0, "unreachable");
}

static void	guard_before_underrun(void)
{
	char	*s;

	s = guard_before("abc", 4);
	doing("reading one byte before a guarded string");
	check(((volatile char *)s)[-1] == 0, "unreachable");
}

/* Three mallocs, every failure handled */
static void	*careful(void *arg)
{
	char	**words;

	(void)arg;
	words = malloc(3 * sizeof(char *));
	if (!words)
		return (NULL);
	words[0] = malloc(2);
	words[1] = words[0] ? malloc(2) : NULL;
	if (!words[1])
	{
		free(words[0]);
		free(words);
		return (NULL);
	}
	words[2] = NULL;
	return (words);
}

/* Forgets the first word when the second malloc fails */
static void	*leaky(void *arg)
{
	char	**words;

	(void)arg;
	words = malloc(3 * sizeof(char *));
	if (!words)
		return (NULL);
	words[0] = malloc(2);
	words[1] = words[0] ? malloc(2) : NULL;
	if (!words[1])
	{
		free(words);
		return (NULL);
	}
	words[2] = NULL;
	return (words);
}

/* Never checks malloc */
static void	*reckless(void *arg)
{
	char	*s;

	(void)arg;
	s = malloc(4);
	s[0] = 'x';
	return (s);
}

static void	release_words(void *p)
{
	char	**words;

	words = p;
	free(words[0]);
	free(words[1]);
	free(words);
}

static void	malloc_failures_handled(void)
{
	doing("careful()");
	check_malloc_failures(careful, NULL, release_words, 1);
}

static void	malloc_failures_leak(void)
{
	doing("leaky()");
	check_malloc_failures(leaky, NULL, release_words, 1);
}

static void	malloc_failures_crash(void)
{
	doing("reckless()");
	check_malloc_failures(reckless, NULL, free, 1);
}

static void	capture_fd(void)
{
	const char	*out;
	size_t		len;
	int			fd;

	fd = capture();
	write(fd, "to fd", 5);
	out = captured(&len);
	check_size(len, 5, "length");
	check_str(out, "to fd", "output");
}

static void	capture_stdout(void)
{
	size_t	len;

	capture();
	write(STDOUT_FILENO, "oops\n", 5);
	captured(&len);
}

/* Not static: clang calls a function only used in __typeof__ unused */
int	sample(char *s)
{
	return (s != NULL);
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "framework");
	PROTOTYPE(int, sample, (char *));
	PROTOTYPE(int, sample, (const char *));
	test("checks that pass", checks_pass);
	test("checks that fail", checks_fail);
	test("more checks that fail, past the display limit", checks_fail_more);
	test("a crash", crash);
	test("an infinite loop", infinite_loop);
	test("a call to exit()", calls_exit);
	test("a leak", leak);
	test("a missing +1", missing_plus_one);
	test("an overflow", overflow);
	test("a double free", double_free);
	test("frees of pointers malloc never returned", bad_frees);
	test("fresh blocks are filled with garbage", garbage_fill);
	test("block sizes", alloc_sizes);
	test("guarded buffers, in bounds", guards_in_bounds);
	test("reading past a guarded buffer", guard_after_overrun);
	test("reading before a guarded buffer", guard_before_underrun);
	test("malloc failures, handled", malloc_failures_handled);
	test("malloc failures, leaking", malloc_failures_leak);
	test("malloc failures, not checked", malloc_failures_crash);
	test("output to the fd", capture_fd);
	test("output to stdout instead", capture_stdout);
	return (done());
}
