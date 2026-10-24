#include "libft.h"
#include "tester.h"

static void	letters(void)
{
	const char	*out;
	size_t		len;
	int			fd;

	fd = capture();
	doing("ft_putchar_fd('a', fd) then ft_putchar_fd('Z', fd)");
	ft_putchar_fd('a', fd);
	ft_putchar_fd('Z', fd);
	out = captured(&len);
	check_mem(out, "aZ", len < 2 ? 2 : len, "written");
	check_size(len, 2, "bytes written");
}

static void	every_byte(void)
{
	unsigned char	want[256];
	const char		*out;
	size_t			len;
	int				fd;
	int				c;

	fd = capture();
	doing("ft_putchar_fd(c, fd) for every c from 0 to 255");
	c = 0;
	while (c < 256)
	{
		want[c] = c;
		ft_putchar_fd(c, fd);
		c++;
	}
	out = captured(&len);
	if (check_size(len, 256, "bytes written"))
		check_mem(out, want, 256, "written");
}

static void	bad_fd(void)
{
	size_t	len;

	capture();
	doing("ft_putchar_fd('a', -1)");
	ft_putchar_fd('a', -1);
	captured(&len);
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_putchar_fd");
	PROTOTYPE(void, ft_putchar_fd, (char, int));
	test("writes the char to fd", letters);
	test("every byte, '\\0' included", every_byte);
	test("fd -1 doesn't crash", bad_fd);
	return (done());
}
