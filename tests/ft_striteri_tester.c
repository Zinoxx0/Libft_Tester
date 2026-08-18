#include "../src/libft.h"
#include <stdio.h>
#include <string.h>

static void	capitalize_even(unsigned int index, char *character)
{
	if (index % 2 == 0 && *character >= 'a' && *character <= 'z')
		*character -= 'a' - 'A';
}

int	main(void)
{
	char	text[] = "abcdef";
	int		passed;

	printf("\n\033[1;36m      🧪 Testing ft_striteri Function     \033[0m\n\n");
	ft_striteri(text, capitalize_even);
	passed = strcmp(text, "AbCdEf") == 0;
	printf("%s\n", passed ? "\033[0;32m✓ ft_striteri passed\033[0m" : "\033[0;31m✗ ft_striteri failed\033[0m");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	if (passed)
		printf("\033[1;32m✨ All tests passed! (1/1)\033[0m\n");
	else
		printf("\033[1;33m⚠ Result: 0/1 tests passed\033[0m\n");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	return (passed);
}
