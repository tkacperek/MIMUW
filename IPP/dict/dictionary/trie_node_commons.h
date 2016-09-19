/** @file
    Wspólny fragment interfejsu dla trie_node oraz trie_node_array.

    @ingroup dictionary
    @author Tomasz Kacperek <tk334578@students.mimuw.edu.pl>
    @date 2015-06-07
 */

#ifndef __TRIE_NODE_COMMONS_H__
#define __TRIE_NODE_COMMONS_H__

#include <stdlib.h>
#include <wchar.h>

struct trie_node;
struct trie_node_array;

/**
  Zwraca liczbę węzłów w tablicy.
  @param[in] array Tablica węzłów.
  @return Liczba węzłów w tablicy.
 */
static inline
size_t trie_node_array_size(const struct trie_node_array *array);

/**
  Zwraca tablicę węzłów.
  @param[in] array Tablica węzłów.
  @return Tablica węzłów.
 */
static inline
struct trie_node **trie_node_array_get(const struct trie_node_array *array);

#endif /* __TRIE_NODE_COMMONS_H__ */
