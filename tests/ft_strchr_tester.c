#include "../src/libft.h"
#include <stdio.h>
#include <string.h>

int	main(void)
{
	const char	*text = "libft";
	int			passed;

	printf("\n\033[1;36m      🧪 Testing ft_strchr Function     \033[0m\n\n");
	passed = ft_strchr(text, 'f') == strchr(text, 'f');
	passed = passed && ft_strchr(text, '\0') == strchr(text, '\0');
	passed = passed && ft_strchr(text, 'x') == NULL;
	printf("%s\n", passed ? "\033[0;32m✓ ft_strchr passed\033[0m" : "\033[0;31m✗ ft_strchr failed\033[0m");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	if (passed)
		printf("\033[1;32m✨ All tests passed! (1/1)\033[0m\n");
	else
		printf("\033[1;33m⚠ Result: 0/1 tests passed\033[0m\n");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	return (passed);
}
