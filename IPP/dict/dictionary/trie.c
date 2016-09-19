/** @file
    Implementacja drzewa trie.

    @ingroup dictionary
    @author Tomasz Kacperek <tk334578@students.mimuw.edu.pl>
    @date 2015-06-08
 */

#include "trie.h"
#include <wctype.h>
#include <stdio.h>

/** @name Funkcje pomocnicze
  @{
 */

/**
  Zwraca wskaźnik do węzła, któremu odpowiada najdłuższy prefiks
  szukanego słowa.
  @param[in] trie Drzewo.
  @param[in] word Słowo.
  @param[out] len Długość pasującego podsłowa.
  @return Wskaźnik na szukany węzeł.
 */
static
struct trie_node *partial_find(const struct trie *trie,
                               const wchar_t *word, size_t *len)
{
    size_t i = 0, n = wcslen(word);
    struct trie_node *tmp_child, *tmp = trie->root;
    *len = 0;
    while (i < n)
    {
        tmp_child = trie_node_get_child(tmp, word[i]);
        if (tmp_child == NULL)
            i = n;
        else
        {
            tmp = tmp_child;
            ++(*len);
        }
        ++i;
    }
    return tmp;
}

/**
  Serializuje poddrzewo.
  @param[in] node Korzeń poddrzewa.
  @param[out] stream Plik, do którego będzie zapisywane drzewo.
  @return -1 przy niepowodzeniu, 0 w p.p.
 */
static
int serialize(const struct trie_node *node, FILE *stream)
{
    if (fprintf(stream, "%lc", trie_node_get_character(node)) < 0)
        return -1;
    size_t n = trie_node_child_count(node);
    struct trie_node * const *children = trie_node_get_children(node);
    for (size_t i = 0; i < n; ++i)
        if (serialize(children[i], stream) < 0)
            return -1;
    if (fprintf(stream, "%lc", ((trie_node_ends_word(node)) ? L'1' : L'0')) < 0)
        return -1;
    return 0;
}

/**@}*/

/** @name Elementy interfejsu
   @{
 */

struct trie *trie_new()
{
    struct trie *trie = malloc(sizeof(struct trie));
    trie->root = trie_node_new(L'#', 0);
    trie->size = 1;
    word_list_s_init(&trie->characters);
    return trie;
}

void trie_kill(struct trie *trie)
{
    trie_node_kill_branch(trie->root);
    word_list_s_done(&trie->characters);
    free(trie);
}

int trie_find(const struct trie *trie, const wchar_t* word)
{
    size_t len;
    struct trie_node *match = partial_find(trie, word, &len);
    if (len == wcslen(word) && trie_node_ends_word(match) == 1)
       return 1;
    else
       return 0;
}

int trie_serialize(const struct trie *trie, FILE *stream)
{
    int ret = 0;
    ret = serialize(trie->root, stream);
    if (fprintf(stream, "%lc", L'\0') < 0)
        ret = -1;
    return ret;
}

struct trie *trie_deserialize(const wchar_t *str)
{
    int error = 0;
    size_t depth, n = wcslen(str);
    if (n < 2)
        return NULL;
    if (n % 2 == 1 || str[0] != L'#' || str[n - 1] != L'0')
        return NULL;
    struct trie *trie = trie_new();
    struct trie_node *new_node, *tmp = trie->root;
    depth = 0;
    for (size_t i = 1; i < n - 1; ++i)
    {
        if (iswlower(str[i]))
        {
            new_node = trie_node_new(str[i], 0);
            if (trie_node_add_child(tmp, new_node))
            {
                tmp = new_node;
                ++depth;
                wchar_t tmp_str[2] = {str[i],L'\0'};
                word_list_s_add(&trie->characters, tmp_str);
            }
            else
            {
                trie_node_kill(new_node);
                error = 1;
                break;
            }
        }
        else if (str[i] == L'0' || str[i] == L'1')
        {
            if (depth > 0)
            {
                trie_node_set_ends_word(tmp, (str[i] == L'0') ? 0 : 1);
                tmp = trie_node_get_parent(tmp);
                --depth;
            }
            else
            {
                error = 1;
                break;
            }
        }
        else
        {
            error = 1;
            break;
        }
    }
    if (depth != 0)
        error = 1;
    if (error)
    {
        trie_kill(trie);
        return NULL;
    }
    return trie;
}

int trie_insert(struct trie *trie, const wchar_t* word)
{
    size_t len, n = wcslen(word);
    if (n == 0)
        return 0;
    struct trie_node *new_node, *match = partial_find(trie, word, &len);
    if (len == n)
    {
        if (trie_node_ends_word(match) == 1)
            return 0;
        else
        {
            trie_node_set_ends_word(match, 1);
            return 1;
        }
    }
    for (size_t i = len; i < n; ++i)
    {
        new_node = trie_node_new(word[i], 0);
        trie_node_add_child(match, new_node);
        trie->size += 1;
        match = new_node;
        wchar_t str[2] = {word[i],L'\0'};
        word_list_s_add(&trie->characters, str);
    }
    trie_node_set_ends_word(match, 1);
    return 1;
}

int trie_delete(struct trie *trie, const wchar_t* word)
{
    size_t deleted_nodes = 0, len, n = wcslen(word);
    if (n == 0)
        return 0;
    struct trie_node *prev, *match = partial_find(trie, word, &len);
    if (len != n || trie_node_ends_word(match) == 0)
        return 0;
    if (trie_node_child_count(match) > 0)
    {
        trie_node_set_ends_word(match, 0);
        return 1;
    }
    while (trie_node_child_count(match) < 2 && match != trie->root)
    {
        prev = match;
        match = trie_node_get_parent(match);
        ++deleted_nodes;
    }
    trie_node_kill_branch(prev);
    trie->size -= deleted_nodes;
    return 1;
}

/**@}*/
