#include "../src/libft.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int	test(const char *input)
{
	char	*got;
	int		ok;

	got = ft_strdup(input);
	ok = got && strcmp(got, input) == 0;
	if (ok)
		printf("\033[0;32m✓ ft_strdup(\"%s\") passed\033[0m\n", input);
	else
		printf("\033[0;31m✗ ft_strdup(\"%s\") failed\033[0m\n", input);
	free(got);
	return (ok);
}

int	main(void)
{
	int	passed;

	printf("\n\033[1;36m      🧪 Testing ft_strdup Function     \033[0m\n\n");

	passed = 0;
	passed += test("hello");
	passed += test("");
	passed += test("42 Lisboa");

	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	if (passed == 3)
		printf("\033[1;32m✨ All tests passed! (%d/3)\033[0m\n", passed);
	else
		printf("\033[1;33m⚠ Result: %d/3 tests passed\033[0m\n", passed);
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	return (passed == 3);
}
