/*
** Output capture for the ft_put*_fd tests: the function writes to a
** temporary file instead of the terminal, and stdout is redirected to a
** second one, which must stay empty.
*/

#include "internal.h"

#include <stdio.h>
#include <unistd.h>

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
