#include "libft.h"
#include "tester.h"
#include <string.h>

/* Both buffers end right before a protected page */
static void	check_memcmp(const void *a, const void *b, size_t n)
{
	doing("ft_memcmp(%s, %s, %zu)", show_mem(a, n), show_mem(b, n), n);
	check_sign(ft_memcmp(guard_after(a, n), guard_after(b, n), n),
		memcmp(a, b, n), "return value");
}

static void	equal(void)
{
	check_memcmp("abc", "abc", 3);
	check_memcmp("hello world", "hello world", 11);
}

static void	different(void)
{
	check_memcmp("abc", "abd", 3);
	check_memcmp("abd", "abc", 3);
	check_memcmp("abc", "bbc", 3);
	check_memcmp("hello world", "hello_world", 11);
}

static void	after_n(void)
{
	check_memcmp("abc", "abd", 2);
	check_memcmp("xyz", "abc", 0);
}

static void	past_nul(void)
{
	check_memcmp("ab\0c", "ab\0d", 4);
	check_memcmp("ab\0d", "ab\0c", 4);
	check_memcmp("\0\0\0a", "\0\0\0a", 4);
}

static void	unsigned_char(void)
{
	check_memcmp("\x80", "\x01", 1);
	check_memcmp("\x01", "\x80", 1);
	check_memcmp("a\xff", "a\x00", 2);
	check_memcmp("\xfe", "\xff", 1);
}

static void	zero_length(void)
{
	doing("ft_memcmp(s1, s2, 0) with pointers that can't be touched");
	check_int(ft_memcmp(guard_after(NULL, 0), guard_after(NULL, 0), 0), 0,
		"return value");
}

/* One differing byte at every position of buffers up to 40 bytes */
static void	every_position(void)
{
	unsigned char	a[40];
	unsigned char	b[40];
	size_t			diff;
	size_t			n;

	memset(a, 'm', sizeof(a));
	memset(b, 'm', sizeof(b));
	diff = 0;
	while (diff < sizeof(a))
	{
		b[diff] = (diff % 2) ? 'a' : 'z';
		n = 0;
		while (n <= sizeof(a))
			check_memcmp(a, b, n++);
		b[diff++] = 'm';
	}
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_memcmp");
	PROTOTYPE(int, ft_memcmp, (const void *, const void *, size_t));
	test("equal buffers give 0", equal);
	test("the sign follows the first different byte", different);
	test("differences after n bytes don't count", after_n);
	test("doesn't stop at '\\0'", past_nul);
	test("compares bytes as unsigned char", unsigned_char);
	test("n = 0 touches nothing", zero_length);
	test("every position of a difference, every n up to 40",
		every_position);
	return (done());
}
