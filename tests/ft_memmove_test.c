#include "libft.h"
#include "tester.h"
#include <string.h>

/*
** Moves n bytes from buf + from to buf + to inside a buffer of size bytes,
** placed either right before or right after a protected page, and compares
** the whole buffer with what memmove does.
*/
static void	check_move(size_t size, size_t to, size_t from, size_t n,
		int guard_front)
{
	unsigned char	data[256];
	unsigned char	want[256];
	unsigned char	*buf;
	size_t			i;
	void			*ret;

	i = 0;
	while (i < size)
	{
		data[i] = 'a' + i % 26;
		i++;
	}
	memcpy(want, data, size);
	memmove(want + to, want + from, n);
	if (guard_front)
		buf = guard_before(data, size);
	else
		buf = guard_after(data, size);
	doing("ft_memmove(buf + %zu, buf + %zu, %zu) in a %zu-byte buffer",
		to, from, n, size);
	ret = ft_memmove(buf + to, buf + from, n);
	check_ptr(ret, buf + to, buf + to, "return value");
	check_mem(buf, want, size, "buf");
}

static void	no_overlap(void)
{
	check_move(20, 10, 0, 10, 0);
	check_move(20, 0, 10, 10, 0);
}

static void	dst_after_src(void)
{
	check_move(10, 2, 0, 8, 0);
	check_move(10, 2, 0, 8, 1);
	check_move(10, 1, 0, 9, 0);
	check_move(10, 1, 0, 9, 1);
}

static void	dst_before_src(void)
{
	check_move(10, 0, 2, 8, 0);
	check_move(10, 0, 2, 8, 1);
	check_move(10, 0, 1, 9, 0);
	check_move(10, 0, 1, 9, 1);
}

static void	same_pointer(void)
{
	check_move(10, 0, 0, 10, 0);
	check_move(10, 3, 3, 5, 1);
}

static void	zero_length(void)
{
	void	*p;

	p = guard_after(NULL, 0);
	doing("ft_memmove(dst, src, 0) with pointers that can't be touched");
	check_ptr(ft_memmove(p, guard_before(NULL, 0), 0), p, p, "return value");
}

/* Every pair of offsets 0-15 and every length that fits in 48 bytes */
static void	every_overlap(void)
{
	size_t	to;
	size_t	from;
	size_t	n;

	to = 0;
	while (to < 16)
	{
		from = 0;
		while (from < 16)
		{
			n = 0;
			while (n + (to > from ? to : from) <= 48)
			{
				check_move(48, to, from, n, (to + from + n) % 2);
				n++;
			}
			from++;
		}
		to++;
	}
}

static void	big_move(void)
{
	static unsigned char	buf[100010];
	static unsigned char	want[100010];
	size_t					i;

	i = 0;
	while (i < sizeof(buf))
	{
		buf[i] = i % 251;
		i++;
	}
	memcpy(want, buf, sizeof(buf));
	memmove(want + 7, want, 100000);
	doing("ft_memmove(buf + 7, buf, 100000)");
	ft_memmove(buf + 7, buf, 100000);
	check_mem(buf, want, sizeof(buf), "buf");
	memmove(want, want + 9, 100000);
	doing("ft_memmove(buf, buf + 9, 100000)");
	ft_memmove(buf, buf + 9, 100000);
	check_mem(buf, want, sizeof(buf), "buf");
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_memmove");
	PROTOTYPE(void *, ft_memmove, (void *, const void *, size_t));
	test("copies without overlap", no_overlap);
	test("overlap with dst after src (copies backwards)", dst_after_src);
	test("overlap with dst before src", dst_before_src);
	test("dst == src", same_pointer);
	test("n = 0 touches nothing", zero_length);
	test("every overlap in a 48-byte buffer", every_overlap);
	test("big overlapping moves (100000 bytes)", big_move);
	return (done());
}
