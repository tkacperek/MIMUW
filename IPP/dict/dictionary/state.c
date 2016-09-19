/** @file
  Implementacja struktury stanu.

  @ingroup dictionary
  @author Tomasz Kacperek <tk334578@students.mimuw.edu.pl>
  @copyright Uniwerstet Warszawski
  @date 2015-06-22
 */

#include "state.h"
#include "word_list.h"
#include <assert.h>

struct state *state_new(const wchar_t *s, struct trie_node *n, 
        struct trie_node *p, size_t c, int f)
{
    struct state *ns = malloc(sizeof(struct state));
    ns->suffix = calloc(wcslen(s) + 1, sizeof(wchar_t));
    wcscpy(ns->suffix, s);
    ns->node = n;
    ns->prev = p;
    ns->cost = c;
    ns->final = f;
    ns->ignore = 0;
    if (wcslen(ns->suffix) == 0 && ns->node->ends_word)
        ns->end = 1;
    else
        ns->end = 0;
    return ns;
}

struct state *state_copy(const struct state *s)
{
    struct state * news = state_new(s->suffix, s->node, s->prev, s->cost, s->final);
    news->ignore = s->ignore;
    return news;
}

void state_done(struct state *s)
{
    free(s->suffix);
    free(s);
}

int state_compare_abs(struct state *s1, struct state *s2)
{
    if (s1 == NULL && s2 == NULL)
        return 0;
    else if (s1 == NULL)
        return -1;
    else if (s2 == NULL)
        return 1;

    long tmp = (long)(s2->prev) - (long)(s1->prev);
    if (tmp == 0)
    {
        tmp = (long)(s2->node) - (long)(s1->node);
        if (tmp == 0)
        {
            tmp = wcscmp(s1->suffix, s2->suffix);
            if (tmp == 0)
                tmp = (long)(s1->final - s2->final);
        }
    }
    int ret;
    if (tmp == 0)
        ret = 0;
    else if (tmp < 0)
        ret = -1;
    else
        ret = 1;
    return ret;
}

int state_compare(struct state *s1, struct state *s2)
{

    int tmp = state_compare_abs(s1, s2);
    if (tmp != 0)
        return tmp;
    else
        return s1->cost - s2->cost;
}

wchar_t *state_extract(struct state *s)
{
    wchar_t single[2] = {(wchar_t)(0), (wchar_t)(0)};
    struct word_list wl;
    word_list_init(&wl);
    struct trie_node *tmp_node;

    tmp_node = s->node;
    while (tmp_node->parent)
    {
        single[0] = tmp_node->character;
        word_list_add(&wl, single);
        tmp_node = tmp_node->parent;
    }
    if (s->prev)
    {
        single[0] = L' '; 
        word_list_add(&wl, single);
        tmp_node = s->prev;
        while (tmp_node->parent)
        {
            single[0] = tmp_node->character;
            word_list_add(&wl, single);
            tmp_node = tmp_node->parent;
        }
    }
    long i, n = word_list_size(&wl);
    wchar_t *almost_there = calloc(n + 1, sizeof(wchar_t));
    for (i = n - 1; i >= 0; --i)
        almost_there[n - 1 - i] = wl.array[i][0];
    almost_there[n] = (wchar_t)(0);
    word_list_done(&wl);
    return almost_there;
}
