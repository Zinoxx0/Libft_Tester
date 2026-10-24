#include "libft.h"
#include "tester.h"
#include "lists.h"

static void	empty_list(void)
{
	t_list	*list;
	t_list	*node;
	t_list	old;

	list = NULL;
	node = new_named("new");
	node->next = &old;
	doing("ft_lstadd_front(&list, node) with list = NULL");
	ft_lstadd_front(&list, node);
	check(list == node, "list doesn't point to the node");
	check(node->next == NULL, "node->next isn't NULL (the old list)");
	node->next = NULL;
	free_list(node);
}

static void	in_front(void)
{
	t_list	*list;
	t_list	*node;

	list = new_list(3);
	node = new_named("new");
	doing("ft_lstadd_front(&list, node) with list = \"0 1 2\"");
	ft_lstadd_front(&list, node);
	check(list == node, "list doesn't point to the node");
	check_str(list_str(list), "new 0 1 2", "list");
	free_list(list);
}

static void	several(void)
{
	t_list	*list;

	list = NULL;
	doing("ft_lstadd_front 3 times, starting from an empty list");
	ft_lstadd_front(&list, new_named("a"));
	ft_lstadd_front(&list, new_named("b"));
	ft_lstadd_front(&list, new_named("c"));
	check_str(list_str(list), "c b a", "list");
	free_list(list);
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_lstadd_front");
	PROTOTYPE(void, ft_lstadd_front, (t_list **, t_list *));
	test("onto an empty list", empty_list);
	test("in front of a list", in_front);
	test("several times", several);
	return (done());
}
