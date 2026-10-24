/*
** The check_*() functions: each compares one value and records a failure
** describing it when it's wrong.
*/

#include "internal.h"

#include <stdio.h>
#include <string.h>

int	check(int ok, const char *fmt, ...)
{
	va_list	ap;

	if (ok)
		return (1);
	va_start(ap, fmt);
	vfailure(fmt, ap);
	va_end(ap);
	return (0);
}

int	check_int(long got, long want, const char *what)
{
	return (check(got == want, "%s: expected %ld, got %ld", what, want, got));
}

int	check_size(size_t got, size_t want, const char *what)
{
	return (check(got == want, "%s: expected %zu, got %zu", what, want, got));
}

int	check_bool(int got, int want, const char *what)
{
	if (!got == !want)
		return (1);
	if (want)
		return (check(0, "%s: expected true (non-zero), got 0", what));
	return (check(0, "%s: expected false (0), got %d", what, got));
}

static const char	*sign_name(long n)
{
	if (n < 0)
		return ("a negative value");
	if (n > 0)
		return ("a positive value");
	return ("0");
}

int	check_sign(long got, long want, const char *what)
{
	if ((got < 0) == (want < 0) && (got > 0) == (want > 0))
		return (1);
	return (check(0, "%s: expected %s, got %ld", what, sign_name(want), got));
}

int	check_str(const char *got, const char *want, const char *what)
{
	if (got == want || (got && want && strcmp(got, want) == 0))
		return (1);
	return (check(0, "%s: expected %s, got %s", what, show(want), show(got)));
}

int	check_mem(const void *got, const void *want, size_t n, const char *what)
{
	const unsigned char	*g;
	const unsigned char	*w;
	size_t				i;

	g = got;
	w = want;
	i = 0;
	while (i < n && g[i] == w[i])
		i++;
	if (i == n)
		return (1);
	if (n <= 40)
		return (check(0, "%s: expected %s, got %s", what, show_mem(w, n),
				show_mem(g, n)));
	return (check(0, "%s: byte %zu is 0x%02x, expected 0x%02x", what, i,
			g[i], w[i]));
}

static const char	*where(const void *p, const void *base)
{
	char	*out;

	if (!p)
		return ("NULL");
	out = slot();
	sprintf(out, "s + %td", (const char *)p - (const char *)base);
	return (out);
}

int	check_ptr(const void *got, const void *want, const void *base,
		const char *what)
{
	if (got == want)
		return (1);
	return (check(0, "%s: expected %s, got %s", what, where(want, base),
			where(got, base)));
}
