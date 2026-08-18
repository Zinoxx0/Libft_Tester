#include "../src/libft.h"
#include <stdio.h>
#include <string.h>

int	main(void)
{
	int	passed;

	printf("\n\033[1;36m      🧪 Testing ft_strlen Function     \033[0m\n\n");

	passed = 0;
	if (ft_strlen("Hello, world!") == strlen("Hello, world!"))
	{
		printf("\033[0;32m✓ normal string passed\033[0m\n");
		passed++;
	}
	else
		printf("\033[0;31m✗ normal string failed\033[0m\n");

	if (ft_strlen("") == 0)
	{
		printf("\033[0;32m✓ empty string passed\033[0m\n");
		passed++;
	}
	else
		printf("\033[0;31m✗ empty string failed\033[0m\n");

	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	if (passed == 2)
		printf("\033[1;32m✨ All tests passed! (%d/2)\033[0m\n", passed);
	else
		printf("\033[1;33m⚠ Result: %d/2 tests passed\033[0m\n", passed);
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	return (passed == 2);
}
