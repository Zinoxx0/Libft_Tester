/*
** The interactive menu: three columns, one per part of the subject, with
** each function's state and its last result.
*/

#include "runner.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Width of a menu column, in visible characters */
#define COLUMN	25

static void	clear_screen(void)
{
	if (isatty(STDOUT_FILENO))
		printf("\033[H\033[2J");
}

/* One menu cell, padded to COLUMN visible characters unless it ends the row */
static void	print_cell(int index, int last)
{
	const t_func	*f;
	const char		*mark;
	const char		*color;

	f = &g_funcs[index];
	mark = " ";
	color = "";
	if (f->state == MISSING)
	{
		mark = "·";
		color = DIM;
	}
	else if (f->state == BROKEN)
	{
		mark = "!";
		color = YELLOW;
	}
	else if (f->result != NOT_RUN)
	{
		mark = f->result == PASSED ? "✓" : "✗";
		color = f->result == PASSED ? GREEN : RED;
	}
	printf("%s%3d %s%s %s%s%s", paint(f->state == MISSING ? DIM : ""),
		index + 1, f->name, paint(RESET), paint(color), mark, paint(RESET));
	if (!last)
		printf("%*s", COLUMN - 6 - (int)strlen(f->name), "");
}

/* Three columns, one per part; each part is shorter than the one before */
static void	print_menu(void)
{
	int	first[GROUPS + 1];
	int	row;
	int	g;

	g = 0;
	row = -1;
	while (++row < g_count)
		if (row == 0 || g_funcs[row].group != g_funcs[row - 1].group)
			first[g++] = row;
	first[GROUPS] = g_count;
	printf("\n %s🧪 Libft Tester%s  %s%s%s\n\n", paint(BOLD), paint(RESET),
		paint(DIM), g_runner.src, paint(RESET));
	g = -1;
	while (++g < GROUPS)
		printf("    %s%-*s%s", paint(BOLD), g < GROUPS - 1 ? COLUMN - 4 : 0,
			g_group_names[g], paint(RESET));
	printf("\n");
	row = 0;
	while (first[0] + row < first[1])
	{
		g = -1;
		while (++g < GROUPS && first[g] + row < first[g + 1])
			print_cell(first[g] + row, g == GROUPS - 1
				|| first[g + 1] + row >= first[g + 2]);
		printf("\n");
		row++;
	}
	printf("\n  %snumber/name%s run one   %sp1 p2 p3 all%s run a group   "
		"%sq%s quit\n", paint(YELLOW), paint(RESET), paint(YELLOW),
		paint(RESET), paint(YELLOW), paint(RESET));
	printf("  %s·: file not found   !: doesn't build (select it to see "
		"why)%s\n\n> ", paint(DIM), paint(RESET));
	fflush(stdout);
}

void	interactive(void)
{
	char	line[256];

	clear_screen();
	print_menu();
	while (fgets(line, sizeof(line), stdin))
	{
		line[strcspn(line, "\n")] = '\0';
		if (!strcmp(line, "q") || !strcmp(line, "quit")
			|| !strcmp(line, "exit"))
			return ;
		if (line[0] && select_line(line))
		{
			clear_screen();
			run_selected();
			printf("\n%sPress Enter to go back to the menu...%s", paint(YELLOW),
				paint(RESET));
			fflush(stdout);
			if (!fgets(line, sizeof(line), stdin))
				return ;
			clear_screen();
			print_menu();
		}
		else
		{
			printf("> ");
			fflush(stdout);
		}
	}
}
