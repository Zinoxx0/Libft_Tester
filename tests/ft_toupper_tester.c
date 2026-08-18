#include "../src/libft.h"
#include <stdio.h>

int	main(void)
{
	int	passed;

	printf("\n\033[1;36m      🧪 Testing ft_toupper Function     \033[0m\n\n");
	passed = ft_toupper('a') == 'A' && ft_toupper('Z') == 'Z'
		&& ft_toupper('!') == '!';
	printf("%s\n", passed ? "\033[0;32m✓ ft_toupper passed\033[0m" : "\033[0;31m✗ ft_toupper failed\033[0m");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	if (passed)
		printf("\033[1;32m✨ All tests passed! (1/1)\033[0m\n");
	else
		printf("\033[1;33m⚠ Result: 0/1 tests passed\033[0m\n");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	return (passed);
}
