#include "libft.h"
#include "tester.h"
#include <stdint.h>

static void	check_calloc(size_t count, size_t size)
{
	unsigned char	*p;
	size_t			i;

	doing("ft_calloc(%zu, %zu)", count, size);
	p = ft_calloc(count, size);
	if (!check(p != NULL, "returned NULL")
		|| !check_alloc(p, count * size, "the result"))
		return ;
	i = 0;
	while (i < count * size && p[i] == 0)
		i++;
	check(i == count * size, "byte %zu isn't 0", i);
	free(p);
}

static void	zeroed(void)
{
	check_calloc(1, 1);
	check_calloc(10, 1);
	check_calloc(5, 4);
	check_calloc(3, sizeof(long));
	check_calloc(1000, 8);
	check_calloc(1, 100000);
}

/* The subject: a unique pointer that can be passed to free() */
static void	zero_size(void)
{
	const size_t	args[][2] = {{0, 5}, {5, 0}, {0, 0}};
	void			*p[3];
	size_t			i;

	i = 0;
	while (i < 3)
	{
		doing("ft_calloc(%zu, %zu)", args[i][0], args[i][1]);
		p[i] = ft_calloc(args[i][0], args[i][1]);
		if (check(p[i] != NULL, "returned NULL"))
			check_alloc(p[i], 0, "the result");
		i++;
	}
	doing("three ft_calloc calls with a 0 argument");
	check(!p[0] || !p[1] || !p[2] || (p[0] != p[1] && p[1] != p[2]
			&& p[0] != p[2]), "returned the same pointer twice");
	while (i-- > 0)
		free(p[i]);
}

static void	overflow(void)
{
	const size_t	args[][2] = {{SIZE_MAX, 2}, {2, SIZE_MAX},
	{SIZE_MAX / 2 + 2, 2}, {SIZE_MAX, SIZE_MAX}, {SIZE_MAX / 4 + 1, 4}};
	size_t			i;
	void			*p;

	i = 0;
	while (i < sizeof(args) / sizeof(*args))
	{
		doing("ft_calloc(%zu, %zu)", args[i][0], args[i][1]);
		p = ft_calloc(args[i][0], args[i][1]);
		if (!check(p == NULL, "count * size overflows size_t: must return "
				"NULL, not a %zu-byte block", args[i][0] * args[i][1]))
			free(p);
		i++;
	}
}

static void	*call_calloc(void *arg)
{
	(void)arg;
	return (ft_calloc(4, 4));
}

static void	malloc_fails(void)
{
	doing("ft_calloc(4, 4)");
	check_malloc_failures(call_calloc, NULL, free, 1);
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_calloc");
	PROTOTYPE(void *, ft_calloc, (size_t, size_t));
	test("allocates count * size bytes set to 0", zeroed);
	test("count or size 0 gives a unique pointer free() accepts",
		zero_size);
	test("returns NULL when count * size overflows", overflow);
	test("returns NULL when malloc fails", malloc_fails);
	return (done());
}
