#include "libft.h"
#include "tester.h"
#include "lists.h"
#include <string.h>

static int	g_f_calls;
static int	g_del_calls;

/* "0" -> "0!", in a new block */
static void	*exclaim(void *content)
{
	char	*copy;
	size_t	len;

	g_f_calls++;
	len = strlen(content);
	copy = malloc(len + 2);
	if (!copy)
		return (NULL);
	memcpy(copy, content, len);
	memcpy(copy + len, "!", 2);
	return (copy);
}

static void	*same(void *content)
{
	return (content);
}

static void	del(void *content)
{
	g_del_calls++;
	free(content);
}

static void	del_nothing(void *content)
{
	(void)content;
}

static void	free_nodes(void *list)
{
	t_list	*node;
	t_list	*next;

	node = list;
	while (node)
	{
		next = node->next;
		free(node);
		node = next;
	}
}

static void	release_list(void *list)
{
	free_list(list);
}

static void	maps(void)
{
	t_list	*list;
	t_list	*mapped;
	int		i;

	list = new_list(3);
	doing("ft_lstmap(\"0 1 2\", f, del) with f(\"0\") = \"0!\"");
	mapped = ft_lstmap(list, exclaim, del);
	check_str(list_str(mapped), "0! 1! 2!", "the new list");
	check_str(list_str(list), "0 1 2", "the original list");
	check_int(g_f_calls, 3, "number of calls to f");
	check_int(g_del_calls, 0, "number of calls to del");
	i = -1;
	while (++i < 3 && nth(mapped, i))
	{
		check(nth(mapped, i) != nth(list, i), "node %d of the new list is "
			"a node of the original list", i);
		check_alloc(nth(mapped, i), sizeof(t_list), "a new node");
	}
	free_list(mapped);
	free_list(list);
}

static void	empty(void)
{
	doing("ft_lstmap(NULL, f, del)");
	check(ft_lstmap(NULL, exclaim, del) == NULL, "didn't return NULL");
	check_int(g_f_calls, 0, "number of calls to f");
}

static void	long_list(void)
{
	t_list	*list;
	t_list	*mapped;

	list = new_list(1000);
	doing("ft_lstmap(<list of 1000 nodes>, f, del)");
	mapped = ft_lstmap(list, exclaim, del);
	check_int(g_f_calls, 1000, "number of calls to f");
	if (check(nth(mapped, 999) && !nth(mapped, 1000), "the new list doesn't "
			"have 1000 nodes"))
		check_str(nth(mapped, 999)->content, "999!", "last content");
	free_list(mapped);
	free_list(list);
}

static void	*call_lstmap_same(void *list)
{
	return (ft_lstmap(list, same, del_nothing));
}

static void	*call_lstmap(void *list)
{
	return (ft_lstmap(list, exclaim, del));
}

static void	malloc_fails(void)
{
	t_list	*list;

	list = new_list(3);
	doing("ft_lstmap(\"0 1 2\", f, del) with f(\"0\") = \"0\"");
	check_malloc_failures(call_lstmap_same, list, free_nodes, 1);
	doing("ft_lstmap(\"0 1 2\", f, del) with f(\"0\") = \"0!\" (malloc'd)");
	check_malloc_failures(call_lstmap, list, release_list, 0);
	free_list(list);
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_lstmap");
	PROTOTYPE(t_list *, ft_lstmap, (t_list *, void *(*)(void *),
			void (*)(void *)));
	test("builds a new list of f(content), in order", maps);
	test("empty list gives NULL", empty);
	test("1000 nodes", long_list);
	test("returns NULL and frees everything when malloc fails",
		malloc_fails);
	return (done());
}
