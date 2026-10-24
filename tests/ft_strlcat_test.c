#include "libft.h"
#include "tester.h"
#include <string.h>

/* What the BSD strlcat does (glibc only has it since 2.38) */
static size_t	ref_strlcat(char *dst, const char *src, size_t size)
{
	size_t	dlen;
	size_t	slen;
	size_t	i;

	dlen = 0;
	while (dlen < size && dst[dlen])
		dlen++;
	slen = strlen(src);
	if (dlen == size)
		return (size + slen);
	i = 0;
	while (src[i] && dlen + i + 1 < size)
	{
		dst[dlen + i] = src[i];
		i++;
	}
	dst[dlen + i] = '\0';
	return (dlen + slen);
}

/*
** dst holds the string start in a buffer of max(size, strlen(start) + 1)
** bytes, padded with garbage and right before a protected page.
*/
static void	check_strlcat(const char *start, const char *src, size_t size)
{
	char	want[4096];
	char	*dst;
	size_t	bufsize;
	size_t	ret;

	bufsize = strlen(start) + 1;
	if (size > bufsize)
		bufsize = size;
	dst = guard_after(NULL, bufsize);
	memcpy(dst, start, strlen(start) + 1);
	memcpy(want, dst, bufsize);
	ret = ref_strlcat(want, src, size);
	doing("ft_strlcat(dst = %s, %s, %zu)", show(start), show(src), size);
	check_size(ft_strlcat(dst, guard_str(src), size), ret, "return value");
	check_mem(dst, want, bufsize, "dst buffer");
}

static void	enough_room(void)
{
	check_strlcat("42", " Lisboa", 20);
	check_strlcat("abc", "def", 7);
	check_strlcat("hello ", "world", 100);
}

static void	truncates(void)
{
	check_strlcat("abc", "defgh", 6);
	check_strlcat("abc", "defgh", 5);
	check_strlcat("abc", "defgh", 4);
}

static void	size_below_dst(void)
{
	check_strlcat("hello", "abc", 5);
	check_strlcat("hello", "abc", 3);
	check_strlcat("hello", "abc", 1);
	check_strlcat("hello", "abc", 0);
}

static void	empty_strings(void)
{
	check_strlcat("", "abc", 10);
	check_strlcat("", "abc", 2);
	check_strlcat("abc", "", 10);
	check_strlcat("", "", 1);
	check_strlcat("", "", 0);
}

static void	long_src(void)
{
	static char	src[5001];

	memset(src, 'L', sizeof(src) - 1);
	check_strlcat("start", src, 10);
	check_strlcat("start", src, 4096);
}

static void	every_size(void)
{
	const char	*starts[] = {"", "a", "hello", "0123456789"};
	const char	*sources[] = {"", "x", "world", "abcdefghij"};
	size_t		i;
	size_t		j;
	size_t		size;

	i = 0;
	while (i < 4)
	{
		j = 0;
		while (j < 4)
		{
			size = 0;
			while (size <= 24)
				check_strlcat(starts[i], sources[j], size++);
			j++;
		}
		i++;
	}
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_strlcat");
	PROTOTYPE(size_t, ft_strlcat, (char *, const char *, size_t));
	test("enough room", enough_room);
	test("src is cut so the result fits in size - 1 chars", truncates);
	test("size <= strlen(dst): dst unchanged, returns size + strlen(src)",
		size_below_dst);
	test("empty dst or src", empty_strings);
	test("returns the full length even when it doesn't fit", long_src);
	test("every size from 0 to 24, no byte written past size", every_size);
	return (done());
}
