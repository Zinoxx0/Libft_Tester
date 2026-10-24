#include "libft.h"
#include "tester.h"

static void	converted(void)
{
	int	c;

	c = 'a';
	while (c <= 'z')
	{
		doing("ft_toupper(%s)", show_char(c));
		check_int(ft_toupper(c), c - 'a' + 'A', "return value");
		c++;
	}
}

static void	unchanged(void)
{
	int	c;

	c = -1;
	while (c <= 255)
	{
		if (c < 'a' || c > 'z')
		{
			doing("ft_toupper(%s)", show_char(c));
			check_int(ft_toupper(c), c, "return value");
		}
		c++;
	}
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_toupper");
	PROTOTYPE(int, ft_toupper, (int));
	test("lowercase letters are converted", converted);
	test("every other value from -1 to 255 is returned unchanged", unchanged);
	return (done());
}
