#include "../src/libft.h"
#include <stdio.h>
#include <string.h>

static char	shift(unsigned int index, char character)
{
	(void)index;
	if (character >= 'a' && character <= 'z')
		return (character - 'a' + 'A');
	return (character);
}

int	main(void)
{
	char	*result;
	int		passed;

	printf("\n\033[1;36m      🧪 Testing ft_strmapi Function     \033[0m\n\n");
	result = ft_strmapi("hello!", shift);
	passed = result && strcmp(result, "HELLO!") == 0;
	free(result);
	printf("%s\n", passed ? "\033[0;32m✓ ft_strmapi passed\033[0m" : "\033[0;31m✗ ft_strmapi failed\033[0m");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	if (passed)
		printf("\033[1;32m✨ All tests passed! (1/1)\033[0m\n");
	else
		printf("\033[1;33m⚠ Result: 0/1 tests passed\033[0m\n");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	return (passed);
}
