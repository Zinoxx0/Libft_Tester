#include "../src/libft.h"
#include <stdio.h>
#include <string.h>

int	main(void)
{
	char	got[16];
	char	expected[16];
	int		passed;

	printf("\n\033[1;36m      🧪 Testing ft_memcpy Function     \033[0m\n\n");
	memset(got, 'x', sizeof(got));
	memset(expected, 'x', sizeof(expected));
	ft_memcpy(got, "hello", 6);
	memcpy(expected, "hello", 6);
	passed = memcmp(got, expected, sizeof(got)) == 0;
	printf("%s\n", passed ? "\033[0;32m✓ ft_memcpy passed\033[0m" : "\033[0;31m✗ ft_memcpy failed\033[0m");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	if (passed)
		printf("\033[1;32m✨ All tests passed! (1/1)\033[0m\n");
	else
		printf("\033[1;33m⚠ Result: 0/1 tests passed\033[0m\n");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	return (passed);
}
