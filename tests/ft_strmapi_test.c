#include "libft.h"
#include "tester.h"
#include <string.h>

static unsigned int	g_calls;
static unsigned int	g_indices[64];
static char			g_chars[64];

static char	upper(unsigned int i, char c)
{
	if (g_calls < 64)
	{
		g_indices[g_calls] = i;
		g_chars[g_calls] = c;
	}
	g_calls++;
	if (c >= 'a' && c <= 'z')
		return (c - 'a' + 'A');
	return (c);
}

static char	add_index(unsigned int i, char c)
{
	return (c + i);
}

static void	check_strmapi(const char *str, char (*f)(unsigned int, char),
		const char *want)
{
	char	*s;
	char	*mapped;

	s = guard_str(str);
	g_calls = 0;
	doing("ft_strmapi(%s, f)", show(str));
	mapped = ft_strmapi(s, f);
	check_str(s, str, "s (must not change)");
	if (!check_str(mapped, want, "result") || !mapped)
		return ;
	check(mapped != s, "returned s itself, not a new string");
	check_alloc(mapped, strlen(want) + 1, "the result");
	free(mapped);
}

static void	applies_f(void)
{
	check_strmapi("hello!", upper, "HELLO!");
	check_strmapi("42 Lisboa", upper, "42 LISBOA");
}

static void	indices(void)
{
	unsigned int	i;

	check_strmapi("aaaaaa", add_index, "abcdef");
	check_strmapi("abcdef", upper, "ABCDEF");
	check_int(g_calls, 6, "number of calls to f");
	i = 0;
	while (i < 6 && i < g_calls)
	{
		check(g_indices[i] == i && g_chars[i] == "abcdef"[i],
			"call %u to f: expected f(%u, '%c'), got f(%u, %s)", i + 1, i,
			"abcdef"[i], g_indices[i], show_char((unsigned char)g_chars[i]));
		i++;
	}
}

static void	empty(void)
{
	check_strmapi("", upper, "");
	check_int(g_calls, 0, "number of calls to f");
}

static void	long_string(void)
{
	static char	s[10001];
	static char	want[10001];

	memset(s, 'x', sizeof(s) - 1);
	memset(want, 'X', sizeof(want) - 1);
	check_strmapi(s, upper, want);
}

static void	*call_strmapi(void *arg)
{
	return (ft_strmapi(arg, upper));
}

static void	malloc_fails(void)
{
	doing("ft_strmapi(\"hello\", f)");
	check_malloc_failures(call_strmapi, "hello", free, 1);
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_strmapi");
	PROTOTYPE(char *, ft_strmapi, (char const *, char (*)(unsigned int,
				char)));
	test("applies f to every char, into a new string", applies_f);
	test("f gets each index and char, in order", indices);
	test("empty string: f is never called", empty);
	test("long string (10000 chars)", long_string);
	test("returns NULL when malloc fails", malloc_fails);
	return (done());
}
