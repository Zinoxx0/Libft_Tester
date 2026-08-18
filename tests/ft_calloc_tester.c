#include "../src/libft.h"
#include <stdio.h>

int	main(void)
{
	unsigned char	*buf;
	size_t		i;
	int			passed;

	printf("\n\033[1;36m      🧪 Testing ft_calloc Function     \033[0m\n\n");
	buf = ft_calloc(16, sizeof(unsigned char));
	passed = (buf != NULL);
	if (passed)
		for (i = 0; i < 16; i++)
			if (buf[i] != 0)
				passed = 0;
	free(buf);
	printf("%s\n", passed ? "\033[0;32m✓ ft_calloc passed\033[0m" : "\033[0;31m✗ ft_calloc failed\033[0m");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	if (passed)
		printf("\033[1;32m✨ All tests passed! (1/1)\033[0m\n");
	else
		printf("\033[1;33m⚠ Result: 0/1 tests passed\033[0m\n");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	return (passed);
}
