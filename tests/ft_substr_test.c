#include "libft.h"
#include "tester.h"
#include <limits.h>
#include <stdint.h>
#include <string.h>

static void	check_substr(const char *str, unsigned int start, size_t len,
		const char *want)
{
	char	*sub;

	doing("ft_substr(%s, %u, %zu)", show(str), start, len);
	sub = ft_substr(guard_str(str), start, len);
	if (!check_str(sub, want, "result") || !sub)
		return ;
	check_alloc(sub, strlen(want) + 1, "the result");
	free(sub);
}

static void	middle(void)
{
	check_substr("hello world", 6, 5, "world");
	check_substr("hello", 1, 3, "ell");
	check_substr("hello", 0, 1, "h");
}

static void	whole(void)
{
	check_substr("hello", 0, 5, "hello");
}

static void	len_too_long(void)
{
	check_substr("hello", 2, 100, "llo");
	check_substr("hello", 0, SIZE_MAX, "hello");
	check_substr("hello", 4, SIZE_MAX, "o");
}

static void	len_zero(void)
{
	check_substr("hello", 2, 0, "");
	check_substr("hello", 0, 0, "");
}

static void	start_at_end(void)
{
	check_substr("hello", 5, 3, "");
	check_substr("", 0, 5, "");
}

static void	start_past_end(void)
{
	check_substr("hello", 10, 3, "");
	check_substr("hello", 6, 0, "");
	check_substr("hello", UINT_MAX, 5, "");
	check_substr("", 1, 1, "");
}

static void	*call_substr(void *arg)
{
	return (ft_substr(arg, 2, 5));
}

static void	malloc_fails(void)
{
	doing("ft_substr(\"hello world\", 2, 5)");
	check_malloc_failures(call_substr, "hello world", free, 1);
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_substr");
	PROTOTYPE(char *, ft_substr, (char const *, unsigned int, size_t));
	test("substring in the middle", middle);
	test("the whole string", whole);
	test("len past the end of s is cut (and doesn't overflow)",
		len_too_long);
	test("len 0 gives an empty string", len_zero);
	test("start at the end gives an empty string", start_at_end);
	test("start past the end gives an empty string, without reading there",
		start_past_end);
	test("returns NULL when malloc fails", malloc_fails);
	return (done());
}
