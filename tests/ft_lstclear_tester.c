#include "../src/libft.h"
#include <stdio.h>

int	main(void)
{
	t_list	*list;
	int		passed;

	printf("\n\033[1;36m      🧪 Testing ft_lstclear Function     \033[0m\n\n");
	list = ft_lstnew("one");
	ft_lstadd_back(&list, ft_lstnew("two"));
	ft_lstclear(&list, NULL);
	passed = list == NULL;
	printf("%s\n", passed ? "\033[0;32m✓ ft_lstclear passed\033[0m" : "\033[0;31m✗ ft_lstclear failed\033[0m");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	if (passed)
		printf("\033[1;32m✨ All tests passed! (1/1)\033[0m\n");
	else
		printf("\033[1;33m⚠ Result: 0/1 tests passed\033[0m\n");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	return (passed);
}
