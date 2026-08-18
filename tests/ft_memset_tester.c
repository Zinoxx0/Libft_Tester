#include "../src/libft.h"
#include <stdio.h>
#include <string.h>

int	main(void)
{
	char	got[8];
	char	expected[8];
	int		passed;

	printf("\n\033[1;36m      🧪 Testing ft_memset Function     \033[0m\n\n");
	ft_memset(got, 'A', sizeof(got));
	memset(expected, 'A', sizeof(expected));
	passed = memcmp(got, expected, sizeof(got)) == 0;
	printf("%s\n", passed ? "\033[0;32m✓ ft_memset passed\033[0m" : "\033[0;31m✗ ft_memset failed\033[0m");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	if (passed)
		printf("\033[1;32m✨ All tests passed! (1/1)\033[0m\n");
	else
		printf("\033[1;33m⚠ Result: 0/1 tests passed\033[0m\n");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	return (passed);
}
