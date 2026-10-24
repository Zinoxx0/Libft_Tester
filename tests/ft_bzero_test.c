#include "libft.h"
#include "tester.h"
#include <string.h>

/* Same layout as the memset test: 8-byte margin, protected page right after */
static void	check_bzero(size_t n)
{
	unsigned char	*buf;
	unsigned char	want[8 + 4096];

	buf = guard_after(NULL, 8 + n);
	memset(buf, 'x', 8 + n);
	memset(want, 'x', 8);
	memset(want + 8, 0, n);
	doing("ft_bzero(s, %zu)", n);
	ft_bzero(buf + 8, n);
	check_mem(buf, want, 8 + n, "buffer (8 bytes before s, then s)");
}

static void	zeroes_n_bytes(void)
{
	check_bzero(1);
	check_bzero(5);
	check_bzero(16);
}

static void	zero_length(void)
{
	doing("ft_bzero(s, 0) on a pointer that can't be touched");
	ft_bzero(guard_after(NULL, 0), 0);
}

static void	every_size(void)
{
	size_t	n;

	n = 0;
	while (n <= 64)
		check_bzero(n++);
	check_bzero(4096);
}

static void	big_buffer(void)
{
	static unsigned char	got[100000];
	static unsigned char	want[100000];

	memset(got, 'x', sizeof(got));
	doing("ft_bzero(s, 100000)");
	ft_bzero(got, sizeof(got));
	check_mem(got, want, sizeof(got), "s");
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_bzero");
	PROTOTYPE(void, ft_bzero, (void *, size_t));
	test("zeroes n bytes", zeroes_n_bytes);
	test("n = 0 touches nothing", zero_length);
	test("writes exactly n bytes, every n up to 64", every_size);
	test("big buffer (100000 bytes)", big_buffer);
	return (done());
}
