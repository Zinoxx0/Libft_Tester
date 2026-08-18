#include "../src/libft.h"
#include <stdio.h>
#include <string.h>

int	main(void)
{
	char	*value;
	int		passed;

	printf("\n\033[1;36m      🧪 Testing ft_itoa Function     \033[0m\n\n");
	passed = 1;
	value = ft_itoa(0);
	passed = value && strcmp(value, "0") == 0;
	free(value);
	value = ft_itoa(-2147483648);
	passed = passed && value && strcmp(value, "-2147483648") == 0;
	free(value);
	value = ft_itoa(2147483647);
	passed = passed && value && strcmp(value, "2147483647") == 0;
	free(value);
	printf("%s\n", passed ? "\033[0;32m✓ ft_itoa passed\033[0m" : "\033[0;31m✗ ft_itoa failed\033[0m");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	if (passed)
		printf("\033[1;32m✨ All tests passed! (1/1)\033[0m\n");
	else
		printf("\033[1;33m⚠ Result: 0/1 tests passed\033[0m\n");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	return (passed);
}
