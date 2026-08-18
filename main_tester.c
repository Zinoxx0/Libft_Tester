#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define TEST_DIR "./tests/bin"
#define MAX_TESTS 128
#define PAGE_SIZE 15

static void	clear_screen(void)
{
	system("clear");
}

static int	is_bonus(const char *name)
{
	return (strncmp(name, "ft_lst", 6) == 0);
}

static int	load_tests(char **tests)
{
	DIR				*dir;
	struct dirent	*entry;
	int				count;

	dir = opendir(TEST_DIR);
	if (!dir)
		return (0);
	count = 0;
	while ((entry = readdir(dir)) != NULL && count < MAX_TESTS)
	{
		if (strncmp(entry->d_name, "ft_", 3) == 0)
		{
			tests[count] = strdup(entry->d_name);
			if (!tests[count])
				break ;
			count++;
		}
	}
	closedir(dir);
	return (count);
}

static void	free_tests(char **tests, int count)
{
	int	i;

	i = 0;
	while (i < count)
		free(tests[i++]);
}

static int	cmp_tests(const void *a, const void *b)
{
	const char	*const *left = a;
	const char	*const *right = b;

	return (strcmp(*left, *right));
}

static void	print_menu(char **tests, int count, int page_start)
{
	int	i;
	int	page_end;

	printf("\n");
	printf("\033[1;32m========== Libft Tester by Samercad! ==========\033[0m\n");
	printf("  \033[1;33mall\033[0m        - run all available mandatory tests\n");
	printf("  \033[1;33mbonus\033[0m      - run all available bonus tests\n");
	printf("  \033[1;33mlist\033[0m       - list available tests\n");
	printf("  \033[1;33mexit\033[0m       - quit\n");
	printf("\n\033[1;34mAvailable tests:\033[0m\n");
	page_end = page_start + PAGE_SIZE;
	if (page_end > count)
		page_end = count;
	i = page_start;
	while (i < page_end)
	{
		printf("  [\033[1;33m%2d\033[0m] %s%s\n", i + 1, tests[i],
			is_bonus(tests[i]) ? "  [bonus]" : "");
		i++;
	}
	printf("\n  \033[1;33mn\033[0m - next page  |  \033[1;33mb\033[0m - previous page\n");
	printf("\nPage %d/%d\n", page_start / PAGE_SIZE + 1,
		(count + PAGE_SIZE - 1) / PAGE_SIZE);
	printf("\n> ");
}

