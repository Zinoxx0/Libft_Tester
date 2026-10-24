#include "libft.h"
#include "tester.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* What the BSD strnstr does (glibc doesn't have it) */
static char	*ref_strnstr(const char *big, const char *little, size_t len)
{
	size_t	llen;
	size_t	i;

	llen = strlen(little);
	if (llen == 0)
		return ((char *)big);
	i = 0;
	while (big[i] && i + llen <= len)
	{
		if (strncmp(big + i, little, llen) == 0)
			return ((char *)big + i);
		i++;
	}
	return (NULL);
}

static void	check_strnstr(const char *big, const char *little, size_t len)
{
	char	*b;

	b = guard_str(big);
	doing("ft_strnstr(%s, %s, %zu)", show(big), show(little), len);
	check_ptr(ft_strnstr(b, guard_str(little), len),
		ref_strnstr(b, little, len), b, "return value");
}

static void	finds(void)
{
	check_strnstr("hello world", "world", 11);
	check_strnstr("hello world", "o w", 20);
	check_strnstr("hello world", "hello", 5);
	check_strnstr("hello", "hello", 5);
	check_strnstr("aaab", "ab", 4);
}

static void	not_found(void)
{
	check_strnstr("hello", "xyz", 5);
	check_strnstr("hello", "hello!", 10);
	check_strnstr("", "a", 5);
	check_strnstr("hello", "lol", 5);
}

static void	empty_needle(void)
{
	check_strnstr("hello", "", 5);
	check_strnstr("hello", "", 0);
	check_strnstr("", "", 0);
}

static void	must_fit_in_len(void)
{
	check_strnstr("hello world", "world", 10);
	check_strnstr("abcdef", "cd", 3);
	check_strnstr("abcdef", "cd", 4);
	check_strnstr("hello", "h", 0);
	check_strnstr("hello", "lo", 4);
}

static void	big_len(void)
{
	check_strnstr("hello", "lo", 100);
	check_strnstr("hello", "lo", SIZE_MAX);
	check_strnstr("hello", "xyz", SIZE_MAX);
}

static void	backtracking(void)
{
	check_strnstr("aaaab", "aab", 5);
	check_strnstr("ababc", "abc", 5);
	check_strnstr("mississippi", "issip", 11);
	check_strnstr("abcabcabd", "abcabd", 9);
	check_strnstr("aaa", "aaaa", 10);
}

/* Random strings over a 2-letter alphabet, so matches are frequent */
static void	random_strings(void)
{
	char	big[24];
	char	little[6];
	size_t	i;
	size_t	j;

	srand(42);
	i = 0;
	while (i++ < 3000)
	{
		j = 0;
		while (j < (size_t)rand() % 20)
			big[j++] = "ab"[rand() % 2];
		big[j] = '\0';
		j = 0;
		while (j < (size_t)rand() % 5)
			little[j++] = "ab"[rand() % 2];
		little[j] = '\0';
		check_strnstr(big, little, rand() % 24);
	}
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_strnstr");
	PROTOTYPE(char *, ft_strnstr, (const char *, const char *, size_t));
	test("finds the needle", finds);
	test("returns NULL when the needle isn't there", not_found);
	test("empty needle returns big", empty_needle);
	test("the needle must end within len chars", must_fit_in_len);
	test("len bigger than big", big_len);
	test("partial matches before the real one", backtracking);
	test("3000 random searches", random_strings);
	return (done());
}
