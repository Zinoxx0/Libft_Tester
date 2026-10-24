#include "libft.h"
#include "tester.h"

static int	expected(int c)
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
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
			doing("ft_isalpha(%s)", show_char(c));
			check_int(ft_isalpha(c), want, "return value (must be 1 or 0)");
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

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_isalpha");
	PROTOTYPE(int, ft_isalpha, (int));
	test("letters a-z and A-Z", accepted);
	test("every other value from -1 to 255", rejected);
	return (done());
}
