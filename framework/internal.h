#ifndef INTERNAL_H
# define INTERNAL_H

/*
** Shared by the framework's own files only; tests include tester.h.
**
** Every case runs in a child process (suite.c). The child writes what went
** wrong into a t_report that lives in shared memory, so the parent can still
** read it after the child crashed or was killed, and prints it.
*/

# include "tester.h"
# include <stdarg.h>

# define TIMEOUT			5
# define MAX_SHOWN			6
# define MAX_SHOWN_QUIET	2
# define MAX_CASES_QUIET	3
# define REPORT_SIZE		16384

/* Fill for fresh malloc blocks and guarded buffers: never a valid string */
# define GARBAGE			0xBE

/* How a case's child exits; 86 is set in ASAN_OPTIONS by libt.sh */
# define EXIT_PASSED		0
# define EXIT_FAILED		10
# define EXIT_SANITIZER		86

# define RED				"\033[31m"
# define GREEN				"\033[32m"
# define YELLOW				"\033[33m"
# define BOLD				"\033[1m"
# define DIM				"\033[2m"
# define RESET				"\033[0m"

typedef struct s_report
{
	int		failures;
	int		warnings;
	int		shown;
	int		doing_shown;
	char	doing[512];
	size_t	len;
	char	text[REPORT_SIZE];
}	t_report;

typedef struct s_suite
{
	const char	*name;
	int			quiet;
	int			color;
	int			timeout;
	int			no_fork;
	int			cases;
	int			passed;
	int			warned;
	int			printed;
	t_report	*report;
	char		*out;
	size_t		out_len;
}	t_suite;

extern t_suite	g_suite;

/* report.c: the child fills the report, the parent prints with emit() */
const char	*paint(const char *code);
void		emit(const char *fmt, ...)
			__attribute__((format(printf, 1, 2)));
void		add(int indent, const char *fmt, ...)
			__attribute__((format(printf, 2, 3)));
void		failure(const char *fmt, ...)
			__attribute__((format(printf, 1, 2)));
void		vfailure(const char *fmt, va_list ap);
int			max_shown(void);

/* show.c */
char		*slot(void);

/* memory.c: called by the child around each case */
void		memory_reset(void);
void		report_leaks(void);

#endif
