#include "tester.h"

#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#define TIMEOUT			5
#define MAX_SHOWN		6
#define MAX_SHOWN_QUIET	2
#define MAX_CASES_QUIET	3
#define REPORT_SIZE		16384
#define EXIT_FAILED		10
#define EXIT_SANITIZER	86

#define MAX_BLOCKS		16384
#define CANARY_SIZE		16
#define QUARANTINE_MAX	(64UL << 20)
#define GARBAGE			0xBE
#define CANARY			0xCA
#define FREED			0xDF

#define RED				"\033[31m"
#define GREEN			"\033[32m"
#define YELLOW			"\033[33m"
#define BOLD			"\033[1m"
#define DIM				"\033[2m"
#define RESET			"\033[0m"

/*
** What a case's child reports back. It lives in shared memory, so the parent
** can still read it when the child crashes.
*/
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
	int			cases;
	int			passed;
	int			warned;
	int			printed;
	t_report	*report;
	char		*out;
	size_t		out_len;
}	t_suite;

typedef struct s_block
{
	unsigned char	*ptr;
	size_t			size;
	size_t			serial;
	int				live;
}	t_block;

typedef struct s_memory
{
	t_block	blocks[MAX_BLOCKS];
	size_t	count;
	size_t	serial;
	size_t	calls;
	size_t	fail_at;
	size_t	quarantined;
}	t_memory;

static t_suite	g_suite;
static t_memory	g_mem;

/* ************************************************************************** */
/*  Output                                                                    */
/* ************************************************************************** */

static const char	*paint(const char *code)
{
	if (g_suite.color)
		return (code);
	return ("");
}

/* Parent side: straight to stdout, or kept for done() in quiet mode */
static void	emit(const char *fmt, ...)
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

static void	add(int indent, const char *fmt, ...)
{
	va_list	ap;

	va_start(ap, fmt);
	vadd(indent, fmt, ap);
	va_end(ap);
}

/* How many failures and warnings a case shows: fewer in a group run */
static int	max_shown(void)
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

static void	vfailure(const char *fmt, va_list ap)
{
	g_suite.report->failures++;
	note("", fmt, ap);
}

static void	failure(const char *fmt, ...)
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

/* ************************************************************************** */
/*  Printable values                                                          */
/* ************************************************************************** */

static char	*slot(void)
{
	static char	slots[8][320];
	static int	next;

	return (slots[next++ % 8]);
}

static int	escape(char *out, unsigned char c)
{
	const char	*from = "\n\t\r\v\f\"\\";
	const char	*to = "ntrvf\"\\";
	const char	*found;

	found = NULL;
	if (c)
		found = strchr(from, c);
	if (found)
		return (sprintf(out, "\\%c", to[found - from]));
	if (c == '\0')
		return (sprintf(out, "\\0"));
	if (c >= 32 && c < 127)
		return (sprintf(out, "%c", c));
	return (sprintf(out, "\\x%02x", c));
}

static const char	*quote(const unsigned char *s, size_t n, const char *unit)
{
	char	*out;
	size_t	o;
	size_t	i;

	out = slot();
	o = 0;
	i = 0;
	out[o++] = '"';
	while (i < n && o < 200)
		o += escape(out + o, s[i++]);
	out[o++] = '"';
	out[o] = '\0';
	if (i < n)
		sprintf(out + o, "... (%zu %s)", n, unit);
	return (out);
}

const char	*show(const char *s)
{
	if (!s)
		return ("NULL");
	return (quote((const unsigned char *)s, strlen(s), "chars"));
}

const char	*show_mem(const void *p, size_t n)
{
	if (!p)
		return ("NULL");
	return (quote(p, n, "bytes"));
}

const char	*show_char(int c)
{
	char	*out;

	out = slot();
	if (c == EOF)
		return ("EOF");
	if (c < 0 || c > 255)
		sprintf(out, "%d", c);
	else
	{
		out[0] = '\'';
		sprintf(out + 1 + escape(out + 1, c), "'");
	}
	return (out);
}

/* ************************************************************************** */
/*  Checks                                                                    */
/* ************************************************************************** */

int	check(int ok, const char *fmt, ...)
{
	va_list	ap;

	if (ok)
		return (1);
	va_start(ap, fmt);
	vfailure(fmt, ap);
	va_end(ap);
	return (0);
}

