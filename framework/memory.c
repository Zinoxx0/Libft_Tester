/*
** The malloc tracker. Tests and libft sources are compiled with
** -Dmalloc=tester_malloc -Dfree=tester_free, so every allocation lands here.
**
** Each block is filled with garbage (0xBE) and followed by a 16-byte canary
** (0xCA): a missing '\0' shows as garbage, a write past the end damages the
** canary. Freed blocks are kept (filled with 0xDF) to recognise double frees,
** until they take more than 64 MB. Blocks still live at the end of a case are
** leaks. fail_at makes one chosen malloc call return NULL.
*/

#include "internal.h"

#include <stdlib.h>
#include <string.h>

#define MAX_BLOCKS		16384
#define CANARY_SIZE		16
#define QUARANTINE_MAX	(64UL << 20)
#define CANARY			0xCA
#define FREED			0xDF

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

static t_memory	g_mem;

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

void	memory_reset(void)
{
	memset(&g_mem, 0, sizeof(g_mem));
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
			failure("more than %d live blocks: the tester gives up",
				MAX_BLOCKS);
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

static size_t	live_blocks(void)
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

void	report_leaks(void)
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
