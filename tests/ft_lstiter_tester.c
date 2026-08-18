#include "../src/libft.h"
#include <stdio.h>
#include <string.h>

static void	mark(void *content)
{
	((char *)content)[0] = 'X';
}

int	main(void)
{
	char	first[] = "one";
	char	second[] = "two";
	t_list	node2 = {second, NULL};
	t_list	node1 = {first, &node2};
	int		passed;

	printf("\n\033[1;36m      🧪 Testing ft_lstiter Function     \033[0m\n\n");
	ft_lstiter(&node1, mark);
	passed = first[0] == 'X' && second[0] == 'X';
	printf("%s\n", passed ? "\033[0;32m✓ ft_lstiter passed\033[0m" : "\033[0;31m✗ ft_lstiter failed\033[0m");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	if (passed)
		printf("\033[1;32m✨ All tests passed! (1/1)\033[0m\n");
	else
		printf("\033[1;33m⚠ Result: 0/1 tests passed\033[0m\n");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	return (passed);
}
