#include "../src/libft.h"
#include <stdio.h>
#include <string.h>

static void	*duplicate(void *content)
{
	return (ft_strdup((char *)content));
}

static void	free_content(void *content)
{
	free(content);
}

int	main(void)
{
	t_list	*list;
	t_list	*mapped;
	int		passed;

	printf("\n\033[1;36m      🧪 Testing ft_lstmap Function     \033[0m\n\n");
	list = ft_lstnew("one");
	ft_lstadd_back(&list, ft_lstnew("two"));
	mapped = ft_lstmap(list, duplicate, free_content);
	passed = mapped && ft_lstsize(mapped) == 2
		&& strcmp(mapped->content, "one") == 0
		&& strcmp(mapped->next->content, "two") == 0;
	ft_lstclear(&list, NULL);
	ft_lstclear(&mapped, free_content);
	printf("%s\n", passed ? "\033[0;32m✓ ft_lstmap passed\033[0m" : "\033[0;31m✗ ft_lstmap failed\033[0m");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	if (passed)
		printf("\033[1;32m✨ All tests passed! (1/1)\033[0m\n");
	else
		printf("\033[1;33m⚠ Result: 0/1 tests passed\033[0m\n");
	printf("\033[1;36m────────────────────────────────────────\033[0m\n");
	return (passed);
}
