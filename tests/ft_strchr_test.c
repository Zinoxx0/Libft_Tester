#include "libft.h"
#include "tester.h"
#include <string.h>

static void	check_strchr(const char *str, int c)
{
	char	*s;

	s = guard_str(str);
	doing("ft_strchr(%s, %s)", show(str), show_char(c));
	check_ptr(ft_strchr(s, c), strchr(s, c), s, "return value");
}

static void	finds_first(void)
{
	check_strchr("hello", 'l');
	check_strchr("hello", 'h');
	check_strchr("hello", 'o');
	check_strchr("a", 'a');
}

static void	not_found(void)
{
	check_strchr("hello", 'z');
	check_strchr("", 'a');
}

static void	terminator(void)
{
	check_strchr("hello", '\0');
	check_strchr("", '\0');
}

static void	char_conversion(void)
{
	check_strchr("hello", 'l' + 256);
	check_strchr("hello", 256);
	check_strchr("caf\xc3\xa9", 0xa9);
	check_strchr("caf\xc3\xa9", (char)0xa9);
	check_strchr("caf\xc3\xa9", -1);
}

/* A string with every byte from 1 to 255: each one is found in place */
static void	every_char(void)
{
	char	s[256];
	int		c;

	c = 1;
	while (c < 256)
	{
		s[c - 1] = c;
		c++;
	}
	s[255] = '\0';
	c = -1;
	while (c <= 255)
		check_strchr(s, c++);
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_strchr");
	PROTOTYPE(char *, ft_strchr, (const char *, int));
	test("finds the first occurrence", finds_first);
	test("returns NULL when c isn't there", not_found);
	test("c = '\\0' finds the terminator", terminator);
	test("c is converted to char", char_conversion);
	test("every byte value", every_char);
	return (done());
}
