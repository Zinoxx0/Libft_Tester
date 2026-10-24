/*
** The case report. In the child, failures, warnings and the doing() line
** are appended to the shared t_report; in the parent, emit() prints, or
** keeps the text for done() in quiet mode.
*/

#include "internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char	*paint(const char *code)
{
	if (g_suite.color)
		return (code);
	return ("");
}

/* Parent side: straight to stdout, or kept for done() in quiet mode */
void	emit(const char *fmt, ...)
{
	va_list	ap;
	char	line[1024];
	int		len;

	va_start(ap, fmt);
	len = vsnprintf(line, sizeof(line), fmt, ap);
	va_end(ap);
	if (len < 0)
		return ;
	if ((size_t)len >= sizeof(line))
		len = sizeof(line) - 1;
	if (!g_suite.quiet)
	{
		fputs(line, stdout);
		return ;
	}
	g_suite.out = realloc(g_suite.out, g_suite.out_len + len + 1);
	if (!g_suite.out)
		exit(2);
	memcpy(g_suite.out + g_suite.out_len, line, len + 1);
	g_suite.out_len += len;
}

/* Child side: appends one line to the shared report */
static void	vadd(int indent, const char *fmt, va_list ap)
{
	t_report	*r;
	size_t		room;
	int			len;

	r = g_suite.report;
	room = REPORT_SIZE - r->len;
	if (room < (size_t)indent + 2)
		return ;
	memset(r->text + r->len, ' ', indent);
	r->len += indent;
	room -= indent;
	len = vsnprintf(r->text + r->len, room - 1, fmt, ap);
	if (len < 0)
		len = 0;
	if ((size_t)len > room - 2)
		len = room - 2;
	r->len += len;
	r->text[r->len++] = '\n';
	r->text[r->len] = '\0';
}

void	add(int indent, const char *fmt, ...)
{
	va_list	ap;

	va_start(ap, fmt);
	vadd(indent, fmt, ap);
	va_end(ap);
}

/* How many failures and warnings a case shows: fewer in a group run */
int	max_shown(void)
{
	if (g_suite.quiet)
		return (MAX_SHOWN_QUIET);
	return (MAX_SHOWN);
}

static void	note(const char *mark, const char *fmt, va_list ap)
{
	t_report	*r;
	char		msg[768];

	r = g_suite.report;
	if (r->shown++ >= max_shown())
		return ;
	if (r->doing[0] && !r->doing_shown)
	{
		add(0, "%s", r->doing);
		r->doing_shown = 1;
	}
	vsnprintf(msg, sizeof(msg), fmt, ap);
	add(r->doing[0] ? 2 : 0, "%s%s", mark, msg);
}

void	vfailure(const char *fmt, va_list ap)
{
	g_suite.report->failures++;
	note("", fmt, ap);
}

void	failure(const char *fmt, ...)
{
	va_list	ap;

	va_start(ap, fmt);
	vfailure(fmt, ap);
	va_end(ap);
}

void	warn(const char *fmt, ...)
{
	va_list	ap;
	char	mark[32];

	snprintf(mark, sizeof(mark), "%s⚠%s ", paint(YELLOW), paint(RESET));
	g_suite.report->warnings++;
	va_start(ap, fmt);
	note(mark, fmt, ap);
	va_end(ap);
}

void	doing(const char *fmt, ...)
{
	va_list	ap;

	va_start(ap, fmt);
	vsnprintf(g_suite.report->doing, sizeof(g_suite.report->doing), fmt, ap);
	va_end(ap);
	g_suite.report->doing_shown = 0;
}
