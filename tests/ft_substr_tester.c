#include "../src/libft.h"
#include <stdio.h>
#include <string.h>

int	main(void)
{
	char	*part;
	int		passed;

	printf("\n\033[1;36m      🧪 Testing ft_substr Function     \033[0m\n\n");
	part = ft_substr("libft testing", 6, 7);
	passed = part && strcmp(part, "testing") == 0;
	free(part);
	part = ft_substr("hello", 20, 3);
	passed = passed && part && strcmp(part, "") == 0;
	free(part);
	printf("%s\n", passed ? "\033[0;32m✓ ft_substr passed\033[0m" : "\033[0;31m✗ ft_substr failed\033[0m");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	if (passed)
		printf("\033[1;32m✨ All tests passed! (1/1)\033[0m\n");
	else
		printf("\033[1;33m⚠ Result: 0/1 tests passed\033[0m\n");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	return (passed);
}
