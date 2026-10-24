/*
** Libft Tester runner: runs the test binaries built by the Makefile, one by
** one, by group or all of them, from the command line or from a menu.
**
** LIBT_SRC is the libft directory (set by libt.sh), LIBT_BUILD the build
** directory. A test binary exits with 0 when every case passed.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define RED		"\033[31m"
#define GREEN	"\033[32m"
#define YELLOW	"\033[33m"
#define BOLD	"\033[1m"
#define DIM		"\033[2m"
#define RESET	"\033[0m"

#define COLUMN	25

typedef enum e_group
{
	PART1,
	PART2,
	PART3,
	GROUPS
}	t_group;

typedef enum e_state
{
	MISSING,
	BROKEN,
	READY
}	t_state;

typedef enum e_result
{
	NOT_RUN,
	PASSED,
	FAILED
}	t_result;

typedef struct s_func
{
	const char	*name;
	t_group		group;
	t_state		state;
	t_result	result;
	int			selected;
}	t_func;

static t_func	g_funcs[] = {
	{"ft_isalpha", PART1, 0, 0, 0}, {"ft_isdigit", PART1, 0, 0, 0},
	{"ft_isalnum", PART1, 0, 0, 0}, {"ft_isascii", PART1, 0, 0, 0},
	{"ft_isprint", PART1, 0, 0, 0}, {"ft_strlen", PART1, 0, 0, 0},
	{"ft_memset", PART1, 0, 0, 0}, {"ft_bzero", PART1, 0, 0, 0},
	{"ft_memcpy", PART1, 0, 0, 0}, {"ft_memmove", PART1, 0, 0, 0},
	{"ft_strlcpy", PART1, 0, 0, 0}, {"ft_strlcat", PART1, 0, 0, 0},
	{"ft_toupper", PART1, 0, 0, 0}, {"ft_tolower", PART1, 0, 0, 0},
	{"ft_strchr", PART1, 0, 0, 0}, {"ft_strrchr", PART1, 0, 0, 0},
	{"ft_strncmp", PART1, 0, 0, 0}, {"ft_memchr", PART1, 0, 0, 0},
	{"ft_memcmp", PART1, 0, 0, 0}, {"ft_strnstr", PART1, 0, 0, 0},
	{"ft_atoi", PART1, 0, 0, 0}, {"ft_calloc", PART1, 0, 0, 0},
	{"ft_strdup", PART1, 0, 0, 0},
	{"ft_substr", PART2, 0, 0, 0}, {"ft_strjoin", PART2, 0, 0, 0},
	{"ft_strtrim", PART2, 0, 0, 0}, {"ft_split", PART2, 0, 0, 0},
	{"ft_itoa", PART2, 0, 0, 0}, {"ft_strmapi", PART2, 0, 0, 0},
	{"ft_striteri", PART2, 0, 0, 0}, {"ft_putchar_fd", PART2, 0, 0, 0},
	{"ft_putstr_fd", PART2, 0, 0, 0}, {"ft_putendl_fd", PART2, 0, 0, 0},
	{"ft_putnbr_fd", PART2, 0, 0, 0},
	{"ft_lstnew", PART3, 0, 0, 0}, {"ft_lstadd_front", PART3, 0, 0, 0},
	{"ft_lstsize", PART3, 0, 0, 0}, {"ft_lstlast", PART3, 0, 0, 0},
	{"ft_lstadd_back", PART3, 0, 0, 0}, {"ft_lstdelone", PART3, 0, 0, 0},
	{"ft_lstclear", PART3, 0, 0, 0}, {"ft_lstiter", PART3, 0, 0, 0},
	{"ft_lstmap", PART3, 0, 0, 0},
};

#define COUNT	(int)(sizeof(g_funcs) / sizeof(*g_funcs))

static const char	*g_group_names[GROUPS] = {"Part 1", "Part 2", "Part 3"};
static const char	*g_src = ".";
static const char	*g_build = "build";
static int			g_color;
static int			g_verbose;

static const char	*paint(const char *code)
{
	if (g_color)
		return (code);
	return ("");
}

static int	file_exists(const char *dir, const char *name, const char *ext)
{
	char	path[4096];

	snprintf(path, sizeof(path), "%s/%s%s", dir, name, ext);
	return (access(path, F_OK) == 0);
}

static void	load_states(void)
{
	int	i;

	i = 0;
	while (i < COUNT)
	{
		g_funcs[i].state = MISSING;
		if (file_exists(g_src, g_funcs[i].name, ".c"))
		{
			g_funcs[i].state = BROKEN;
			if (file_exists(g_build, "bin/", g_funcs[i].name))
				g_funcs[i].state = READY;
		}
		i++;
	}
}

static void	clear_screen(void)
{
	if (isatty(STDOUT_FILENO))
		printf("\033[H\033[2J");
}

/* ************************************************************************** */
/*  Running tests                                                             */
/* ************************************************************************** */

