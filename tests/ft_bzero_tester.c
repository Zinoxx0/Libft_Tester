#include "../src/libft.h"
#include <stdio.h>

static int	test(size_t length)
{
	char	buf[8];
	int	i;

	for (i = 0; i < 8; i++)
		buf[i] = 'x';
	ft_bzero(buf, length);
	for (i = 0; i < 8; i++)
	{
		if (i < (int)length && buf[i] != 0)
			return (0);
		if (i >= (int)length && buf[i] != 'x')
			return (0);
	}
	printf("\033[0;32m✓ ft_bzero(buf, %zu) passed\033[0m\n", length);
	return (1);
}

int	main(void)
{
	int	passed;

	printf("\n\033[1;36m      🧪 Testing ft_bzero Function     \033[0m\n\n");
	passed = 0;
	passed += test(0);
	passed += test(1);
	passed += test(4);
	passed += test(8);
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	if (passed == 4)
		printf("\033[1;32m✨ All tests passed! (%d/4)\033[0m\n", passed);
	else
		printf("\033[1;33m⚠ Result: %d/4 tests passed\033[0m\n", passed);
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	return (passed == 4);
}
