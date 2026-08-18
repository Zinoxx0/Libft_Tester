#include "../src/libft.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int	check_words(char **words, const char **expected, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n)
	{
		if (!words[i] || strcmp(words[i], expected[i]) != 0)
			return (0);
		i++;
	}
	return (words[n] == NULL);
}

static void	free_words(char **words)
{
	size_t	i;

	if (!words)
		return ;
	i = 0;
	while (words[i])
		free(words[i++]);
	free(words);
}

int	main(void)
{
	char		**words;
	const char	*expected[] = {"This", "is", "a", "test"};
	int			passed;

	printf("\n\033[1;36m       🧪 Testing ft_split Function     \033[0m\n\n");

	passed = 0;
	words = ft_split("This is a test", ' ');
	if (words && check_words(words, expected, 4))
	{
		printf("\033[0;32m✓ basic split passed\033[0m\n");
		passed++;
	}
	else
		printf("\033[0;31m✗ basic split failed\033[0m\n");
	free_words(words);

	words = ft_split("---one---two---", '-');
	if (words && strcmp(words[0], "one") == 0
		&& strcmp(words[1], "two") == 0 && words[2] == NULL)
	{
		printf("\033[0;32m✓ repeated delimiters passed\033[0m\n");
		passed++;
	}
	else
		printf("\033[0;31m✗ repeated delimiters failed\033[0m\n");
	free_words(words);

	words = ft_split("", ' ');
	if (words && words[0] == NULL)
	{
		printf("\033[0;32m✓ empty string passed\033[0m\n");
		passed++;
	}
	else
		printf("\033[0;31m✗ empty string failed\033[0m\n");
	free_words(words);

	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	if (passed == 3)
		printf("\033[1;32m✨ All tests passed! (%d/3)\033[0m\n", passed);
	else
		printf("\033[1;33m⚠ Result: %d/3 tests passed\033[0m\n", passed);
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	return (passed == 3);
}