int	check_int(long got, long want, const char *what)
{
	return (check(got == want, "%s: expected %ld, got %ld", what, want, got));
}

int	check_size(size_t got, size_t want, const char *what)
{
	return (check(got == want, "%s: expected %zu, got %zu", what, want, got));
}

int	check_bool(int got, int want, const char *what)
{
	if (!got == !want)
		return (1);
	if (want)
		return (check(0, "%s: expected true (non-zero), got 0", what));
	return (check(0, "%s: expected false (0), got %d", what, got));
}

static const char	*sign_name(long n)
{
	if (n < 0)
		return ("a negative value");
	if (n > 0)
		return ("a positive value");
	return ("0");
}

int	check_sign(long got, long want, const char *what)
{
	if ((got < 0) == (want < 0) && (got > 0) == (want > 0))
		return (1);
	return (check(0, "%s: expected %s, got %ld", what, sign_name(want), got));
}

int	check_str(const char *got, const char *want, const char *what)
{
	if (got == want || (got && want && strcmp(got, want) == 0))
		return (1);
	return (check(0, "%s: expected %s, got %s", what, show(want), show(got)));
}

int	check_mem(const void *got, const void *want, size_t n, const char *what)
{
	const unsigned char	*g;
	const unsigned char	*w;
	size_t				i;

	g = got;
	w = want;
	i = 0;
	while (i < n && g[i] == w[i])
		i++;
	if (i == n)
		return (1);
	if (n <= 40)
		return (check(0, "%s: expected %s, got %s", what, show_mem(w, n),
				show_mem(g, n)));
	return (check(0, "%s: byte %zu is 0x%02x, expected 0x%02x", what, i,
			g[i], w[i]));
}

static const char	*where(const void *p, const void *base)
{
	char	*out;

	if (!p)
		return ("NULL");
	out = slot();
	sprintf(out, "s + %td", (const char *)p - (const char *)base);
	return (out);
}

int	check_ptr(const void *got, const void *want, const void *base,
		const char *what)
{
	if (got == want)
		return (1);
	return (check(0, "%s: expected %s, got %s", what, where(want, base),
			where(got, base)));
}

/* ************************************************************************** */
/*  Memory tracking                                                           */
/* ************************************************************************** */

static t_block	*find_block(const void *p)
{
	size_t	i;

	i = g_mem.count;
	while (i-- > 0)
		if (g_mem.blocks[i].ptr == p)
			return (&g_mem.blocks[i]);
	return (NULL);
}

static t_block	*find_containing(const void *p)
{
	const unsigned char	*c;
	size_t				i;
	t_block				*b;

	c = p;
	i = g_mem.count;
	while (i-- > 0)
	{
		b = &g_mem.blocks[i];
		if (b->live && c > b->ptr && c < b->ptr + b->size + CANARY_SIZE)
			return (b);
	}
	return (NULL);
}

/* Drops the freed blocks kept to catch double frees, to make room */
static void	release_quarantine(void)
{
	size_t	i;
	size_t	kept;

	i = 0;
	kept = 0;
	while (i < g_mem.count)
	{
		if (g_mem.blocks[i].live)
			g_mem.blocks[kept++] = g_mem.blocks[i];
		else
			free(g_mem.blocks[i].ptr);
		i++;
	}
	g_mem.count = kept;
	g_mem.quarantined = 0;
}

/* Reports a damaged canary once, then repairs it */
static int	canary_ok(const t_block *b)
{
	size_t	i;

	i = 0;
	while (i < CANARY_SIZE && b->ptr[b->size + i] == CANARY)
		i++;
	if (i == CANARY_SIZE)
		return (1);
	if (i == 0 && b->ptr[b->size] == '\0')
		failure("wrote a '\\0' right after the end of a %zu-byte malloc "
			"block: forgot the +1?", b->size);
	else
		failure("wrote past the end of a %zu-byte malloc block", b->size);
	memset(b->ptr + b->size, CANARY, CANARY_SIZE);
	return (0);
}

