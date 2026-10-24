#include "libft.h"
#include "tester.h"
#include "lists.h"

static void	empty_list(void)
{
	t_list	*list;
	t_list	*node;

	list = NULL;
	node = new_named("new");
	doing("ft_lstadd_back(&list, node) with list = NULL");
	ft_lstadd_back(&list, node);
	check(list == node, "list doesn't point to the node");
	if (list == node)
		check_str(list_str(list), "new", "list");
	free_list(node);
}

static void	at_the_end(void)
{
	t_list	*list;
	t_list	*node;

	list = new_list(3);
	node = new_named("new");
	doing("ft_lstadd_back(&list, node) with list = \"0 1 2\"");
	ft_lstadd_back(&list, node);
	check(nth(list, 3) == node, "node isn't the 4th node");
	check_str(list_str(list), "0 1 2 new", "list");
	free_list(list);
}

static void	one_node(void)
{
	t_list	*list;

	list = new_list(1);
	doing("ft_lstadd_back(&list, node) with list = \"0\"");
	ft_lstadd_back(&list, new_named("new"));
	check_str(list_str(list), "0 new", "list");
	free_list(list);
}

static void	several(void)
{
	t_list	*list;

	list = NULL;
	doing("ft_lstadd_back 4 times, starting from an empty list");
	ft_lstadd_back(&list, new_named("a"));
	ft_lstadd_back(&list, new_named("b"));
	ft_lstadd_back(&list, new_named("c"));
	ft_lstadd_back(&list, new_named("d"));
	check_str(list_str(list), "a b c d", "list");
	free_list(list);
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_lstadd_back");
	PROTOTYPE(void, ft_lstadd_back, (t_list **, t_list *));
	test("onto an empty list", empty_list);
	test("onto a one-node list", one_node);
	test("at the end of a list", at_the_end);
	test("several times", several);
	return (done());
}
