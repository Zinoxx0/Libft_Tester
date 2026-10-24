#include "libft.h"
#include "tester.h"
#include <stdio.h>
#include <string.h>

static void	free_words(void *array)
{
	char	**words;
	size_t	i;

	words = array;
	i = 0;
	while (words[i])
		free(words[i++]);
	free(words);
}

/* want is NULL-terminated. Every word must be its own exact malloc block */
static void	check_split(const char *s, char c, const char **want)
{
	char	**got;
	size_t	count;
	size_t	i;
	char	what[32];

	count = 0;
	while (want[count])
		count++;
	doing("ft_split(%s, %s)", show(s), show_char((unsigned char)c));
	got = ft_split(guard_str(s), c);
	if (!check(got != NULL, "returned NULL"))
		return ;
	i = 0;
	while (i < count && got[i])
	{
		snprintf(what, sizeof(what), "word %zu", i);
		if (check_str(got[i], want[i], what))
			check_alloc(got[i], strlen(want[i]) + 1, what);
		i++;
	}
	if (!check(i == count && got[i] == NULL, "expected %zu word%s, got %s",
			count, count == 1 ? "" : "s", i < count ? "fewer" : "more"))
		return ;
	check_alloc(got, (count + 1) * sizeof(char *), "the array");
	free_words(got);
}

static void	spaces(void)
{
	check_split("hello world", ' ', (const char *[]){"hello", "world", NULL});
	check_split("a b c", ' ', (const char *[]){"a", "b", "c", NULL});
}

static void	extra_separators(void)
{
	check_split("  hello   world  ", ' ',
		(const char *[]){"hello", "world", NULL});
	check_split(",,a,,b,,", ',', (const char *[]){"a", "b", NULL});
	check_split("x", 'x', (const char *[]){NULL});
}

static void	no_separator(void)
{
	check_split("hello", ' ', (const char *[]){"hello", NULL});
}

static void	empty(void)
{
	check_split("", ' ', (const char *[]){NULL});
	check_split("     ", ' ', (const char *[]){NULL});
}

static void	nul_separator(void)
{
	check_split("hello world", '\0', (const char *[]){"hello world", NULL});
	check_split("", '\0', (const char *[]){NULL});
}

static void	high_separator(void)
{
	check_split("a\xff" "b\xff\xff" "c", '\xff',
		(const char *[]){"a", "b", "c", NULL});
	check_split("\xe9t\xe9", 't', (const char *[]){"\xe9", "\xe9", NULL});
}

static void	many_words(void)
{
	static char	s[8000];
	static char	words[1000][6];
	const char	*want[1001];
	size_t		len;
	int			i;

	len = 0;
	i = 0;
	while (i < 1000)
	{
		snprintf(words[i], sizeof(words[i]), "w%d", i);
		want[i] = words[i];
		len += snprintf(s + len, sizeof(s) - len, "%s ", words[i]);
		i++;
	}
	want[1000] = NULL;
	check_split(s, ' ', want);
}

static void	*call_split(void *arg)
{
	return (ft_split(arg, ' '));
}

static void	malloc_fails(void)
{
	doing("ft_split(\"  hello wonderful world  \", ' ')");
	check_malloc_failures(call_split, "  hello wonderful world  ",
		free_words, 1);
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_split");
	PROTOTYPE(char **, ft_split, (char const *, char));
	test("words separated by spaces", spaces);
	test("separators at the ends and repeated", extra_separators);
	test("no separator gives one word", no_separator);
	test("empty or only separators gives an empty array", empty);
	test("c = '\\0' gives the whole string", nul_separator);
	test("chars above 127", high_separator);
	test("1000 words", many_words);
	test("returns NULL and frees everything when malloc fails",
		malloc_fails);
	return (done());
}
