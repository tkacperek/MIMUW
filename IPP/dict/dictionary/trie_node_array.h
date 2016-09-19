/** @file
    Interfejs posortowanej tablicy węzłów drzewa trie bez powtórzeń.

    @ingroup dictionary
    @author Tomasz Kacperek <tk334578@students.mimuw.edu.pl>
    @date 2015-06-07
 */

#ifndef __TRIE_NODE_ARRAY_H__
#define __TRIE_NODE_ARRAY_H__

#include "trie_node_commons.h"
#include "trie_node.h"
#include <assert.h>

/// Domyślna pojemność tablicy po zainicjalizowaniu.
#define TRIE_NODE_ARRAY_DEFAULT_CAPACITY 5

/// Licznik współczynnika kurczenia.
#define TRIE_NODE_ARRAY_SHRINK_RATIO_NUMERATOR 1

/// Mianownik współczynnika kurczenia.
#define TRIE_NODE_ARRAY_SHRINK_RATIO_DENOMINATOR 2

/// Licznik współczynnika przyrostu.
#define TRIE_NODE_ARRAY_EXTEND_RATIO_NUMERATOR 3

/// Mianownik współczynnika przyrostu.
#define TRIE_NODE_ARRAY_EXTEND_RATIO_DENOMINATOR 2

/// Struktura przechowująca tablicę węzłow.
struct trie_node_array
{
    /// Tablica węzłów.
    struct trie_node **array;
    /// Rozmiar tablicy.
    size_t size;
    /// Pojemność tablicy.
    size_t capacity;
};

/**
  Zwraca tablicę węzłów.
  @return Nowa tablica węzłów.
 */
struct trie_node_array *trie_node_array_new();

/**
  Destrukcja tablicy węzłów.
  Nie niszczy węzłów.
  @param[in,out] array Tablica węzłów.
 */
void trie_node_array_done(struct trie_node_array *array);

/**
  Zwraca liczbę węzłów w tablicy.
  @param[in] array Tablica węzłów.
  @return Liczba węzłów w tablicy.
 */
static inline
size_t trie_node_array_size(const struct trie_node_array *array)
{
    assert(array != NULL);
    return array->size;
}

/**
  Zwraca tablicę węzłów.
  @param[in] array Tablica węzłów.
  @return Tablica węzłów.
 */
static inline
struct trie_node **trie_node_array_get(const struct trie_node_array *array)
{
    return array->array;
}

/**
  Dodaje węzeł do tablicy.
  Tablica po dodaniu jest posortowana.
  Przy próbie dodania węzła z tym samym znakiem
  tablica się nie zmienia i zwracana jest wartość 0.
  @param[in,out] array Tablica węzłów.
  @param[in] node Dodawany węzeł.
  @return 1 jeśli się udało, 0 w p.p.
 */
int trie_node_array_add(struct trie_node_array *array,
                        struct trie_node *node);

/**
  Usuwa z tablicy węzeł z podanej pozycji.
  Nie niszczy węzła.
  @param[in,out] array Tablica węzłów.
  @param[in] index Pozycja usuwanego węzła.
  @return 1 jeśli się udało, 0 w p.p.
 */
int trie_node_array_remove(struct trie_node_array *array, const size_t index);

/**
  Zwraca indeks węzła o podanym znaku.
  @param[in] array Tablica węzłów.
  @param[in] character Znak.
  @return Indeks szukanego węzła jeżeli jest w tablicy, rozmiar tablicy w p.p.
 */
size_t trie_node_array_get_index(const struct trie_node_array *array,
                                 const wchar_t character);

/**
  Zwraca węzeł o podanej literze lub NULL, jeżeli go nie ma.
  @param[in] array Tablica węzłów.
  @param[in] character Znak.
  @return Szukany węzeł lub NULL.
 */
struct trie_node *trie_node_array_get_node(const struct trie_node_array *array,
                                           const wchar_t character);

/**
  Usuwa węzeł z tablicy.
  @param[in,out] array Tablica węzłów.
  @param[in] node Usuwany węzeł.
  @return 1 jeśli się udało, 0 w p.p.
 */
int trie_node_array_remove_node(struct trie_node_array *array,
                                struct trie_node *node);

#endif /* __TRIE_NODE_ARRAY_H__ */
