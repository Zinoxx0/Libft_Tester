#include "libft.h"
#include "tester.h"
#include "lists.h"
#include <stdlib.h>

static int	g_calls;
static int	g_order[64];

static void	record(void *content)
{
	if (g_calls < 64)
		g_order[g_calls] = atoi(content);
	g_calls++;
}

static void	bump(void *content)
{
	((char *)content)[0]++;
}

static void	in_order(void)
{
	t_list	*list;
	int		i;

	list = new_list(5);
	doing("ft_lstiter(\"0 1 2 3 4\", f)");
	ft_lstiter(list, record);
	check_int(g_calls, 5, "number of calls to f");
	i = 0;
	while (i < 5 && i < g_calls && check(g_order[i] == i,
			"call %d to f got node %d's content", i + 1, g_order[i]))
		i++;
	free_list(list);
}

static void	changes_contents(void)
{
	t_list	*list;
	t_list	*nodes[3];
	int		i;

	list = new_list(3);
	i = -1;
	while (++i < 3)
		nodes[i] = nth(list, i);
	doing("ft_lstiter(\"0 1 2\", f) with f adding 1 to each content");
	ft_lstiter(list, bump);
	check_str(list_str(list), "1 2 3", "the list afterwards");
	i = -1;
	while (++i < 3)
		check(nth(list, i) == nodes[i], "node %d was replaced", i);
	free_list(list);
}

static void	empty(void)
{
	doing("ft_lstiter(NULL, f)");
	ft_lstiter(NULL, record);
	check_int(g_calls, 0, "number of calls to f");
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_lstiter");
	PROTOTYPE(void, ft_lstiter, (t_list *, void (*)(void *)));
	test("calls f on every content, in order", in_order);
	test("f can change the contents, the nodes stay", changes_contents);
	test("empty list: f is never called", empty);
	return (done());
}
