#include "libft.h"
#include "tester.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void	check_putnbr(int n)
{
	char		want[16];
	const char	*out;
	size_t		len;
	int			fd;

	snprintf(want, sizeof(want), "%d", n);
	fd = capture();
	doing("ft_putnbr_fd(%d, fd)", n);
	ft_putnbr_fd(n, fd);
	out = captured(&len);
	if (check_size(len, strlen(want), "bytes written"))
		check_mem(out, want, len, "written");
}

static void	zero(void)
{
	check_putnbr(0);
}

static void	positive(void)
{
	check_putnbr(7);
	check_putnbr(10);
	check_putnbr(42);
	check_putnbr(100);
	check_putnbr(987654);
}

static void	negative(void)
{
	check_putnbr(-7);
	check_putnbr(-10);
	check_putnbr(-42);
	check_putnbr(-100);
	check_putnbr(-987654);
}

static void	limits(void)
{
	check_putnbr(INT_MAX);
	check_putnbr(INT_MIN);
	check_putnbr(INT_MIN + 1);
}

static void	random_numbers(void)
{
	int	i;

	srand(42);
	i = 0;
	while (i++ < 300)
		check_putnbr(rand() % 2 ? rand() : -rand());
}

static void	bad_fd(void)
{
	size_t	len;

	capture();
	doing("ft_putnbr_fd(-42, -1)");
	ft_putnbr_fd(-42, -1);
	captured(&len);
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_putnbr_fd");
	PROTOTYPE(void, ft_putnbr_fd, (int, int));
	test("zero", zero);
	test("positive numbers", positive);
	test("negative numbers", negative);
	test("INT_MAX and INT_MIN", limits);
	test("300 random numbers", random_numbers);
	test("fd -1 doesn't crash", bad_fd);
	return (done());
}
