/*
** Runs test binaries: fork + exec of build/bin/ft_name, with -q in group
** runs so each one prints a single line unless it fails.
*/

#include "runner.h"

#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

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
		snprintf(path, sizeof(path), "%s/log/%s%s", g_runner.build, f->name,
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
		printf("  %s· %-18s not found in %s%s\n", paint(DIM), f->name,
			g_runner.src, paint(RESET));
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
	snprintf(path, sizeof(path), "%s/bin/%s", g_runner.build, f->name);
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
	while (i < g_count)
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
int	run_selected(void)
{
	int		i;
	int		count;
	int		passed;
	int		ran;
	t_group	group;

	count = 0;
	i = -1;
	while (++i < g_count)
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
	while (++i < g_count)
	{
		if (!g_funcs[i].selected || g_funcs[i].state == MISSING)
			continue ;
		if (g_funcs[i].group != group)
			printf("\n %s%s%s\n", paint(BOLD),
				g_group_names[g_funcs[i].group], paint(RESET));
		group = g_funcs[i].group;
		passed += run_one(&g_funcs[i], !g_runner.verbose) == PASSED;
		ran++;
	}
	print_summary(passed, ran);
	return (passed == ran);
}
