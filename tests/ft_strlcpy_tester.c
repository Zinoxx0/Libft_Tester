#include "../src/libft.h"
#include <stdio.h>
#include <string.h>

int	main(void)
{
	char	got[5];
	char	expected[5];
	size_t	got_len;
	size_t	expected_len;
	int		passed;

	printf("\n\033[1;36m      🧪 Testing ft_strlcpy Function     \033[0m\n\n");
	got_len = ft_strlcpy(got, "testing", sizeof(got));
	expected_len = strlcpy(expected, "testing", sizeof(expected));
	passed = got_len == expected_len && strcmp(got, expected) == 0;
	printf("%s\n", passed ? "\033[0;32m✓ ft_strlcpy passed\033[0m" : "\033[0;31m✗ ft_strlcpy failed\033[0m");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	if (passed)
		printf("\033[1;32m✨ All tests passed! (1/1)\033[0m\n");
	else
		printf("\033[1;33m⚠ Result: 0/1 tests passed\033[0m\n");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	return (passed);
}
