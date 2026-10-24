#include "libft.h"
#include "tester.h"
#include <limits.h>

static int	expected(int c)
{
	return (c >= 0 && c <= 127);
}

/* Checks every value from -1 (EOF) to 255 whose expected answer is want */
static void	check_values(int want)
{
	int	c;

	c = -1;
	while (c <= 255)
	{
		if (expected(c) == want)
		{
			doing("ft_isascii(%s)", show_char(c));
			check_int(ft_isascii(c), want, "return value (must be 1 or 0)");
		}
		c++;
	}
}

static void	accepted(void)
{
	check_values(1);
}

static void	rejected(void)
{
	check_values(0);
}

static void	far_values(void)
{
	const int	values[] = {INT_MIN, -129, -128, 256, 1000, INT_MAX};
	size_t		i;

	i = 0;
	while (i < sizeof(values) / sizeof(*values))
	{
		doing("ft_isascii(%d)", values[i]);
		check_int(ft_isascii(values[i]), 0, "return value");
		i++;
	}
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_isascii");
	PROTOTYPE(int, ft_isascii, (int));
	test("0 to 127", accepted);
	test("every other value from -1 to 255", rejected);
	test("values far outside 0-255", far_values);
	return (done());
}
