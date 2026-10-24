/*
** Printable versions of values for messages: strings and buffers quoted
** with C escapes, characters as 'a', '\n', '\xff' or EOF.
*/

#include "internal.h"

#include <stdio.h>
#include <string.h>

/* One of 8 rotating buffers, so several values fit in one message */
char	*slot(void)
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