/* Prints the first non-empty build log of a function that didn't build */
static void	print_build_log(const t_func *f)
{
	const char	*suffixes[] = {".c.log", ".test.log", ".link.log"};
	char		path[4096];
	char		buf[4096];
	FILE		*log;
	size_t		n;
	size_t		i;

	printf("\n %s✗ %s didn't build:%s\n\n", paint(RED), f->name, paint(RESET));
	i = 0;
	while (i < sizeof(suffixes) / sizeof(*suffixes))
	{
		snprintf(path, sizeof(path), "%s/log/%s%s", g_build, f->name,
			suffixes[i++]);
		log = fopen(path, "r");
		if (!log)
			continue ;
		n = fread(buf, 1, sizeof(buf), log);
		fclose(log);
		if (n == 0)
			continue ;
		fwrite(buf, 1, n, stdout);
		if (n == sizeof(buf))
			printf("[...]\n");
		return ;
	}
	printf("   No build log: run libt again.\n");
}

static t_result	run_one(t_func *f, int quiet)
{
	char	path[4096];
	pid_t	pid;
	int		status;

	if (f->state == MISSING)
	{
		printf("  %s· %-18s not found in %s%s\n", paint(DIM), f->name, g_src,
			paint(RESET));
		return (NOT_RUN);
	}
	if (f->state == BROKEN)
	{
		if (quiet)
			printf("  %s✗ %-18s didn't build%s\n", paint(RED), f->name,
				paint(RESET));
		else
			print_build_log(f);
		return (f->result = FAILED);
	}
	snprintf(path, sizeof(path), "%s/bin/%s", g_build, f->name);
	fflush(stdout);
	pid = fork();
	if (pid == 0)
	{
		execl(path, f->name, quiet ? "-q" : (char *)NULL, (char *)NULL);
		perror(path);
		_exit(127);
	}
	if (pid < 0 || waitpid(pid, &status, 0) < 0)
		status = 1;
	if (!WIFEXITED(status) || WEXITSTATUS(status) > 1)
		printf("  %s✗ %s: the test itself stopped unexpectedly%s\n",
			paint(RED), f->name, paint(RESET));
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
		return (f->result = PASSED);
	return (f->result = FAILED);
}

static void	print_names(const char *label, const char *color, t_state state,
		t_result result)
{
	int	i;
	int	first;

	first = 1;
	i = 0;
	while (i < COUNT)
	{
		if (g_funcs[i].selected && g_funcs[i].state == state
			&& (state != READY || g_funcs[i].result == result))
		{
			if (first)
				printf("  %s%s:%s", paint(color), label, paint(RESET));
			printf(" %s", g_funcs[i].name);
			first = 0;
		}
		i++;
	}
	if (!first)
		printf("\n");
}