void	*tester_malloc(size_t size)
{
	unsigned char	*p;
	t_block			*b;

	g_mem.calls++;
	if (g_mem.fail_at && g_mem.calls == g_mem.fail_at)
		return (NULL);
	if (g_mem.count == MAX_BLOCKS)
		release_quarantine();
	if (g_mem.count == MAX_BLOCKS || size > (size_t)-1 - CANARY_SIZE)
	{
		if (g_mem.count == MAX_BLOCKS)
			failure("more than %d live blocks: the tester gives up", MAX_BLOCKS);
		return (NULL);
	}
	p = malloc(size + CANARY_SIZE);
	if (!p)
		return (NULL);
	memset(p, GARBAGE, size);
	memset(p + size, CANARY, CANARY_SIZE);
	b = &g_mem.blocks[g_mem.count++];
	b->ptr = p;
	b->size = size;
	b->serial = ++g_mem.serial;
	b->live = 1;
	return (p);
}

void	tester_free(void *ptr)
{
	t_block	*b;

	if (!ptr)
		return ;
	b = find_block(ptr);
	if (!b && find_containing(ptr))
		failure("free() of a pointer into the middle of a %zu-byte block",
			find_containing(ptr)->size);
	else if (!b)
		failure("free() of a pointer malloc never returned");
	else if (!b->live)
		failure("double free of a %zu-byte block", b->size);
	if (!b || !b->live)
		return ;
	canary_ok(b);
	b->live = 0;
	memset(b->ptr, FREED, b->size);
	g_mem.quarantined += b->size;
	if (g_mem.quarantined > QUARANTINE_MAX)
		release_quarantine();
}

size_t	live_blocks(void)
{
	size_t	i;
	size_t	n;

	i = 0;
	n = 0;
	while (i < g_mem.count)
		n += g_mem.blocks[i++].live;
	return (n);
}

int	check_alloc(const void *p, size_t need, const char *what)
{
	t_block	*b;

	b = find_block(p);
	if (!b || !b->live)
	{
		if (find_containing(p))
			return (check(0, "%s points into the middle of a malloc block, "
					"it can't be freed on its own", what));
		return (check(0, "%s wasn't allocated with malloc", what));
	}
	if (!canary_ok(b))
		return (0);
	if (b->size < need)
		return (check(0, "%s: malloc'd %zu bytes, needs %zu", what,
				b->size, need));
	if (b->size > need)
		warn("%s: malloc'd %zu bytes, %zu would be enough", what, b->size,
			need);
	return (1);
}

/* Blocks allocated after `serial` that are still live: forgotten, so they're
** only reported once */
static size_t	take_leaks(size_t serial, size_t *bytes)
{
	size_t	i;
	size_t	n;

	i = 0;
	n = 0;
	*bytes = 0;
	while (i < g_mem.count)
	{
		if (g_mem.blocks[i].live && g_mem.blocks[i].serial > serial)
		{
			g_mem.blocks[i].live = 0;
			*bytes += g_mem.blocks[i].size;
			n++;
		}
		i++;
	}
	return (n);
}

static void	report_leaks(void)
{
	size_t	i;
	size_t	n;
	size_t	bytes;
	t_block	*b;

	n = 0;
	bytes = 0;
	i = 0;
	while (i < g_mem.count)
	{
		if (g_mem.blocks[i].live)
		{
			n++;
			bytes += g_mem.blocks[i].size;
		}
		i++;
	}
	if (!n)
		return ;
	g_suite.report->doing[0] = '\0';
	failure("leaked %zu bytes in %zu block%s:", bytes, n, n > 1 ? "s" : "");
	i = 0;
	n = 0;
	while (i < g_mem.count && n < 3)
	{
		b = &g_mem.blocks[i++];
		if (b->live && ++n)
			add(2, "%zu bytes: %s", b->size, show_mem(b->ptr,
					b->size < 24 ? b->size : 24));
	}
}

void	check_malloc_failures(void *(*fn)(void *), void *arg,
		void (*release)(void *), int null_required)
{
	char	call[sizeof(g_suite.report->doing)];
	size_t	calls;
	size_t	k;
	size_t	mark;
	size_t	bytes;
	size_t	owned;
	void	*result;

	memcpy(call, g_suite.report->doing, sizeof(call));
	calls = g_mem.calls;
	result = fn(arg);
	calls = g_mem.calls - calls;
	if (result)
		release(result);
	k = 0;
	while (++k <= calls)
	{
		doing("%s with malloc #%zu of %zu failing", call, k, calls);
		mark = g_mem.serial;
		owned = live_blocks();
		g_mem.fail_at = g_mem.calls + k;
		result = fn(arg);
		g_mem.fail_at = 0;
		if (result && null_required)
			failure("returned a pointer instead of NULL");
		else if (result)
			release(result);
		if (take_leaks(mark, &bytes))
			failure("leaked %zu bytes after the failure", bytes);
		if (live_blocks() < owned)
			failure("freed memory it didn't allocate");
	}
	doing("%s", call);
}

