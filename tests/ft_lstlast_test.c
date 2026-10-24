#include "libft.h"
#include "tester.h"
#include "lists.h"

static void	check_lstlast(int n)
{
	t_list	*list;

	list = new_list(n);
	doing("ft_lstlast(<list of %d nodes>)", n);
	check(ft_lstlast(list) == nth(list, n - 1),
		"didn't return the last node");
	free_list(list);
}

static void	empty(void)
{
	doing("ft_lstlast(NULL)");
	check(ft_lstlast(NULL) == NULL, "didn't return NULL");
}

static void	short_lists(void)
{
	check_lstlast(1);
	check_lstlast(2);
	check_lstlast(5);
}

static void	long_list(void)
{
	check_lstlast(1000);
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_lstlast");
	PROTOTYPE(t_list *, ft_lstlast, (t_list *));
	test("NULL gives NULL", empty);
	test("short lists", short_lists);
	test("1000 nodes", long_list);
	return (done());
}
