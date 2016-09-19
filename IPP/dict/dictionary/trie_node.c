/** @file
    Implementacja węzła w drzewie trie.

    @ingroup dictionary
    @author Tomasz Kacperek <tk334578@students.mimuw.edu.pl>
    @date 2015-06-07
 */

#include "trie_node.h"

/** @name Elementy interfejsu
   @{
 */

struct trie_node * trie_node_new(const wchar_t c, const int ew)
{
    struct trie_node *node = malloc(sizeof(struct trie_node));
    node->character = c;
    node->ends_word = ew;
    node->parent = NULL;
    node->children = trie_node_array_new();
    return node;
}

int trie_node_kill(struct trie_node *node)
{
    if (node->parent != NULL || trie_node_array_size(node->children) != 0)
        return 0;
    trie_node_array_done(node->children);
    free(node);
    return 1;
}

struct trie_node *
trie_node_get_child(const struct trie_node *node, const wchar_t c)
{
    return trie_node_array_get_node(node->children, c);
}

void trie_node_kill_branch(struct trie_node *node)
{
    if (node->parent != NULL)
        trie_node_remove_child(node->parent, node);
    size_t size = trie_node_array_size(node->children);
    if (size > 0)
    {
        struct trie_node ** children = calloc(size, sizeof(struct trie_node *));
        for (size_t i = 0; i < size; ++i)
            children[i] = trie_node_array_get(node->children)[i];
        for (size_t i = 0; i < size; ++i)
        {
            trie_node_kill_branch(children[i]);
        }
        free(children);
    }
    trie_node_kill(node);
}

int trie_node_add_child(struct trie_node *parent, struct trie_node *child)
{
    if (child->parent != NULL)
        return 0;
    if (trie_node_array_add(parent->children, child))
    {
        child->parent = parent;
        return 1;
    }
    return 0;
}

int trie_node_remove_child(struct trie_node *parent, struct trie_node *child)
{
    if (child->parent == NULL 
        || trie_node_get_child(parent, child->character) == NULL)
        return 0;
    child->parent = NULL;
    trie_node_array_remove_node(parent->children, child);
    return 1;
}

/**@}*/
