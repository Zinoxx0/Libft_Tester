#include "../src/libft.h"
#include <stdio.h>
#include <string.h>

int	main(void)
{
	t_list	*list;
	t_list	*node;
	int		passed;

	printf("\n\033[1;36m      🧪 Testing ft_lstadd_back Function     \033[0m\n\n");
	list = ft_lstnew("first");
	node = ft_lstnew("second");
	ft_lstadd_back(&list, node);
	passed = list && list->next == node
		&& strcmp(list->next->content, "second") == 0;
	free(list->next);
	free(list);
	printf("%s\n", passed ? "\033[0;32m✓ ft_lstadd_back passed\033[0m" : "\033[0;31m✗ ft_lstadd_back failed\033[0m");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	if (passed)
		printf("\033[1;32m✨ All tests passed! (1/1)\033[0m\n");
	else
		printf("\033[1;33m⚠ Result: 0/1 tests passed\033[0m\n");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	return (passed);
}
