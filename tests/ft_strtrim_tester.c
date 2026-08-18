#include "../src/libft.h"
#include <stdio.h>
#include <string.h>

int	main(void)
{
	char	*trimmed;
	int		passed;

	printf("\n\033[1;36m      🧪 Testing ft_strtrim Function     \033[0m\n\n");
	trimmed = ft_strtrim("---libft---", "-");
	passed = trimmed && strcmp(trimmed, "libft") == 0;
	free(trimmed);
	trimmed = ft_strtrim("  hello  ", " ");
	passed = passed && trimmed && strcmp(trimmed, "hello") == 0;
	free(trimmed);
	printf("%s\n", passed ? "\033[0;32m✓ ft_strtrim passed\033[0m" : "\033[0;31m✗ ft_strtrim failed\033[0m");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	if (passed)
		printf("\033[1;32m✨ All tests passed! (1/1)\033[0m\n");
	else
		printf("\033[1;33m⚠ Result: 0/1 tests passed\033[0m\n");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	return (passed);
}
