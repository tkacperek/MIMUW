/** @file
    Interfejs tablicy stanów.

    @ingroup dictionary
    @author Tomasz Kacperek <tk334578@students.mimuw.edu.pl>
    @date 2015-06-22
 */

#ifndef __STATE_ARRAY_H__
#define __STATE_ARRAY_H__

#include <stdlib.h>
#include <wchar.h>

/// Domyślna pojemność tablicy po zainicjalizowaniu.
#define STATE_ARRAY_DEFAULT_CAPACITY 16

/// Licznik współczynnika przyrostu.
#define STATE_ARRAY_EXTEND_RATIO_NUMERATOR 3

/// Mianownik współczynnika przyrostu.
#define STATE_ARRAY_EXTEND_RATIO_DENOMINATOR 2

/// Struktura przechowująca tablicę stanów.
struct state_array
{
    /// Tablica stanów.
    struct state **array;
    /// Rozmiar tablicy.
    size_t size;
    /// Pojemność tablicy.
    size_t capacity;
};

/**
  Zwraca tablicę stanów.
  @return Nowa tablica stanów.
 */
struct state_array *state_array_new();

/**
  Zwraca tablicę stanów o zadanej pojemności.
  @return Nowa tablica stanów.
 */
struct state_array *state_array_new_n(size_t n);

/**
  Destrukcja tablicy stanów.
  Nie niszczy stanów.
  @param[in,out] array Tablica stanów.
 */
void state_array_done(struct state_array *array);

/**
  Destrukcja tablicy stanów z zawartymi stanami.
  @param[in,out] array Tablica stanów.
 */
void state_array_kill(struct state_array *array);

/**
  Zwraca liczbę stanów w tablicy.
  @param[in] array Tablica stanów.
  @return Liczba stanów w tablicy.
 */
static inline
size_t state_array_size(const struct state_array *array)
{
    return array->size;
}

/**
  Zwraca tablicę stanów.
  @param[in] array Tablica stanów.
  @return Tablica stanów.
 */
static inline
struct state **state_array_get(const struct state_array *array)
{
    return array->array;
}

/**
  Dodaje stan do tablicy.
  @param[in,out] array Tablica stanów.
  @param[in] node Dodawana stanów.
  @return 1 jeśli się udało, 0 w p.p.
 */
int state_array_add(struct state_array *array, struct state *node);

/**
  Dodaje kopię stanu do tablicy.
  @param[in,out] array Tablica stanów.
  @param[in] node Dodawana stanów.
  @return 1 jeśli się udało, 0 w p.p.
 */
int state_array_add_copy(struct state_array *array, struct state *node);

/**
  Sortuje tablicę stanów.
  @param[in,out] array Tablica do posortowania.
 */
void state_array_sort(struct state_array *array);

#endif /* __STATE_ARRAY_H__ */
