#include "../src/libft.h"
#include <stdio.h>
#include <string.h>

static int check(const char *label, const char *got, const char *expected)
{
	if (strcmp(got, expected) == 0)
	{
		printf("\033[0;32m✓ %s passed\033[0m\n", label);
		return (1);
	}
	printf("\033[0;31m✗ %s failed\033[0m\n", label);
	printf("  expected: \"%s\"\n", expected);
	printf("  got:      \"%s\"\n", got);
	return (0);
}

int main(void)
{
	char	buf[32];
	int		passed;

	printf("\n\033[1;36m      🧪 Testing ft_memmove Function    \033[0m\n\n");

	passed = 0;
	memcpy(buf, "0123456789", 11);
	ft_memmove(buf + 2, buf, 8);
	passed += check("ft_memmove overlap", buf, "0101234567");

	memcpy(buf, "abcdefghij", 11);
	ft_memmove(buf, buf + 2, 8);
	passed += check("ft_memmove reverse overlap", buf, "cdefghijij");

	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	if (passed == 2)
		printf("\033[1;32m✨ All tests passed! (%d/2)\033[0m\n", passed);
	else
		printf("\033[1;33m⚠ Result: %d/2 tests passed\033[0m\n", passed);
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	return (passed == 2);
}
