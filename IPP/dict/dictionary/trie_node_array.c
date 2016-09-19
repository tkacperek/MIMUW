/** @file
    Implementacja posortowanej tablicy węzłów drzewa trie bez powtórzeń.

    @ingroup dictionary
    @author Tomasz Kacperek <tk334578@students.mimuw.edu.pl>
    @date 2015-06-07
 */

#include "trie_node_array.h"
#include <stdio.h>

/** @name Funkcje pomocnicze
  @{
 */

/**
  Dodaje węzeł na końcu tablicy.
  Zakłada, że węzeł zmieści się w tablicy.
  @param[in,out] array Tablica węzłów.
  @param[in] node Dodawany węzeł.
  @return 1 jeśli się udało, 0 w p.p.
 */
static
int push_back(struct trie_node_array *array,
              struct trie_node *node)
{
    array->array[array->size++] = node;
    return 1;
}

/**
  Zmienia pojemność tablicy węzłów.
  @param[in,out] array Tablica węzłów.
  @param[in] ratio_numerator Licznik współczynnika zmiany rozmiaru.
  @param[in] ratio_denominator Mianownik współczynnika zminay rozmiaru.
  @return 1 jeśli się udało, 0 w p.p.
 */
static
int resize_array(struct trie_node_array *array,
                 const size_t ratio_numerator, const size_t ratio_denominator)
{
    size_t i, new_array_capacity =
        (array->capacity * ratio_numerator) / ratio_denominator;
    if (array->size > new_array_capacity)
        return 0;
    struct trie_node **new_array = calloc(new_array_capacity, sizeof(struct trie_node *));
    if (! new_array)
        return 0;
    for (i = 0; i < array->size; ++i)
        new_array[i] = array->array[i];
    free (array->array);
    array->capacity = new_array_capacity;
    array->array = new_array;
    return 1;
}

/**
  Zmniejsza rozmiar tablicy.
  @param[in,out] array Tablica węzłów.
  @return 1 jeśli się udało, 0 w p.p.
 */
static
int shrink_array(struct trie_node_array *array)
{
    return resize_array(array,
                 TRIE_NODE_ARRAY_SHRINK_RATIO_NUMERATOR,
                 TRIE_NODE_ARRAY_SHRINK_RATIO_DENOMINATOR);
}

/**
  Zwiększa rozmiar tablicy.
  @param[in,out] array Tablica węzłów. 
  @return 1 jeśli się udało, 0 w p.p.
 */
static
int extend_array(struct trie_node_array *array)
{
    return resize_array(array,
                 TRIE_NODE_ARRAY_EXTEND_RATIO_NUMERATOR,
                 TRIE_NODE_ARRAY_EXTEND_RATIO_DENOMINATOR);
}

/**
  Usuwa ostani węzeł w tablicy.
  @param[in,out] array Tablica węzłów. 
  @return 1 jeśli się udało, 0 w p.p.
 */
static
int remove_last(struct trie_node_array *array)
{
    if (array->size == 0)
        return 0;
    array->size--;
    return 1;
}

/**
  Zwraca znak węzła na danej pozycji.
  @param[in] array Tablica węzłów.
  @param[in] index Pozycja.
  @return Szukany znak.
 */
static
const wchar_t get_char(const struct trie_node_array *array, const size_t index)
{
    return array->array[index]->character;
}

/**
  Porównuje znaki przy pomocy funkcji wcscoll.
  @param[in] c1 Pierwszy znak.
  @param[in] c2 Drugi znak.
  @return 0 gdy równe, >0 gdy c1>c2, <0 w p.p.
 */
static
int wc_coll(const wchar_t c1, const wchar_t c2)
{
    const wchar_t str1[2] = {c1, L'\0'}, str2[2] = {c2, L'\0'};
    return wcscoll(str1, str2);
}

/**
  Zwraca pozycję węzła najbliżeszego do szukanego.
  (czyli pozycję węzła, jeżeli występuje, lub jego sąsiada, gdyby występował)
  @param[in] array Tablica węzłów.
  @param[in] left_bound Początek badanego przedziału.
  @param[in] right_bound Koniec badanego przedziału.
  @param[in] key Szukany węzeł.
  @return Pozycja najbliższego węzła, lub 0 dla pustej tablicy.
 */
