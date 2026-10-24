/*
** Entry point, started by libt.sh: with test names, runs them and exits
** with 0 when they all passed; without, opens the menu.
*/

#include "runner.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void	usage(void)
{
	printf("Usage: libt [--asan] [-v] [test...]\n\n"
		"  libt                  open the menu\n"
		"  libt all              run every test\n"
		"  libt part1|part2|part3\n"
		"                        run one part of the subject\n"
		"  libt strlen split.c   run some tests (ft_ and .c are optional)\n"
		"  libt -v all           show every case, not only the failures\n"
		"  libt --asan ...       also build with AddressSanitizer/UBSan\n");
}

int	main(int argc, char **argv)
{
	int	i;
	int	ok;

	if (getenv("LIBT_SRC"))
		g_runner.src = getenv("LIBT_SRC");
	if (getenv("LIBT_BUILD"))
		g_runner.build = getenv("LIBT_BUILD");
	g_runner.color = isatty(STDOUT_FILENO) && !getenv("NO_COLOR");
	load_states();
	i = 1;
	while (i < argc && argv[i][0] == '-' && strcmp(argv[i], "-a"))
	{
		if (strcmp(argv[i], "-v") && strcmp(argv[i], "--verbose"))
		{
			usage();
			return (strcmp(argv[i], "-h") && strcmp(argv[i], "--help"));
		}
		g_runner.verbose = 1;
		i++;
	}
	if (i == argc)
	{
		interactive();
		return (0);
	}
	ok = 1;
	while (i < argc)
		ok &= select_word(argv[i++]);
	return (!(ok && run_selected()));
}
