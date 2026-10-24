#include "libft.h"
#include "tester.h"
#include <string.h>

static unsigned int	g_calls;
static unsigned int	g_indices[64];
static char			*g_ptrs[64];

static void	upper(unsigned int i, char *c)
{
	if (g_calls < 64)
	{
		g_indices[g_calls] = i;
		g_ptrs[g_calls] = c;
	}
	g_calls++;
	if (*c >= 'a' && *c <= 'z')
		*c = *c - 'a' + 'A';
}

static void	index_digit(unsigned int i, char *c)
{
	*c = '0' + i % 10;
}

static void	check_striteri(const char *str, void (*f)(unsigned int, char *),
		const char *want)
{
	char	*s;

	s = guard_str(str);
	g_calls = 0;
	doing("ft_striteri(%s, f)", show(str));
	ft_striteri(s, f);
	check_str(s, want, "s afterwards");
}

static void	in_place(void)
{
	check_striteri("hello!", upper, "HELLO!");
	check_striteri("aaaa", index_digit, "0123");
}

static void	arguments(void)
{
	char			*s;
	unsigned int	i;

	s = guard_str("abcdef");
	g_calls = 0;
	doing("ft_striteri(\"abcdef\", f)");
	ft_striteri(s, upper);
	check_int(g_calls, 6, "number of calls to f");
	i = 0;
	while (i < 6 && i < g_calls)
	{
		check(g_indices[i] == i && g_ptrs[i] == s + i,
			"call %u to f: expected f(%u, s + %u), got f(%u, s + %td)",
			i + 1, i, i, g_indices[i], g_ptrs[i] - s);
		i++;
	}
}

static void	empty(void)
{
	check_striteri("", upper, "");
	check_int(g_calls, 0, "number of calls to f");
}

static void	long_string(void)
{
	static char	s[10001];
	static char	want[10001];

	memset(s, 'x', sizeof(s) - 1);
	memset(want, 'X', sizeof(want) - 1);
	check_striteri(s, upper, want);
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_striteri");
	PROTOTYPE(void, ft_striteri, (char *, void (*)(unsigned int, char *)));
	test("f changes the string in place", in_place);
	test("f gets each index and the address of each char, in order",
		arguments);
	test("empty string: f is never called", empty);
	test("long string (10000 chars)", long_string);
	return (done());
}
