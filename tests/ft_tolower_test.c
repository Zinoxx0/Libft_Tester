#include "libft.h"
#include "tester.h"

static void	converted(void)
{
	int	c;

	c = 'A';
	while (c <= 'Z')
	{
		doing("ft_tolower(%s)", show_char(c));
		check_int(ft_tolower(c), c - 'A' + 'a', "return value");
		c++;
	}
}

static void	unchanged(void)
{
	int	c;

	c = -1;
	while (c <= 255)
	{
		if (c < 'A' || c > 'Z')
		{
			doing("ft_tolower(%s)", show_char(c));
			check_int(ft_tolower(c), c, "return value");
		}
		c++;
	}
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_tolower");
	PROTOTYPE(int, ft_tolower, (int));
	test("uppercase letters are converted", converted);
	test("every other value from -1 to 255 is returned unchanged", unchanged);
	return (done());
}
