#include "libft.h"
#include "tester.h"
#include <string.h>

static void	check_strtrim(const char *s1, const char *set, const char *want)
{
	char	*trimmed;

	doing("ft_strtrim(%s, %s)", show(s1), show(set));
	trimmed = ft_strtrim(guard_str(s1), guard_str(set));
	if (!check_str(trimmed, want, "result") || !trimmed)
		return ;
	check_alloc(trimmed, strlen(want) + 1, "the result");
	free(trimmed);
}

static void	both_ends(void)
{
	check_strtrim("  hello  ", " ", "hello");
	check_strtrim("xxhixx", "x", "hi");
}

static void	several_chars(void)
{
	check_strtrim(" \t\nhello world\n\t ", " \n\t", "hello world");
	check_strtrim("abcHELLOcba", "abc", "HELLO");
	check_strtrim("aaabbbcccXcccbbbaaa", "cba", "X");
}

static void	keeps_middle(void)
{
	check_strtrim("ab-c-ba", "ab", "-c-");
	check_strtrim("  a b  ", " ", "a b");
}

static void	one_side(void)
{
	check_strtrim("hello  ", " ", "hello");
	check_strtrim("  hello", " ", "hello");
}

static void	nothing_to_trim(void)
{
	check_strtrim("hello", " ", "hello");
	check_strtrim("hello", "", "hello");
	check_strtrim(" hello ", "", " hello ");
}

static void	everything(void)
{
	check_strtrim("aaaa", "a", "");
	check_strtrim("   ", " ", "");
	check_strtrim("abcabc", "cba", "");
	check_strtrim("", "abc", "");
	check_strtrim("", "", "");
}

static void	*call_strtrim(void *arg)
{
	return (ft_strtrim(arg, " "));
}

static void	malloc_fails(void)
{
	doing("ft_strtrim(\"  hello  \", \" \")");
	check_malloc_failures(call_strtrim, "  hello  ", free, 1);
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_strtrim");
	PROTOTYPE(char *, ft_strtrim, (char const *, char const *));
	test("trims both ends", both_ends);
	test("set with several characters", several_chars);
	test("keeps set characters in the middle", keeps_middle);
	test("only one end to trim", one_side);
	test("nothing to trim, or empty set", nothing_to_trim);
	test("everything trimmed gives an empty string", everything);
	test("returns NULL when malloc fails", malloc_fails);
	return (done());
}
