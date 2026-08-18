#include "../src/libft.h"
#include <stdio.h>

static int	test(const char *label, int result)
{
	if (result)
		printf("\033[0;32m✓ %s passed\033[0m\n", label);
	else
		printf("\033[0;31m✗ %s failed\033[0m\n", label);
	return (result);
}

int	main(void)
{
	int	passed;

	printf("\n\033[1;36m      🧪 Testing ft_isalnum Function     \033[0m\n\n");
	passed = 0;
	passed += test("uppercase letter", ft_isalnum('A'));
	passed += test("digit", ft_isalnum('7'));
	passed += test("hyphen rejected", !ft_isalnum('-'));
	passed += test("null character rejected", !ft_isalnum(0));
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	if (passed == 4)
		printf("\033[1;32m✨ All tests passed! (%d/4)\033[0m\n", passed);
	else
		printf("\033[1;33m⚠ Result: %d/4 tests passed\033[0m\n", passed);
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	return (passed == 4);
}
