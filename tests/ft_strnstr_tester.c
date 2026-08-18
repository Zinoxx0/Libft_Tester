#include "../src/libft.h"
#include <stdio.h>
#include <string.h>

int	main(void)
{
	const char	*text = "find this word";
	int			passed;

	printf("\n\033[1;36m      🧪 Testing ft_strnstr Function     \033[0m\n\n");
	passed = ft_strnstr(text, "this", strlen(text)) == strstr(text, "this");
	passed = passed && ft_strnstr(text, "word", 8) == NULL;
	passed = passed && ft_strnstr(text, "", 0) == text;
	printf("%s\n", passed ? "\033[0;32m✓ ft_strnstr passed\033[0m" : "\033[0;31m✗ ft_strnstr failed\033[0m");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	if (passed)
		printf("\033[1;32m✨ All tests passed! (1/1)\033[0m\n");
	else
		printf("\033[1;33m⚠ Result: 0/1 tests passed\033[0m\n");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	return (passed);
}
