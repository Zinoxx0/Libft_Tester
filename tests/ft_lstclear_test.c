#include "libft.h"
#include "tester.h"
#include "lists.h"
#include <stdlib.h>

static int	g_del_calls;
static int	g_deleted[1000];

/* Contents are "0", "1", ...: counts how many times each one is deleted */
static void	del(void *content)
{
	int	i;

	i = atoi(content);
	if (i >= 0 && i < 1000)
		g_deleted[i]++;
	g_del_calls++;
	free(content);
}

static void	check_clear(int n)
{
	t_list	*list;
	int		i;

	list = new_list(n);
	doing("ft_lstclear(&list, del) on a list of %d nodes", n);
	ft_lstclear(&list, del);
	check(list == NULL, "the list pointer isn't NULL afterwards");
	check_int(g_del_calls, n, "number of calls to del");
	i = 0;
	while (i < n && check(g_deleted[i] == 1, "del got node %d's content "
			"%d times", i, g_deleted[i]))
		i++;
}

static void	whole_list(void)
{
	check_clear(5);
}

static void	empty(void)
{
	check_clear(0);
}

static void	single(void)
{
	check_clear(1);
}

static void	from_the_middle(void)
{
	t_list	*list;

	list = new_list(4);
	doing("ft_lstclear(&second->next, del) on \"0 1 2 3\"");
	ft_lstclear(&list->next->next, del);
	check_int(g_del_calls, 2, "number of calls to del");
	check_str(list_str(list), "0 1", "the list afterwards");
	free_list(list);
}

static void	long_list(void)
{
	check_clear(1000);
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_lstclear");
	PROTOTYPE(void, ft_lstclear, (t_list **, void (*)(void *)));
	test("frees every node and content, sets the list to NULL", whole_list);
	test("empty list", empty);
	test("one node", single);
	test("from a node in the middle: earlier nodes stay", from_the_middle);
	test("1000 nodes", long_list);
	return (done());
}
