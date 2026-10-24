#include "libft.h"
#include "tester.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void	check_itoa(int n)
{
	char	want[16];
	char	*s;

	snprintf(want, sizeof(want), "%d", n);
	doing("ft_itoa(%d)", n);
	s = ft_itoa(n);
	if (!check_str(s, want, "result") || !s)
		return ;
	check_alloc(s, strlen(want) + 1, "the result");
	free(s);
}

static void	zero(void)
{
	check_itoa(0);
}

static void	positive(void)
{
	check_itoa(1);
	check_itoa(9);
	check_itoa(10);
	check_itoa(42);
	check_itoa(100);
	check_itoa(12345);
}

static void	negative(void)
{
	check_itoa(-1);
	check_itoa(-9);
	check_itoa(-10);
	check_itoa(-42);
	check_itoa(-100);
	check_itoa(-12345);
}

static void	limits(void)
{
	check_itoa(INT_MAX);
	check_itoa(INT_MIN);
	check_itoa(INT_MAX - 1);
	check_itoa(INT_MIN + 1);
}

static void	powers_of_ten(void)
{
	int	p;

	p = 1;
	while (p <= 1000000000)
	{
		check_itoa(p - 1);
		check_itoa(p);
		check_itoa(-p);
		check_itoa(-p + 1);
		if (p == 1000000000)
			break ;
		p *= 10;
	}
}

static void	random_numbers(void)
{
	int	i;

	srand(42);
	i = 0;
	while (i++ < 1000)
		check_itoa(rand() % 2 ? rand() : -rand());
}

static void	*call_itoa(void *arg)
{
	(void)arg;
	return (ft_itoa(-12345));
}

static void	malloc_fails(void)
{
	doing("ft_itoa(-12345)");
	check_malloc_failures(call_itoa, NULL, free, 1);
}

int	main(int argc, char **argv)
{
	suite(argc, argv, "ft_itoa");
	PROTOTYPE(char *, ft_itoa, (int));
	test("zero", zero);
	test("positive numbers", positive);
	test("negative numbers", negative);
	test("INT_MAX and INT_MIN", limits);
	test("powers of ten and their neighbours", powers_of_ten);
	test("1000 random numbers", random_numbers);
	test("returns NULL when malloc fails", malloc_fails);
	return (done());
}
