/*
** The functions of the subject, in its order, and the runner's settings.
** Adding a test means adding its function to this table.
*/

#include "runner.h"

#include <stdio.h>
#include <unistd.h>

t_func	g_funcs[] = {
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

const int	g_count = sizeof(g_funcs) / sizeof(*g_funcs);
const char	*g_group_names[GROUPS] = {"Part 1", "Part 2", "Part 3"};
t_runner	g_runner = {".", "build", 0, 0};

const char	*paint(const char *code)
{
	if (g_runner.color)
		return (code);
	return ("");
}

static int	file_exists(const char *dir, const char *name, const char *ext)
{
	char	path[4096];

	snprintf(path, sizeof(path), "%s/%s%s", dir, name, ext);
	return (access(path, F_OK) == 0);
}

void	load_states(void)
{
	int	i;

	i = 0;
	while (i < g_count)
	{
		g_funcs[i].state = MISSING;
		if (file_exists(g_runner.src, g_funcs[i].name, ".c"))
		{
			g_funcs[i].state = BROKEN;
			if (file_exists(g_runner.build, "bin/", g_funcs[i].name))
				g_funcs[i].state = READY;
		}
		i++;
	}
}
