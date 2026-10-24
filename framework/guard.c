/*
** Guarded buffers: data placed right against a page with no access rights
** (mprotect PROT_NONE), so touching one byte too far crashes the case.
*/

#include "internal.h"

#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

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
