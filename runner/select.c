/*
** Turns what the user typed (all, p1, 12, strlen, ft_split.c...) into a
** selection in the function table.
*/

#include "runner.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int	select_group(int group)
{
	int	i;

	i = 0;
	while (i < g_count)
	{
		if (group == GROUPS || (int)g_funcs[i].group == group)
			g_funcs[i].selected = 1;
		i++;
	}
	return (1);
}

/* Accepts all, part1/p1, part2/p2, part3/p3, a menu number or a function name,
** with or without its ft_ prefix and .c extension */
int	select_word(const char *word)
{
	char	name[64];
	size_t	len;
	int		i;

	if (!strcmp(word, "all") || !strcmp(word, "-a"))
		return (select_group(GROUPS));
	if (!strcmp(word, "part1") || !strcmp(word, "p1"))
		return (select_group(PART1));
	if (!strcmp(word, "part2") || !strcmp(word, "p2"))
		return (select_group(PART2));
	if (!strcmp(word, "part3") || !strcmp(word, "p3"))
		return (select_group(PART3));
	i = atoi(word);
	if (i >= 1 && i <= g_count && strspn(word, "0123456789") == strlen(word))
		return (g_funcs[i - 1].selected = 1);
	snprintf(name, sizeof(name), "%s%s", strncmp(word, "ft_", 3) ? "ft_" : "",
		word);
	len = strlen(name);
	if (len > 2 && !strcmp(name + len - 2, ".c"))
		name[len - 2] = '\0';
	i = -1;
	while (++i < g_count)
		if (!strcmp(name, g_funcs[i].name))
			return (g_funcs[i].selected = 1);
	printf("  %sUnknown test: %s%s\n", paint(RED), word, paint(RESET));
	return (0);
}

static void	clear_selection(void)
{
	int	i;

	i = 0;
	while (i < g_count)
		g_funcs[i++].selected = 0;
}

int	select_line(char *line)
{
	char	*word;
	int		ok;

	ok = 0;
	clear_selection();
	word = strtok(line, " \t\n");
	while (word)
	{
		ok |= select_word(word);
		word = strtok(NULL, " \t\n");
	}
	return (ok);
}
