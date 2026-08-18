#include "../src/libft.h"
#include <stdio.h>
#include <string.h>

int	main(void)
{
	char	*joined;
	int		passed;

	printf("\n\033[1;36m      🧪 Testing ft_strjoin Function     \033[0m\n\n");
	joined = ft_strjoin("hello", " world");
	passed = joined && strcmp(joined, "hello world") == 0;
	free(joined);
	joined = ft_strjoin("", "libft");
	passed = passed && joined && strcmp(joined, "libft") == 0;
	free(joined);
	printf("%s\n", passed ? "\033[0;32m✓ ft_strjoin passed\033[0m" : "\033[0;31m✗ ft_strjoin failed\033[0m");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	if (passed)
		printf("\033[1;32m✨ All tests passed! (1/1)\033[0m\n");
	else
		printf("\033[1;33m⚠ Result: 0/1 tests passed\033[0m\n");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	return (passed);
}
