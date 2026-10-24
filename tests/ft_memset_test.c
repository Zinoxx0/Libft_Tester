#include "libft.h"
#include "tester.h"
#include <string.h>

/*
** Fills n bytes after an 8-byte margin, in a buffer that ends right before a
** protected page: writing one byte too many crashes.
*/
static void	check_memset(int c, size_t n)
{
	unsigned char	*buf;
	unsigned char	want[8 + 4096];
	void			*ret;

	buf = guard_after(NULL, 8 + n);
	memset(buf, 'x', 8 + n);
	memset(want, 'x', 8);
	memset(want + 8, c, n);
	doing("ft_memset(s, %d, %zu)", c, n);
	ret = ft_memset(buf + 8, c, n);
	check_ptr(ret, buf + 8, buf + 8, "return value");
	check_mem(buf, want, 8 + n, "buffer (8 bytes before s, then s)");
}

static void	fills_n_bytes(void)
{
	check_memset('A', 1);
	check_memset('A', 5);
	check_memset('z', 16);
	check_memset(0, 10);
}

static void	zero_length(void)
{
	void	*s;

	s = guard_after(NULL, 0);
	doing("ft_memset(s, 'A', 0) on a pointer that can't be touched");
	check_ptr(ft_memset(s, 'A', 0), s, s, "return value");
}

static void	unsigned_char(void)
{
	check_memset(255, 4);
	check_memset(-1, 4);
	check_memset(-128, 4);
	check_memset('A' + 256, 4);
	check_memset(0x12345678, 4);
}

static void	every_size(void)
{
	size_t	n;

	n = 0;
	while (n <= 64)
		check_memset('#', n++);
	check_memset('#', 4096);
}

static void	big_buffer(void)
{
	static unsigned char	got[100000];
	static unsigned char	want[100000];

	memset(want, 'q', sizeof(want));
	doing("ft_memset(s, 'q', 100000)");
	ft_memset(got, 'q', sizeof(got));
	check_mem(got, want, sizeof(got), "s");
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_memset");
	PROTOTYPE(void *, ft_memset, (void *, int, size_t));
	test("fills n bytes and returns s", fills_n_bytes);
	test("n = 0 touches nothing", zero_length);
	test("c is converted to unsigned char", unsigned_char);
	test("writes exactly n bytes, every n up to 64", every_size);
	test("big buffer (100000 bytes)", big_buffer);
	return (done());
}