/* ************************************************************************** */
/*  Guarded memory                                                            */
/* ************************************************************************** */

static unsigned char	*map_pages(size_t len)
{
	void	*p;

	p = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	if (p == MAP_FAILED)
	{
		failure("tester error: mmap failed");
		_exit(EXIT_FAILED);
	}
	return (p);
}

static void	fill(unsigned char *p, const void *src, size_t n)
{
	if (src)
		memcpy(p, src, n);
	else
		memset(p, GARBAGE, n);
}

void	*guard_after(const void *src, size_t n)
{
	size_t			page;
	size_t			data;
	unsigned char	*base;

	page = sysconf(_SC_PAGESIZE);
	data = (n + page - 1) / page * page;
	base = map_pages(data + page);
	mprotect(base + data, page, PROT_NONE);
	fill(base + data - n, src, n);
	return (base + data - n);
}

void	*guard_before(const void *src, size_t n)
{
	size_t			page;
	size_t			data;
	unsigned char	*base;

	page = sysconf(_SC_PAGESIZE);
	data = (n + page - 1) / page * page;
	base = map_pages(page + data + page);
	mprotect(base, page, PROT_NONE);
	fill(base + page, src, n);
	return (base + page);
}

char	*guard_str(const char *s)
{
	return (guard_after(s, strlen(s) + 1));
}

/* ************************************************************************** */
/*  Output capture                                                            */
/* ************************************************************************** */

static FILE	*g_capture_out;
static FILE	*g_capture_stdout;
static int	g_saved_stdout = -1;

static size_t	slurp(FILE *f, char *buf, size_t size)
{
	size_t	len;
	ssize_t	n;

	len = 0;
	lseek(fileno(f), 0, SEEK_SET);
	n = 1;
	while (len < size - 1 && n > 0)
	{
		n = read(fileno(f), buf + len, size - 1 - len);
		if (n > 0)
			len += n;
	}
	buf[len] = '\0';
	fclose(f);
	return (len);
}

int	capture(void)
{
	g_capture_out = tmpfile();
	g_capture_stdout = tmpfile();
	if (!g_capture_out || !g_capture_stdout)
	{
		failure("tester error: can't create a temporary file");
		_exit(EXIT_FAILED);
	}
	fflush(stdout);
	g_saved_stdout = dup(STDOUT_FILENO);
	dup2(fileno(g_capture_stdout), STDOUT_FILENO);
	return (fileno(g_capture_out));
}

const char	*captured(size_t *len)
{
	static char	buf[1 << 17];
	size_t		n;

	fflush(stdout);
	dup2(g_saved_stdout, STDOUT_FILENO);
	close(g_saved_stdout);
	n = slurp(g_capture_stdout, buf, sizeof(buf));
	if (n)
		failure("wrote %s to stdout instead of the fd it was given",
			show_mem(buf, n));
	*len = slurp(g_capture_out, buf, sizeof(buf));
	return (buf);
}

/* ************************************************************************** */
/*  Suite and cases                                                           */
/* ************************************************************************** */

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

static void	run_child(void (*fn)(void))
{
	memset(&g_mem, 0, sizeof(g_mem));
	alarm(g_suite.timeout);
	fn();
	alarm(0);
	if (!g_suite.report->failures)
		report_leaks();
	fflush(stdout);
	if (g_suite.report->failures)
		_exit(EXIT_FAILED);
	_exit(0);
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
	else if (WEXITSTATUS(status) != 0 && WEXITSTATUS(status) != EXIT_FAILED)
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

void	test(const char *title, void (*fn)(void))
{
	pid_t		pid;
	int			status;
	const char	*why;
	int			passed;

	memset(g_suite.report, 0, offsetof(t_report, text) + 1);
	fflush(stdout);
	fflush(stderr);
	pid = fork();
	if (pid == 0)
		run_child(fn);
	status = 0;
	if (pid < 0 || waitpid(pid, &status, 0) < 0)
		why = "tester error: couldn't run the case";
	else
		why = abnormal_end(status);
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
