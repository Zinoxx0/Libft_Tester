#ifndef LISTS_H
# define LISTS_H

/*
** List helpers for the Part 3 tests. They use plain malloc, so a broken
** ft_lstnew doesn't make every other list test fail.
*/

# include "libft.h"
# include <stdio.h>
# include <string.h>

static inline t_list	*new_node(void *content)
{
	t_list	*node;

	node = malloc(sizeof(*node));
	node->content = content;
	node->next = NULL;
	return (node);
}

/* A node whose content is a malloc'd copy of s */
static inline t_list	*new_named(const char *s)
{
	char	*content;

	content = malloc(strlen(s) + 1);
	memcpy(content, s, strlen(s) + 1);
	return (new_node(content));
}

/* A list of n nodes whose contents are malloc'd strings "0", "1", ... */
static inline t_list	*new_list(int n)
{
	t_list	*head;
	t_list	**tail;
	char	*content;
	int		i;

	head = NULL;
	tail = &head;
	i = 0;
	while (i < n)
	{
		content = malloc(12);
		snprintf(content, 12, "%d", i++);
		*tail = new_node(content);
		tail = &(*tail)->next;
	}
	return (head);
}

/* Node i of a list, or NULL */
static inline t_list	*nth(t_list *list, int i)
{
	while (list && i-- > 0)
		list = list->next;
	return (list);
}

/* The contents of a list of strings joined by spaces, for messages */
static inline const char	*list_str(t_list *list)
{
	static char	buf[256];
	size_t		len;
	int			n;

	buf[0] = '\0';
	len = 0;
	n = 0;
	while (list && len < sizeof(buf) - 24 && n++ < 40)
	{
		len += snprintf(buf + len, sizeof(buf) - len, len ? " %s" : "%s",
				list->content ? (char *)list->content : "NULL");
		list = list->next;
	}
	if (list)
		snprintf(buf + len, sizeof(buf) - len, " ...");
	return (buf);
}

static inline void	free_list(t_list *list)
{
	t_list	*next;

	while (list)
	{
		next = list->next;
		free(list->content);
		free(list);
		list = next;
	}
}

#endif
