#include "libft.h"
#include "tester.h"
#include <string.h>

static void	check_strdup(const char *str)
{
	char	*s;
	char	*copy;

	s = guard_str(str);
	doing("ft_strdup(%s)", show(str));
	copy = ft_strdup(s);
	if (!check_str(copy, str, "result") || !copy)
		return ;
	if (check(copy != s, "returned s itself, not a copy"))
		check_alloc(copy, strlen(str) + 1, "the result");
	free(copy);
}

static void	simple(void)
{
	check_strdup("hello");
	check_strdup("42 Lisboa");
}

static void	empty(void)
{
	check_strdup("");
}

static void	high_bytes(void)
{
	check_strdup("caf\xc3\xa9 \xff\x80");
}

static void	long_string(void)
{
	static char	s[10001];

	memset(s, 'x', sizeof(s) - 1);
	check_strdup(s);
}

static void	*call_strdup(void *arg)
{
	return (ft_strdup(arg));
}

static void	malloc_fails(void)
{
	doing("ft_strdup(\"hello\")");
	check_malloc_failures(call_strdup, "hello", free, 1);
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_strdup");
	PROTOTYPE(char *, ft_strdup, (const char *));
	test("copies the string into a new block", simple);
	test("empty string", empty);
	test("bytes above 127", high_bytes);
	test("long string (10000 chars)", long_string);
	test("returns NULL when malloc fails", malloc_fails);
	return (done());
}
