#include "libft.h"
#include "tester.h"
#include <string.h>

/*
** src and dst both end right before a protected page, so reading or writing
** past n bytes crashes. dst starts at `offset` inside its buffer, whose first
** bytes must stay untouched.
*/
static void	check_memcpy(const void *data, size_t n, size_t offset)
{
	unsigned char	*src;
	unsigned char	*dst;
	unsigned char	want[8 + 4096];
	void			*ret;

	src = guard_after(data, n);
	dst = guard_after(NULL, offset + n);
	memset(dst, 'x', offset + n);
	memset(want, 'x', offset);
	memcpy(want + offset, data, n);
	doing("ft_memcpy(dst, %s, %zu)", show_mem(data, n), n);
	ret = ft_memcpy(dst + offset, src, n);
	check_ptr(ret, dst + offset, dst + offset, "return value");
	check_mem(dst, want, offset + n, "dst");
}

static void	copies(void)
{
	check_memcpy("hello", 6, 0);
	check_memcpy("42 Lisboa", 9, 3);
}

static void	binary_data(void)
{
	check_memcpy("ab\0cd\0ef", 8, 0);
	check_memcpy("\0\0\0\0", 4, 1);
	check_memcpy("\xff\x80\x7f\x01", 4, 0);
}

static void	zero_length(void)
{
	void	*dst;

	dst = guard_after(NULL, 0);
	doing("ft_memcpy(dst, src, 0) with pointers that can't be touched");
	check_ptr(ft_memcpy(dst, guard_after(NULL, 0), 0), dst, dst,
		"return value");
}

static void	every_size(void)
{
	unsigned char	data[4096];
	size_t			n;
	size_t			offset;

	n = 0;
	while (n < sizeof(data))
	{
		data[n] = n * 7 + 3;
		n++;
	}
	n = 0;
	while (n <= 64)
	{
		offset = 0;
		while (offset < 8)
			check_memcpy(data, n, offset++);
		n++;
	}
	check_memcpy(data, sizeof(data), 0);
}

static void	big_copy(void)
{
	static unsigned char	src[100000];
	static unsigned char	dst[100000];
	size_t					i;

	i = 0;
	while (i < sizeof(src))
	{
		src[i] = i % 251;
		i++;
	}
	doing("ft_memcpy(dst, src, 100000)");
	ft_memcpy(dst, src, sizeof(src));
	check_mem(dst, src, sizeof(src), "dst");
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_memcpy");
	PROTOTYPE(void *, ft_memcpy, (void *, const void *, size_t));
	test("copies n bytes and returns dst", copies);
	test("copies '\\0' and any other byte", binary_data);
	test("n = 0 touches nothing", zero_length);
	test("every n up to 64 at every alignment, no byte too many",
		every_size);
	test("big copy (100000 bytes)", big_copy);
	return (done());
}
