#include "libft.h"
#include "tester.h"
#include <string.h>

static void	check_strlen(const char *s)
{
	doing("ft_strlen(%s)", show(s));
	check_size(ft_strlen(guard_str(s)), strlen(s), "return value");
}

static void	empty_string(void)
{
	check_strlen("");
}

static void	short_strings(void)
{
	check_strlen("a");
	check_strlen("hello");
	check_strlen("42 Lisboa");
	check_strlen("tabs\tand\nnewlines\n");
}

static void	high_bytes(void)
{
	check_strlen("\xff\x80\xe9");
	check_strlen("caf\xc3\xa9");
}

static void	first_nul(void)
{
	check_strlen("abc\0def");
}

static void	long_string(void)
{
	static char	s[10001];

	memset(s, 'x', sizeof(s) - 1);
	check_strlen(s);
}

/* Lengths 0-40 at every alignment, each ending right before a protected page */
static void	alignments(void)
{
	char	s[64];
	size_t	len;
	size_t	offset;
	char	*copy;

	memset(s, 'a', sizeof(s));
	len = 0;
	while (len <= 40)
	{
		s[len] = '\0';
		offset = 0;
		while (offset < 8)
		{
			copy = (char *)guard_after(NULL, offset + len + 1) + offset;
			memcpy(copy, s, len + 1);
			doing("ft_strlen(<%zu chars at offset %zu>)", len, offset);
			check_size(ft_strlen(copy), len, "return value");
			offset++;
		}
		s[len++] = 'a';
	}
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_strlen");
	PROTOTYPE(size_t, ft_strlen, (const char *));
	test("empty string", empty_string);
	test("short strings", short_strings);
	test("bytes above 127 count too", high_bytes);
	test("stops at the first '\\0'", first_nul);
	test("long string (10000 chars)", long_string);
	test("every length up to 40 at every alignment", alignments);
	return (done());
}
