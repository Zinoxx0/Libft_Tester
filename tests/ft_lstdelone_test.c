#include "libft.h"
#include "tester.h"
#include "lists.h"

static int	g_del_calls;
static void	*g_del_arg;

static void	del(void *content)
{
	g_del_calls++;
	g_del_arg = content;
	free(content);
}

static void	frees_node(void)
{
	t_list	*node;
	void	*content;

	node = new_named("content");
	content = node->content;
	doing("ft_lstdelone(node, del)");
	ft_lstdelone(node, del);
	check_int(g_del_calls, 1, "number of calls to del");
	check(g_del_arg == content, "del didn't get the node's content");
}

static void	keeps_next(void)
{
	t_list	*list;
	t_list	*second;

	list = new_list(3);
	second = list->next;
	doing("ft_lstdelone(first node, del) on \"0 1 2\"");
	ft_lstdelone(list, del);
	check_int(g_del_calls, 1, "number of calls to del");
	check_str(list_str(second), "1 2", "the rest of the list");
	free_list(second);
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_lstdelone");
	PROTOTYPE(void, ft_lstdelone, (t_list *, void (*)(void *)));
	test("calls del on the content and frees the node", frees_node);
	test("doesn't touch the next node", keeps_next);
	return (done());
}
