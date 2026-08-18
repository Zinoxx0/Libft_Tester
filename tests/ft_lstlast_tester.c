#include "../src/libft.h"
#include <stdio.h>
#include <string.h>

int	main(void)
{
	t_list	*list;
	t_list	*last;
	int		passed;

	printf("\n\033[1;36m      🧪 Testing ft_lstlast Function     \033[0m\n\n");
	list = ft_lstnew("one");
	ft_lstadd_back(&list, ft_lstnew("two"));
	last = ft_lstlast(list);
	passed = last && strcmp(last->content, "two") == 0
		&& ft_lstlast(NULL) == NULL;
	ft_lstclear(&list, NULL);
	printf("%s\n", passed ? "\033[0;32m✓ ft_lstlast passed\033[0m" : "\033[0;31m✗ ft_lstlast failed\033[0m");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	if (passed)
		printf("\033[1;32m✨ All tests passed! (1/1)\033[0m\n");
	else
		printf("\033[1;33m⚠ Result: 0/1 tests passed\033[0m\n");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	return (passed);
}
