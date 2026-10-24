#include "libft.h"
#include "tester.h"
#include <string.h>

/*
** Runs the search twice: once with a protected page right after the string,
** once with one right before it (for loops that walk backwards too far).
*/
static void	check_strrchr(const char *str, int c)
{
	char	*s;

	s = guard_str(str);
	doing("ft_strrchr(%s, %s)", show(str), show_char(c));
	check_ptr(ft_strrchr(s, c), strrchr(s, c), s, "return value");
	s = guard_before(str, strlen(str) + 1);
	check_ptr(ft_strrchr(s, c), strrchr(s, c), s, "return value");
}

static void	finds_last(void)
{
	check_strrchr("hello", 'l');
	check_strrchr("banana", 'a');
	check_strrchr("hello", 'o');
}

static void	first_char(void)
{
	check_strrchr("hello", 'h');
	check_strrchr("a", 'a');
	check_strrchr("abcd", 'a');
}

static void	not_found(void)
{
	check_strrchr("hello", 'z');
	check_strrchr("", 'a');
}

static void	terminator(void)
{
	check_strrchr("hello", '\0');
	check_strrchr("", '\0');
}

static void	char_conversion(void)
{
	check_strrchr("hello", 'l' + 256);
	check_strrchr("hello", 256);
	check_strrchr("\xe9t\xe9", 0xe9);
	check_strrchr("\xe9t\xe9", (char)0xe9);
	check_strrchr("a\xff" "b\xff", -1);
}

static void	long_string(void)
{
	static char	s[10001];

	memset(s, 'x', sizeof(s) - 1);
	s[0] = 'y';
	check_strrchr(s, 'y');
	s[5000] = 'y';
	check_strrchr(s, 'y');
	check_strrchr(s, 'z');
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_strrchr");
	PROTOTYPE(char *, ft_strrchr, (const char *, int));
	test("finds the last occurrence", finds_last);
	test("finds a match at index 0", first_char);
	test("returns NULL when c isn't there", not_found);
	test("c = '\\0' finds the terminator", terminator);
	test("c is converted to char", char_conversion);
	test("long string (10000 chars)", long_string);
	return (done());
}
