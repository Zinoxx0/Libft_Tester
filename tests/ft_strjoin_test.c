#include "libft.h"
#include "tester.h"
#include <stdio.h>
#include <string.h>

static void	check_strjoin(const char *s1, const char *s2)
{
	static char	want[20001];
	char		*joined;

	snprintf(want, sizeof(want), "%s%s", s1, s2);
	doing("ft_strjoin(%s, %s)", show(s1), show(s2));
	joined = ft_strjoin(guard_str(s1), guard_str(s2));
	if (!check_str(joined, want, "result") || !joined)
		return ;
	check_alloc(joined, strlen(want) + 1, "the result");
	free(joined);
}

static void	two_words(void)
{
	check_strjoin("hello", " world");
	check_strjoin("42", "Lisboa");
}

static void	empty(void)
{
	check_strjoin("", "libft");
	check_strjoin("libft", "");
	check_strjoin("", "");
}

static void	high_bytes(void)
{
	check_strjoin("caf\xc3\xa9", "\xff\x80");
}

static void	long_strings(void)
{
	static char	a[5001];
	static char	b[5001];

	memset(a, 'a', sizeof(a) - 1);
	memset(b, 'b', sizeof(b) - 1);
	check_strjoin(a, b);
}

static void	*call_strjoin(void *arg)
{
	(void)arg;
	return (ft_strjoin("hello", " world"));
}

static void	malloc_fails(void)
{
	doing("ft_strjoin(\"hello\", \" world\")");
	check_malloc_failures(call_strjoin, NULL, free, 1);
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_strjoin");
	PROTOTYPE(char *, ft_strjoin, (char const *, char const *));
	test("joins two strings", two_words);
	test("empty strings", empty);
	test("bytes above 127", high_bytes);
	test("long strings (5000 + 5000 chars)", long_strings);
	test("returns NULL when malloc fails", malloc_fails);
	return (done());
}
