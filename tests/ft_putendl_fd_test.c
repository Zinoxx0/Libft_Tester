#include "libft.h"
#include "tester.h"
#include <stdio.h>
#include <string.h>

static void	check_putendl(const char *s)
{
	static char	want[10002];
	const char	*out;
	size_t		len;
	int			fd;

	snprintf(want, sizeof(want), "%s\n", s);
	fd = capture();
	doing("ft_putendl_fd(%s, fd)", show(s));
	ft_putendl_fd(guard_str(s), fd);
	out = captured(&len);
	if (check_size(len, strlen(want), "bytes written"))
		check_mem(out, want, len, "written");
}

static void	simple(void)
{
	check_putendl("hello");
	check_putendl("42 Lisboa\twith\ttabs");
}

static void	empty(void)
{
	check_putendl("");
}

static void	high_bytes(void)
{
	check_putendl("caf\xc3\xa9 \xff\x80");
}

static void	long_string(void)
{
	static char	s[10001];

	memset(s, 'x', sizeof(s) - 1);
	check_putendl(s);
}

static void	bad_fd(void)
{
	size_t	len;

	capture();
	doing("ft_putendl_fd(\"hello\", -1)");
	ft_putendl_fd("hello", -1);
	captured(&len);
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_putendl_fd");
	PROTOTYPE(void, ft_putendl_fd, (char *, int));
	test("writes the string and a newline to fd", simple);
	test("empty string writes just the newline", empty);
	test("bytes above 127", high_bytes);
	test("long string (10000 chars)", long_string);
	test("fd -1 doesn't crash", bad_fd);
	return (done());
}
