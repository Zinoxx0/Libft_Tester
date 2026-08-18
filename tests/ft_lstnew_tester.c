#include "../src/libft.h"
#include <stdio.h>
#include <string.h>

int	main(void)
{
	t_list	*node;
	int		passed;

	printf("\n\033[1;36m      🧪 Testing ft_lstnew Function     \033[0m\n\n");
	node = ft_lstnew("content");
	passed = node && node->content && strcmp(node->content, "content") == 0
		&& node->next == NULL;
	free(node);
	printf("%s\n", passed ? "\033[0;32m✓ ft_lstnew passed\033[0m" : "\033[0;31m✗ ft_lstnew failed\033[0m");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	if (passed)
		printf("\033[1;32m✨ All tests passed! (1/1)\033[0m\n");
	else
		printf("\033[1;33m⚠ Result: 0/1 tests passed\033[0m\n");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	return (passed);
}