static void	print_summary(int passed, int ran)
{
	printf("\n  %s──────────────────────────────────────────%s\n",
		paint(DIM), paint(RESET));
	if (ran == 0)
		printf("  Nothing to test.\n");
	else if (passed == ran)
		printf("  %s%s✨ All %d functions passed!%s\n", paint(BOLD),
			paint(GREEN), ran, paint(RESET));
	else
		printf("  %s%d/%d functions passed%s\n", paint(RED), passed, ran,
			paint(RESET));
	print_names("failed", RED, READY, FAILED);
	print_names("didn't build", RED, BROKEN, NOT_RUN);
	print_names("not found", DIM, MISSING, NOT_RUN);
	if (passed < ran)
		printf("  %sRun one test on its own (e.g. libt ft_split) to see all "
			"its details.%s\n", paint(DIM), paint(RESET));
}

/* Runs the selected functions; returns 1 when they all passed */
static int	run_selected(void)
{
	int		i;
	int		count;
	int		passed;
	int		ran;
	t_group	group;

	count = 0;
	i = -1;
	while (++i < COUNT)
		count += g_funcs[i].selected;
	if (count == 1)
	{
		i = 0;
		while (!g_funcs[i].selected)
			i++;
		return (run_one(&g_funcs[i], 0) == PASSED);
	}
	passed = 0;
	ran = 0;
	group = GROUPS;
	i = -1;
	while (++i < COUNT)
	{
		if (!g_funcs[i].selected || g_funcs[i].state == MISSING)
			continue ;
		if (g_funcs[i].group != group)
			printf("\n %s%s%s\n", paint(BOLD),
				g_group_names[g_funcs[i].group], paint(RESET));
		group = g_funcs[i].group;
		passed += run_one(&g_funcs[i], !g_verbose) == PASSED;
		ran++;
	}
	print_summary(passed, ran);
	return (passed == ran);
}

/* ************************************************************************** */
/*  Selection                                                                 */
/* ************************************************************************** */

static int	select_group(int group)
{
	int	i;

	i = 0;
	while (i < COUNT)
	{
		if (group == GROUPS || (int)g_funcs[i].group == group)
			g_funcs[i].selected = 1;
		i++;
	}
	return (1);
}

/* Accepts all, part1/p1, part2/p2, part3/p3, a menu number or a function name,
** with or without its ft_ prefix and .c extension */
static int	select_word(const char *word)
{
	char	name[64];
	size_t	len;
	int		i;

	if (!strcmp(word, "all") || !strcmp(word, "-a"))
		return (select_group(GROUPS));
	if (!strcmp(word, "part1") || !strcmp(word, "p1"))
		return (select_group(PART1));
	if (!strcmp(word, "part2") || !strcmp(word, "p2"))
		return (select_group(PART2));
	if (!strcmp(word, "part3") || !strcmp(word, "p3"))
		return (select_group(PART3));
	i = atoi(word);
	if (i >= 1 && i <= COUNT && strspn(word, "0123456789") == strlen(word))
		return (g_funcs[i - 1].selected = 1);
	snprintf(name, sizeof(name), "%s%s", strncmp(word, "ft_", 3) ? "ft_" : "",
		word);
	len = strlen(name);
	if (len > 2 && !strcmp(name + len - 2, ".c"))
		name[len - 2] = '\0';
	i = -1;
	while (++i < COUNT)
		if (!strcmp(name, g_funcs[i].name))
			return (g_funcs[i].selected = 1);
	printf("  %sUnknown test: %s%s\n", paint(RED), word, paint(RESET));
	return (0);
}

static void	clear_selection(void)
{
	int	i;

	i = 0;
	while (i < COUNT)
		g_funcs[i++].selected = 0;
}

/* ************************************************************************** */
/*  Menu                                                                      */
/* ************************************************************************** */