static
size_t bin_search(const struct trie_node_array *array,
                  size_t left_bound, size_t right_bound,
                  const struct trie_node *key)
{
    size_t mid, n = right_bound - left_bound + 1;
    if (n < 2)
        return 0;
    int cmp;
    const wchar_t key_char = trie_node_get_character(key);

    if (wc_coll(key_char, get_char(array, left_bound)) < 0)
        right_bound = left_bound;
    else if (wc_coll(key_char, get_char(array, right_bound)) > 0)
        left_bound = right_bound;
    while (right_bound - left_bound > 0)
    {
        mid = (right_bound + left_bound) / 2;
        cmp = wc_coll(key_char, get_char(array, mid));
        if (cmp == 0)
        {
            return mid;
        }
        else if (cmp < 0)
            right_bound = (mid > left_bound) ? mid - 1 : left_bound;
        else
            left_bound = (mid < right_bound) ? mid + 1 : right_bound;
    }
    return left_bound;
}

/**
  Wsortowuje ostani węzeł.
  @param[in,out] array Tablica węzłów. 
  @return 1 jeżeli wsortowanie się udało, 0 w p.p.
 */
static
int sort_last(struct trie_node_array *array)
{
    if (array->size < 2)
        return 1;
    int cmp;
    size_t i, pos;
    struct trie_node *last_node = array->array[array->size - 1];
    const wchar_t last_char = get_char(array, array->size - 1);
    pos = bin_search(array, 0, array->size - 2, last_node);  
    cmp = wc_coll(last_char, get_char(array, pos));
    if (cmp == 0)
    {
        remove_last(array);
        return 0;
    }
    if (cmp > 0)
        ++pos;
    // insert
    for (i = array->size - 1; i > pos; --i)
        array->array[i] = array->array[i - 1];
    array->array[pos] = last_node;
    return 1;
}    

/**@}*/

/** @name Elementy interfejsu
   @{
 */

struct trie_node_array *trie_node_array_new()
{
    struct trie_node_array *array = malloc(sizeof(struct trie_node_array));
    array->size = 0;
    array->capacity = TRIE_NODE_ARRAY_DEFAULT_CAPACITY;
    array->array = calloc(array->capacity, sizeof(struct trie_node *));
    if (!(array->array))
    {
        fprintf(stderr, "Out of memory.\n");
        exit(1);
    }
    return array;
}

void trie_node_array_done(struct trie_node_array *array)
{
    free(array->array);
    free(array);
}

int trie_node_array_add(struct trie_node_array *array, struct trie_node *node)
{
    if (array->size == array->capacity)
        extend_array(array);
    push_back(array, node);
    if (sort_last(array))
        return 1;
    else
        return 0;
}

int trie_node_array_remove(struct trie_node_array *array, const size_t index)
{
    if (array->size <= index)
        return 0;
    for (size_t i = index; i < array->size - 1; ++i)
        array->array[i] = array->array[i + 1];
    array->size -= 1;
    shrink_array(array);
    return 1;
}

size_t trie_node_array_get_index(const struct trie_node_array *array,
                                 const wchar_t character)
{
    struct trie_node *tmp_node = trie_node_new(character, 1);
    size_t pos = bin_search(array, 0, array->size - 1, tmp_node);
    trie_node_kill(tmp_node);
    if (array->size == 0)
        return 0;
    if (array->array[pos]->character == character)
        return pos;
    else
        return array->size;
}

struct trie_node *trie_node_array_get_node(const struct trie_node_array *array,
                                           const wchar_t character)
{
    size_t pos = trie_node_array_get_index(array, character);
    if (pos == array->size)
        return NULL;
    else
        return array->array[pos];
}

int trie_node_array_remove_node(struct trie_node_array *array,
                                struct trie_node *node)
{
    size_t pos = trie_node_array_get_index(array, trie_node_get_character(node));
    if (pos == array->size)
        return 0;
    else
    {
        return trie_node_array_remove(array, pos);
    }
}

/**@}*/
