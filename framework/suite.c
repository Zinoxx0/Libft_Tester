/*
** Suites and cases. test() forks: the child runs the case under alarm()
** and exits with EXIT_PASSED or EXIT_FAILED; a signal or another status
** means it crashed, timed out or called exit(). The parent reads the shared
** report and prints the case.
**
** LIBT_NO_FORK=1 runs the cases in this process instead, without a time
** limit, for debuggers: gdb then stops right where a case crashes.
*/

#include "internal.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

t_suite	g_suite;

void	suite(int argc, char **argv, const char *name)
{
	const char	*timeout;

	g_suite.name = name;
	g_suite.quiet = argc > 1 && strcmp(argv[1], "-q") == 0;
	g_suite.color = isatty(STDOUT_FILENO) && !getenv("NO_COLOR");
	timeout = getenv("LIBT_TIMEOUT");
	g_suite.timeout = TIMEOUT;
	if (timeout && atoi(timeout) > 0)
		g_suite.timeout = atoi(timeout);
	g_suite.no_fork = getenv("LIBT_NO_FORK") != NULL;
	g_suite.report = mmap(NULL, sizeof(t_report), PROT_READ | PROT_WRITE,
			MAP_SHARED | MAP_ANON, -1, 0);
	if (g_suite.report == MAP_FAILED)
	{
		perror("mmap");
		exit(2);
	}
	g_suite.report->len = 0;
	if (!g_suite.quiet)
		printf("\n %s🧪 %s%s\n\n", paint(BOLD), name, paint(RESET));
}

void	prototype(const char *subject, int matches)
{
	if (matches)
		return ;
	g_suite.warned++;
	emit("%*s%s⚠%s prototype differs from the subject: %s\n",
		g_suite.quiet ? 6 : 2, "", paint(YELLOW), paint(RESET), subject);
}

static void	run_case(void (*fn)(void))
{
	memory_reset();
	if (!g_suite.no_fork)
		alarm(g_suite.timeout);
	fn();
	alarm(0);
	if (!g_suite.report->failures)
		report_leaks();
}

static void	run_child(void (*fn)(void))
{
	run_case(fn);
	fflush(stdout);
	if (g_suite.report->failures)
		_exit(EXIT_FAILED);
	_exit(EXIT_PASSED);
}

/* Why a child that didn't finish normally stopped, or NULL if it did */
static const char	*abnormal_end(int status)
{
	static char	why[128];

	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGALRM)
		snprintf(why, sizeof(why), "timed out after %d s (infinite loop?)",
			g_suite.timeout);
	else if (WIFSIGNALED(status))
		snprintf(why, sizeof(why), "crashed: %s", strsignal(WTERMSIG(status)));
	else if (WEXITSTATUS(status) == EXIT_SANITIZER)
		snprintf(why, sizeof(why), "stopped by the sanitizer (report above)");
	else if (WEXITSTATUS(status) != EXIT_PASSED
		&& WEXITSTATUS(status) != EXIT_FAILED)
		snprintf(why, sizeof(why), "exited with status %d (calls exit()?)",
			WEXITSTATUS(status));
	else
		return (NULL);
	return (why);
}

static void	print_case(const char *title, int passed, const char *why)
{
	t_report	*r;
	int			indent;
	char		*line;
	char		*end;

	r = g_suite.report;
	indent = g_suite.quiet ? 6 : 2;
	if (passed)
		emit("%*s%s✓%s %s\n", indent, "", paint(GREEN), paint(RESET), title);
	else if (why)
		emit("%*s%s✗ %s — %s%s\n", indent, "", paint(RED), title, why,
			paint(RESET));
	else
		emit("%*s%s✗%s %s\n", indent, "", paint(RED), paint(RESET), title);
	if (why && r->doing[0])
		emit("%*s%swhile running%s %s\n", indent + 4, "", paint(DIM),
			paint(RESET), r->doing);
	line = r->text;
	while (*line)
	{
		end = strchr(line, '\n');
		emit("%*s%.*s\n", indent + 4, "", (int)(end - line), line);
		line = end + 1;
	}
	if (r->shown > max_shown())
		emit("%*s%s... and %d more%s\n", indent + 4, "", paint(DIM),
			r->shown - max_shown(), paint(RESET));
}

/* Runs the case in a child process: returns why it ended badly, or NULL */
static const char	*run_isolated(void (*fn)(void))
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == 0)
		run_child(fn);
	if (pid < 0 || waitpid(pid, &status, 0) < 0)
		return ("tester error: couldn't run the case");
	return (abnormal_end(status));
}

void	test(const char *title, void (*fn)(void))
{
	const char	*why;
	int			passed;

	memset(g_suite.report, 0, offsetof(t_report, text) + 1);
	fflush(stdout);
	fflush(stderr);
	why = NULL;
	if (g_suite.no_fork)
		run_case(fn);
	else
		why = run_isolated(fn);
	passed = !why && !g_suite.report->failures;
	g_suite.cases++;
	g_suite.passed += passed;
	g_suite.warned += g_suite.report->warnings;
	if (!g_suite.quiet)
		print_case(title, passed, why);
	else if ((!passed || g_suite.report->warnings)
		&& g_suite.printed++ < MAX_CASES_QUIET)
		print_case(title, passed, why);
	fflush(stdout);
}

int	done(void)
{
	int	failed;

	failed = g_suite.cases - g_suite.passed;
	if (!g_suite.quiet)
		printf("\n  %s%d/%d cases passed%s\n", paint(failed ? RED : GREEN),
			g_suite.passed, g_suite.cases, paint(RESET));
	else
	{
		printf("  %s%s%s %s", paint(failed ? RED : GREEN),
			failed ? "✗" : "✓", paint(RESET), g_suite.name);
		if (failed)
			printf("%*s %s%d/%d cases passed%s", 18 - (int)strlen(g_suite.name),
				"", paint(RED), g_suite.passed, g_suite.cases, paint(RESET));
		else if (g_suite.warned)
			printf("%*s %s⚠ %d warning%s%s", 18 - (int)strlen(g_suite.name), "",
				paint(YELLOW), g_suite.warned, g_suite.warned > 1 ? "s" : "",
				paint(RESET));
		printf("\n");
		if (g_suite.out)
			fputs(g_suite.out, stdout);
		if (g_suite.printed > MAX_CASES_QUIET)
			printf("      %s... and %d more failing cases%s\n", paint(DIM),
				g_suite.printed - MAX_CASES_QUIET, paint(RESET));
	}
	free(g_suite.out);
	return (failed != 0);
}
