#ifndef TESTER_H
# define TESTER_H

/*
** Tiny test framework shared by every tests/ft_*_test.c file.
**
** A test file is a list of cases:
**
**	static void	empty_string(void)
**	{
**		doing("ft_strlen(\"\")");
**		check_size(ft_strlen(""), 0, "return value");
**	}
**
**	int	main(int argc, char **argv)
**	{
**		suite(argc, argv, "ft_strlen");
**		test("empty string", empty_string);
**		return (done());
**	}
**
** Each case runs in its own child process with a time limit, so a crash or
** an infinite loop is reported and the remaining cases still run. At the end
** of every case the framework also reports leaked memory.
**
** The libft sources and the tests are compiled with -Dmalloc=tester_malloc
** -Dfree=tester_free: every block is tracked, filled with garbage on
** allocation and followed by a canary, so missing '\0's, writes past the end,
** double frees and leaks are all caught without valgrind.
*/

# include <stddef.h>

/* Suite: one per test binary */
void		suite(int argc, char **argv, const char *name);
void		test(const char *title, void (*fn)(void));
int			done(void);

/*
** Compares the libft prototype with the one in the subject. A mismatch is
** reported as a warning, not a failure.
*/
# define PROTOTYPE(ret, name, params) prototype(#ret " " #name #params, \
	__builtin_types_compatible_p(__typeof__(&name), ret (*)params))

void		prototype(const char *subject, int matches);

/*
** Describes the call being tested. It heads the failures reported after it,
** and is shown if the case crashes or times out.
*/
void		doing(const char *fmt, ...)
			__attribute__((format(printf, 1, 2)));

/* Checks: each one returns 1 when it passes */
int			check(int ok, const char *fmt, ...)
			__attribute__((format(printf, 2, 3)));
int			check_int(long got, long want, const char *what);
int			check_size(size_t got, size_t want, const char *what);
int			check_bool(int got, int want, const char *what);
int			check_sign(long got, long want, const char *what);
int			check_str(const char *got, const char *want, const char *what);
int			check_mem(const void *got, const void *want, size_t n,
				const char *what);
int			check_ptr(const void *got, const void *want, const void *base,
				const char *what);
void		warn(const char *fmt, ...)
			__attribute__((format(printf, 1, 2)));

/*
** Memory tracking. check_alloc() passes when p is the start of a live malloc
** block of at least need bytes, and warns when the block is bigger.
*/
void		*tester_malloc(size_t size);
void		tester_free(void *ptr);
int			check_alloc(const void *p, size_t need, const char *what);
size_t		live_blocks(void);

/*
** Calls fn(arg) once to count its mallocs, then once more for each of them
** with that malloc failing. Every time, the call must clean up after itself;
** when null_required, it must also return NULL. Non-NULL results are passed
** to release().
*/
void		check_malloc_failures(void *(*fn)(void *), void *arg,
				void (*release)(void *), int null_required);

/*
** Guarded copies: the n bytes of src (or uninitialised bytes when src is
** NULL) placed right against a page that can't be touched, so reading or
** writing one byte too far crashes. guard_after puts the page after the
** data, guard_before before it.
*/
void		*guard_after(const void *src, size_t n);
void		*guard_before(const void *src, size_t n);
char		*guard_str(const char *s);

/*
** Output capture for the ft_put*_fd functions: capture() returns an fd to
** write to, captured() returns what was written to it and fails the check if
** anything was written to stdout instead.
*/
int			capture(void);
const char	*captured(size_t *len);

/* Printable versions of values, for messages (rotating static buffers) */
const char	*show(const char *s);
const char	*show_mem(const void *p, size_t n);
const char	*show_char(int c);

#endif
