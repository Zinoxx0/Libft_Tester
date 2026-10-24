#include "libft.h"
#include "tester.h"
#include <stdint.h>
#include <string.h>

/* Both strings end right before a protected page */
static void	check_strncmp(const char *s1, const char *s2, size_t n)
{
	doing("ft_strncmp(%s, %s, %zu)", show(s1), show(s2), n);
	check_sign(ft_strncmp(guard_str(s1), guard_str(s2), n),
		strncmp(s1, s2, n), "return value");
}

static void	equal(void)
{
	check_strncmp("abc", "abc", 3);
	check_strncmp("abc", "abc", 10);
	check_strncmp("", "", 5);
}

static void	different(void)
{
	check_strncmp("abc", "abd", 3);
	check_strncmp("abd", "abc", 3);
	check_strncmp("abc", "bbc", 1);
	check_strncmp("hello world", "hello_world", 20);
}

static void	after_n(void)
{
	check_strncmp("abc", "abd", 2);
	check_strncmp("abc", "xyz", 0);
	check_strncmp("", "xyz", 0);
}

static void	prefix(void)
{
	check_strncmp("abc", "abcd", 4);
	check_strncmp("abcd", "abc", 4);
	check_strncmp("abc", "abcd", 3);
	check_strncmp("", "a", 1);
	check_strncmp("a", "", 1);
}

static void	stops_at_nul(void)
{
	check_strncmp("abc", "abc", 100);
	check_strncmp("abc\0x", "abc\0y", 5);
	check_strncmp("abc", "abc", SIZE_MAX);
	check_strncmp("abc", "abd", SIZE_MAX);
}

static void	unsigned_char(void)
{
	check_strncmp("\x80", "\x01", 1);
	check_strncmp("\x01", "\x80", 1);
	check_strncmp("test\200", "test", 6);
	check_strncmp("a\xff", "a\x7f", 2);
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_strncmp");
	PROTOTYPE(int, ft_strncmp, (const char *, const char *, size_t));
	test("equal strings give 0", equal);
	test("the sign follows the first different char", different);
	test("differences after n chars don't count", after_n);
	test("one string is a prefix of the other", prefix);
	test("stops at '\\0' even when n is bigger", stops_at_nul);
	test("compares chars as unsigned char", unsigned_char);
	return (done());
}
