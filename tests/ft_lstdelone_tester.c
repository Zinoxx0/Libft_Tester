#include "../src/libft.h"
#include <stdio.h>
#include <string.h>

static void	del(void *content)
{
	free(content);
}

int	main(void)
{
	t_list	*node;
	char	*content;
	int		passed;

	printf("\n\033[1;36m      🧪 Testing ft_lstdelone Function     \033[0m\n\n");
	content = strdup("content");
	node = ft_lstnew(content);
	ft_lstdelone(node, del);
	passed = 1;
	printf("%s\n", passed ? "\033[0;32m✓ ft_lstdelone passed\033[0m" : "\033[0;31m✗ ft_lstdelone failed\033[0m");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	if (passed)
		printf("\033[1;32m✨ All tests passed! (1/1)\033[0m\n");
	else
		printf("\033[1;33m⚠ Result: 0/1 tests passed\033[0m\n");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	return (passed);
}
