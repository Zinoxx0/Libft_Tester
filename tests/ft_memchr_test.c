#include "libft.h"
#include "tester.h"
#include <string.h>

/* The n bytes searched end right before a protected page */
static void	check_memchr(const void *data, int c, size_t n)
{
	unsigned char	*s;

	s = guard_after(data, n);
	doing("ft_memchr(%s, %s, %zu)", show_mem(data, n), show_char(c), n);
	check_ptr(ft_memchr(s, c, n), memchr(s, c, n), s, "return value");
}

static void	finds_first(void)
{
	check_memchr("hello world", 'o', 11);
	check_memchr("hello world", 'h', 11);
	check_memchr("hello world", 'd', 11);
}

static void	not_found(void)
{
	check_memchr("hello", 'z', 5);
	check_memchr("hello", 'o', 4);
	check_memchr("hello", 'h', 0);
}

static void	past_nul(void)
{
	check_memchr("ab\0cd", 'd', 5);
	check_memchr("ab\0cd", '\0', 5);
	check_memchr("\0\0\0x", 'x', 4);
}

static void	unsigned_char(void)
{
	check_memchr("ab\xffz", 0xff, 4);
	check_memchr("ab\xffz", -1, 4);
	check_memchr("ab\x80z", -128, 4);
	check_memchr("abcd", 'b' + 256, 4);
	check_memchr("ab\0d", 256, 4);
}

static void	every_position(void)
{
	unsigned char	data[64];
	size_t			target;
	size_t			n;

	n = 0;
	while (n < sizeof(data))
	{
		data[n] = 'a' + n % 20;
		n++;
	}
	target = 0;
	while (target < sizeof(data))
	{
		data[target] = '*';
		n = 0;
		while (n <= sizeof(data))
			check_memchr(data, '*', n++);
		data[target] = 'a' + target % 20;
		target += 7;
	}
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_memchr");
	PROTOTYPE(void *, ft_memchr, (const void *, int, size_t));
	test("finds the first occurrence", finds_first);
	test("returns NULL when c isn't in the first n bytes", not_found);
	test("doesn't stop at '\\0'", past_nul);
	test("c is converted to unsigned char", unsigned_char);
	test("every position and length in a 64-byte buffer", every_position);
	return (done());
}
