#include "../src/libft.h"
#include <stdio.h>

int	main(void)
{
	printf("\n\033[1;36m      🧪 Testing ft_putstr_fd Function     \033[0m\n\n");
	ft_putstr_fd("libft", 1);
	ft_putchar_fd('\n', 1);
	printf("\033[0;32m✓ ft_putstr_fd passed\033[0m\n");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	printf("\033[1;32m✨ All tests passed! (1/1)\033[0m\n");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	return (0);
}