/* One menu cell, padded to COLUMN visible characters unless it ends the row */
static void	print_cell(int index, int last)
{
	const t_func	*f;
	const char		*mark;
	const char		*color;

	f = &g_funcs[index];
	mark = " ";
	color = "";
	if (f->state == MISSING)
	{
		mark = "·";
		color = DIM;
	}
	else if (f->state == BROKEN)
	{
		mark = "!";
		color = YELLOW;
	}
	else if (f->result != NOT_RUN)
	{
		mark = f->result == PASSED ? "✓" : "✗";
		color = f->result == PASSED ? GREEN : RED;
	}
	printf("%s%3d %s%s %s%s%s", paint(f->state == MISSING ? DIM : ""),
		index + 1, f->name, paint(RESET), paint(color), mark, paint(RESET));
	if (!last)
		printf("%*s", COLUMN - 6 - (int)strlen(f->name), "");
}

/* Three columns, one per part; each part is shorter than the one before */
static void	print_menu(void)
{
	int	first[GROUPS + 1];
	int	row;
	int	g;

	g = 0;
	row = -1;
	while (++row < COUNT)
		if (row == 0 || g_funcs[row].group != g_funcs[row - 1].group)
			first[g++] = row;
	first[GROUPS] = COUNT;
	printf("\n %s🧪 Libft Tester%s  %s%s%s\n\n", paint(BOLD), paint(RESET),
		paint(DIM), g_src, paint(RESET));
	g = -1;
	while (++g < GROUPS)
		printf("    %s%-*s%s", paint(BOLD), g < GROUPS - 1 ? COLUMN - 4 : 0,
			g_group_names[g], paint(RESET));
	printf("\n");
	row = 0;
	while (first[0] + row < first[1])
	{
		g = -1;
		while (++g < GROUPS && first[g] + row < first[g + 1])
			print_cell(first[g] + row, g == GROUPS - 1
				|| first[g + 1] + row >= first[g + 2]);
		printf("\n");
		row++;
	}
	printf("\n  %snumber/name%s run one   %sp1 p2 p3 all%s run a group   "
		"%sq%s quit\n", paint(YELLOW), paint(RESET), paint(YELLOW),
		paint(RESET), paint(YELLOW), paint(RESET));
	printf("  %s·: file not found   !: doesn't build (select it to see "
		"why)%s\n\n> ", paint(DIM), paint(RESET));
	fflush(stdout);
}

static int	select_line(char *line)
{
	char	*word;
	int		ok;

	ok = 0;
	clear_selection();
	word = strtok(line, " \t\n");
	while (word)
	{
		ok |= select_word(word);
		word = strtok(NULL, " \t\n");
	}
	return (ok);
}

static void	interactive(void)
{
	char	line[256];

	clear_screen();
	print_menu();
	while (fgets(line, sizeof(line), stdin))
	{
		line[strcspn(line, "\n")] = '\0';
		if (!strcmp(line, "q") || !strcmp(line, "quit")
			|| !strcmp(line, "exit"))
			return ;
		if (line[0] && select_line(line))
		{
			clear_screen();
			run_selected();
			printf("\n%sPress Enter to go back to the menu...%s", paint(YELLOW),
				paint(RESET));
			fflush(stdout);
			if (!fgets(line, sizeof(line), stdin))
				return ;
			clear_screen();
			print_menu();
		}
		else
		{
			printf("> ");
			fflush(stdout);
		}
	}
}

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
		g_src = getenv("LIBT_SRC");
	if (getenv("LIBT_BUILD"))
		g_build = getenv("LIBT_BUILD");
	g_color = isatty(STDOUT_FILENO) && !getenv("NO_COLOR");
	load_states();
	i = 1;
	while (i < argc && argv[i][0] == '-' && strcmp(argv[i], "-a"))
	{
		if (strcmp(argv[i], "-v") && strcmp(argv[i], "--verbose"))
		{
			usage();
			return (strcmp(argv[i], "-h") && strcmp(argv[i], "--help"));
		}
		g_verbose = 1;
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
