#include "../src/libft.h"
#include <stdio.h>

int	main(void)
{
	int	passed;

	printf("\n\033[1;36m      🧪 Testing ft_tolower Function     \033[0m\n\n");
	passed = ft_tolower('A') == 'a' && ft_tolower('z') == 'z'
		&& ft_tolower('!') == '!';
	printf("%s\n", passed ? "\033[0;32m✓ ft_tolower passed\033[0m" : "\033[0;31m✗ ft_tolower failed\033[0m");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	if (passed)
		printf("\033[1;32m✨ All tests passed! (1/1)\033[0m\n");
	else
		printf("\033[1;33m⚠ Result: 0/1 tests passed\033[0m\n");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	return (passed);
}
