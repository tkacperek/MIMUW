/** @file
    Interfejs węzła w drzewie trie.

    @ingroup dictionary
    @author Tomasz Kacperek <tk334578@students.mimuw.edu.pl>
    @date 2015-06-07
 */

#ifndef __TRIE_NODE_H__
#define __TRIE_NODE_H__

#include "trie_node_commons.h"
#include "trie_node_array.h"

/// Struktura reprezentująca węzeł w drzewie trie.
struct trie_node
{
    /// Znak węzła.
    wchar_t character;
    /// Informuje, czy węzeł kończy słowo.
    int ends_word;
    /// Rodzic w drzewie.
    struct trie_node *parent;
    /// Tablica dzieci.
    struct trie_node_array *children;
};

/**
  Tworzy nowy węzeł.
  @param[in] c Znak węzła.
  @param[in] ew Czy kończy słowo.
  @return Nowy węzeł.
 */
struct trie_node * trie_node_new(const wchar_t c, const int ew);

/**
  Niszczy węzeł, który nie ma żadnych powiązan.
  @param [in,out] node Węzeł do zniszczenia.
  @return 1 jeśli się udało, 0 w p.p.
 */
int trie_node_kill(struct trie_node *node);

/**
  Zwraca dziecko z podanym znakiem.
  @param[in] node Węzeł.
  @param[in] c Znak.
  @return Odpowiedni węzeł, jeżeli taki istnieje, NULL w p.p.
 */
struct trie_node * 
trie_node_get_child(const struct trie_node *node, const wchar_t c);


/**
  Zwraca znak węzła.
  @param[in] node Węzeł.
  @return Znak.
 */
static inline
wchar_t trie_node_get_character(const struct trie_node *node)
{
    return node->character;
}

/**
  Informuje czy węzeł kończy słowo.
  @param[in] node Węzeł.
  @return 1 lub 0.
 */
static inline
int trie_node_ends_word(const struct trie_node *node)
{
    return node->ends_word;
}

/**
  Ustala, czy węzeł kończy słowo.
  @param[in,out] node Węzeł.
  @param[in] ew Ustalana wartośc.
 */
static inline
void trie_node_set_ends_word(struct trie_node *node, int ew)
{
    node->ends_word = ew;
}

/**
  Zwraca rodzica.
  @param[in] node Węzeł.
  @return Rodzic.
 */
static inline
struct trie_node *trie_node_get_parent(const struct trie_node *node)
{
    return node->parent;
}

/**
  Zwraca liczbę dzieci.
  @param[in] node Węzeł.
  @return Liczba dzieci.
 */
static inline
size_t trie_node_child_count(const struct trie_node *node)
{
    return trie_node_array_size(node->children);
}

/**
  Zwraca tablicę dzieci.
  @param[in] node Węzeł.
  @return Tablica dzieci.
 */
static inline
struct trie_node * const * trie_node_get_children(const struct trie_node *node)
{
    return trie_node_array_get(node->children);
}

/**
  Niszczy gałąź (węzeł i potomków).
  Odłącza gałąź od rodzica, jeżeli taki był.
  @param[in,out] node Węzeł.
 */
void trie_node_kill_branch(struct trie_node *node);

/**
  Dodaje dziecko do węzła.
  @param[in,out] parent Węzeł-rodzic.
  @param[in,out] child Węzeł-dziecko.
  @return 1 jeśli się udało, 0 w p.p.
 */
int trie_node_add_child(struct trie_node *parent, struct trie_node *child);

/**
  Usuwa dziecko węzła (nie niszczy dziecka).
  @param[in,out] parent Węzeł-rodzic.
  @param[in,out] child Węzeł-dziecko.
  @return 1 jeśli się udało, 0 w p.p.
 */
int trie_node_remove_child(struct trie_node *parent, struct trie_node *child);

#endif /* __TRIE_NODE_H__ */
