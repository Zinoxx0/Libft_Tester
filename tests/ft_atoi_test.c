#include "libft.h"
#include "tester.h"
#include <stdio.h>
#include <stdlib.h>

static void	check_atoi(const char *s)
{
	doing("ft_atoi(%s)", show(s));
	check_int(ft_atoi(guard_str(s)), atoi(s), "return value");
}

static void	plain(void)
{
	check_atoi("0");
	check_atoi("42");
	check_atoi("123456");
	check_atoi("007");
}

static void	signs(void)
{
	check_atoi("-42");
	check_atoi("+42");
	check_atoi("-0");
	check_atoi("+0");
}

static void	whitespace(void)
{
	check_atoi("   42");
	check_atoi("\t\n\v\f\r 42");
	check_atoi(" \t -42");
	check_atoi("\n+42");
}

static void	stops(void)
{
	check_atoi("42abc");
	check_atoi("4 2");
	check_atoi("42\n");
	check_atoi("12-3");
	check_atoi("1+2");
}

static void	no_number(void)
{
	check_atoi("");
	check_atoi("   ");
	check_atoi("abc");
	check_atoi("a42");
	check_atoi("-");
	check_atoi("+");
	check_atoi(" - 42");
}

static void	one_sign(void)
{
	check_atoi("--42");
	check_atoi("+-42");
	check_atoi("-+42");
	check_atoi("++42");
}

static void	not_spaces(void)
{
	check_atoi("\x01" "42");
	check_atoi("\x1b" "42");
	check_atoi("\x7f" "42");
	check_atoi("\xa0" "42");
	check_atoi("_42");
}

static void	limits(void)
{
	check_atoi("2147483647");
	check_atoi("-2147483648");
	check_atoi("+2147483647");
	check_atoi("   -2147483648xyz");
}

static void	random_numbers(void)
{
	const char	*prefixes[] = {"", " ", "\t", "\n\v", "  +", " -", "-", "+"};
	char		s[64];
	int			i;

	srand(42);
	i = 0;
	while (i++ < 1000)
	{
		snprintf(s, sizeof(s), "%s%d", prefixes[rand() % 8],
			rand() % 2 ? rand() : rand() % 1000);
		check_atoi(s);
	}
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_atoi");
	PROTOTYPE(int, ft_atoi, (const char *));
	test("plain numbers", plain);
	test("one sign", signs);
	test("leading whitespace (\" \\t\\n\\v\\f\\r\")", whitespace);
	test("stops at the first non-digit", stops);
	test("no number gives 0", no_number);
	test("two signs give 0", one_sign);
	test("other characters aren't whitespace", not_spaces);
	test("INT_MAX and INT_MIN", limits);
	test("1000 random numbers", random_numbers);
	return (done());
}
