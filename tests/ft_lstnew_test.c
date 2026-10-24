#include "libft.h"
#include "tester.h"

static void	stores_content(void)
{
	int		value;
	t_list	*node;

	doing("ft_lstnew(&value)");
	node = ft_lstnew(&value);
	if (!check(node != NULL, "returned NULL"))
		return ;
	check(node->content == &value,
		"content isn't the pointer given to ft_lstnew");
	check(node->next == NULL, "next isn't NULL");
	check_alloc(node, sizeof(t_list), "the node");
	free(node);
}

static void	null_content(void)
{
	t_list	*node;

	doing("ft_lstnew(NULL)");
	node = ft_lstnew(NULL);
	if (!check(node != NULL, "returned NULL"))
		return ;
	check(node->content == NULL, "content isn't NULL");
	check(node->next == NULL, "next isn't NULL");
	free(node);
}

static void	*call_lstnew(void *arg)
{
	return (ft_lstnew(arg));
}

static void	malloc_fails(void)
{
	doing("ft_lstnew(\"content\")");
	check_malloc_failures(call_lstnew, "content", free, 1);
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_lstnew");
	PROTOTYPE(t_list *, ft_lstnew, (void *));
	test("stores the content pointer, next is NULL", stores_content);
	test("NULL content", null_content);
	test("returns NULL when malloc fails", malloc_fails);
	return (done());
}
