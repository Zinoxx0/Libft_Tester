#include "libft.h"
#include "tester.h"
#include <string.h>

/*
** dst is exactly size bytes, filled with garbage, right before a protected
** page; src ends right before another one.
*/
static void	check_strlcpy(const char *src, size_t size)
{
	char	*dst;
	char	want[4096];
	size_t	len;
	size_t	copied;

	len = strlen(src);
	dst = guard_after(NULL, size);
	doing("ft_strlcpy(dst, %s, %zu)", show(src), size);
	check_size(ft_strlcpy(dst, guard_str(src), size), len, "return value");
	if (size == 0)
		return ;
	copied = len < size - 1 ? len : size - 1;
	memcpy(want, src, copied);
	want[copied] = '\0';
	check_mem(dst, want, copied + 1, "dst");
}

static void	fits(void)
{
	check_strlcpy("hello", 10);
	check_strlcpy("hello", 6);
	check_strlcpy("42 Lisboa", 100);
}

static void	truncates(void)
{
	check_strlcpy("hello", 5);
	check_strlcpy("hello", 3);
	check_strlcpy("hello", 2);
	check_strlcpy("hello", 1);
}

static void	size_zero(void)
{
	check_strlcpy("hello", 0);
	check_strlcpy("", 0);
}

static void	empty_src(void)
{
	check_strlcpy("", 1);
	check_strlcpy("", 10);
}

static void	long_src(void)
{
	static char	src[5001];

	memset(src, 'L', sizeof(src) - 1);
	check_strlcpy(src, 10);
	check_strlcpy(src, 4096);
}

static void	every_size(void)
{
	const char	*sources[] = {"", "a", "abc", "hello world", "0123456789abcdef"};
	size_t		i;
	size_t		size;

	i = 0;
	while (i < sizeof(sources) / sizeof(*sources))
	{
		size = 0;
		while (size <= 20)
			check_strlcpy(sources[i], size++);
		i++;
	}
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_strlcpy");
	PROTOTYPE(size_t, ft_strlcpy, (char *, const char *, size_t));
	test("src fits in dst", fits);
	test("src is cut to size - 1 chars, always '\\0'-terminated", truncates);
	test("size 0 writes nothing", size_zero);
	test("empty src", empty_src);
	test("returns strlen(src) even when it doesn't fit", long_src);
	test("every size from 0 to 20, no byte written past size", every_size);
	return (done());
}
