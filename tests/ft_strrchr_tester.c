#include "../src/libft.h"
#include <stdio.h>
#include <string.h>

int	main(void)
{
	const char	*text = "banana";
	int			passed;

	printf("\n\033[1;36m      🧪 Testing ft_strrchr Function     \033[0m\n\n");
	passed = ft_strrchr(text, 'a') == strrchr(text, 'a');
	passed = passed && ft_strrchr(text, '\0') == strrchr(text, '\0');
	passed = passed && ft_strrchr(text, 'x') == NULL;
	printf("%s\n", passed ? "\033[0;32m✓ ft_strrchr passed\033[0m" : "\033[0;31m✗ ft_strrchr failed\033[0m");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	if (passed)
		printf("\033[1;32m✨ All tests passed! (1/1)\033[0m\n");
	else
		printf("\033[1;33m⚠ Result: 0/1 tests passed\033[0m\n");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	return (passed);
}
