#ifndef RUNNER_H
# define RUNNER_H

/*
** The runner: lists the functions of the subject, runs their test binaries
** one by one, by part or all of them, from the command line or a menu.
** A test binary exits with 0 when every case passed, 1 otherwise.
*/

# define RED	"\033[31m"
# define GREEN	"\033[32m"
# define YELLOW	"\033[33m"
# define BOLD	"\033[1m"
# define DIM	"\033[2m"
# define RESET	"\033[0m"

typedef enum e_group
{
	PART1,
	PART2,
	PART3,
	GROUPS
}	t_group;

/* MISSING: no ft_name.c; BROKEN: the file is there but its test didn't build */
typedef enum e_state
{
	MISSING,
	BROKEN,
	READY
}	t_state;

typedef enum e_result
{
	NOT_RUN,
	PASSED,
	FAILED
}	t_result;

typedef struct s_func
{
	const char	*name;
	t_group		group;
	t_state		state;
	t_result	result;
	int			selected;
}	t_func;

/* The libft directory (LIBT_SRC) and the build directory (LIBT_BUILD) */
typedef struct s_runner
{
	const char	*src;
	const char	*build;
	int			color;
	int			verbose;
}	t_runner;

extern t_func		g_funcs[];
extern const int	g_count;
extern const char	*g_group_names[GROUPS];
extern t_runner		g_runner;

/* functions.c */
const char	*paint(const char *code);
void		load_states(void);

/* select.c */
int			select_word(const char *word);
int			select_line(char *line);

/* run.c */
int			run_selected(void);

/* menu.c */
void		interactive(void);

#endif