static int	run_test(const char *name)
{
	char	path[512];
	int		status;
	pid_t	pid;

	snprintf(path, sizeof(path), "%s/%s", TEST_DIR, name);
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return (0);
	}
	if (pid == 0)
	{
		execl(path, name, (char *)NULL);
		perror(path);
		_exit(127);
	}
	if (waitpid(pid, &status, 0) < 0)
	{
		perror("waitpid");
		return (0);
	}
	return (WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

static int	run_group(char **tests, int count, int bonus)
{
	int	i;
	int	run;
	int	passed;

	passed = 0;
	i = 0;
	while (i < count)
	{
		if (is_bonus(tests[i]) == bonus)
		{
			run = run_test(tests[i]);
			if (run)
				passed++;
		}
		i++;
	}
	return (passed);
}

static void	print_list(char **tests, int count)
{
	int	i;

	printf("\n");
	i = 0;
	while (i < count)
	{
		printf("  %-24s%s\n", tests[i], is_bonus(tests[i]) ? "[bonus]" : "");
		i++;
	}
}

static void	wait_for_continue(void)
{
	char	buffer[128];

	printf("\n\033[1;33mPress anything to continue...\033[0m");
	fflush(stdout);
	fgets(buffer, sizeof(buffer), stdin);
	clear_screen();
}

int main(int argc, char **argv)
{
    char    *tests[MAX_TESTS];
    char    input[128];
    int     count;
    int     i;
    int     found;
    int     wrong_attempts;
    int     page_start;

    count = load_tests(tests);
    qsort(tests, count, sizeof(*tests), cmp_tests);
    if (count == 0)
    {
        printf("No testers were built.\n");
        printf("Run: make tests\n");
        return (1);
    }

    /* ========================================= */
    /* CLI MODE (Non-interactive)                */
    /* ========================================= */
    if (argc > 1)
    {
        if (strcmp(argv[1], "-a") == 0 || strcmp(argv[1], "all") == 0)
        {
            printf("\n\033[1;34m--- TESTING ALL ---\033[0m\n");
            run_group(tests, count, 0);
        }
        else if (strcmp(argv[1], "-b") == 0 || strcmp(argv[1], "bonus") == 0)
        {
            printf("\n\033[1;34m--- bonus ---\033[0m\n");
            run_group(tests, count, 1);
        }
        else
        {
            found = 0;
            i = 0;
            while (i < count)
            {
                if (strcmp(argv[1], tests[i]) == 0
                    || (strncmp(argv[1], "ft_", 3) == 0 && strcmp(argv[1], tests[i]) == 0))
                {
                    run_test(tests[i]);
                    found = 1;
                    break;
                }
                i++;
            }
            if (!found)
                printf("\033[1;31mUnknown test or flag: %s\033[0m\n", argv[1]);
        }
        free_tests(tests, count);
        return (0); /* Exit immediately, don't show the menu */
    }

    /* ========================================= */
    /* INTERACTIVE MODE                          */
    /* ========================================= */
    wrong_attempts = 0;
    page_start = 0;
    clear_screen();
    while (1)
    {
        if (wrong_attempts == 0 || wrong_attempts >= 10)
        {
            print_menu(tests, count, page_start);
            wrong_attempts = 0;
        }
        else
        {
            printf("> ");
        }
        if (!fgets(input, sizeof(input), stdin))
            break ;
        input[strcspn(input, "\n")] = '\0';
        if (strcmp(input, "exit") == 0 || strcmp(input, "q") == 0)
            break ;
        if (strcmp(input, "n") == 0)
        {
            if (page_start + PAGE_SIZE < count)
                page_start += PAGE_SIZE;
            else
            {
                clear_screen();
                printf("\033[1;31mThis is the last page already!!!\033[0m\n");
                continue ;
            }
            clear_screen();
            continue ;
        }
        if (strcmp(input, "b") == 0)
        {
            clear_screen();
            if (page_start == 0)
                printf("\033[1;31mThis is the first page!!!\033[0m\n");
            else
                page_start -= PAGE_SIZE;
            continue ;
        }
        if (strcmp(input, "list") == 0)
        {
            clear_screen();
            print_list(tests, count);
            wait_for_continue();
            wrong_attempts = 0;
            continue ;
        }
        if (strcmp(input, "all") == 0)
        {
            clear_screen();
            printf("\n\033[1;34m--- TESTING ALL ---\033[0m\n");
            run_group(tests, count, 0);
            wait_for_continue();
            wrong_attempts = 0;
            continue ;
        }
        if (strcmp(input, "bonus") == 0)
        {
            clear_screen();
            printf("\n\033[1;34m--- bonus ---\033[0m\n");
            run_group(tests, count, 1);
            wait_for_continue();
            wrong_attempts = 0;
            continue ;
        }
        
        found = 0;
        i = 0;
        while (i < count)
        {
            if (strcmp(input, tests[i]) == 0
                || (strncmp(input, "ft_", 3) == 0 && strcmp(input, tests[i]) == 0))
            {
                found = 1;
                clear_screen();
                run_test(tests[i]);
                wait_for_continue();
                wrong_attempts = 0;
                break ;
            }
            i++;
        }
        if (!found)
        {
            int test_num = atoi(input);
            if (test_num > 0 && test_num <= count)
            {
                clear_screen();
                run_test(tests[test_num - 1]);
                wait_for_continue();
                wrong_attempts = 0;
            }
            else
            {
                printf("\033[1;31mUnknown or unavailable test: %s\033[0m\n", input);
                wrong_attempts++;
            }
        }
    }
    free_tests(tests, count);
    return (0);
}
