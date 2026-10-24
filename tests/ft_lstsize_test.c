#include "libft.h"
#include "tester.h"
#include "lists.h"

static void	check_lstsize(int n)
{
	t_list	*list;

	list = new_list(n);
	doing("ft_lstsize(<list of %d nodes>)", n);
	check_int((long)ft_lstsize(list), n, "return value");
	free_list(list);
}

static void	empty(void)
{
	check_lstsize(0);
}

static void	short_lists(void)
{
	check_lstsize(1);
	check_lstsize(2);
	check_lstsize(5);
}

static void	long_list(void)
{
	check_lstsize(1000);
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_lstsize");
	PROTOTYPE(unsigned int, ft_lstsize, (t_list *));
	test("NULL is an empty list", empty);
	test("short lists", short_lists);
	test("1000 nodes", long_list);
	return (done());
}
