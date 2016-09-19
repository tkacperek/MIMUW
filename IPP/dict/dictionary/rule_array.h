/** @file
    Interfejs tablicy reguł.

    @ingroup dictionary
    @author Tomasz Kacperek <tk334578@students.mimuw.edu.pl>
    @date 2015-06-21
 */

#ifndef __RULE_ARRAY_H__
#define __RULE_ARRAY_H__

#include <stdlib.h>
#include <wchar.h>

/// Domyślna pojemność tablicy po zainicjalizowaniu.
#define RULE_ARRAY_DEFAULT_CAPACITY 16

/// Licznik współczynnika przyrostu.
#define RULE_ARRAY_EXTEND_RATIO_NUMERATOR 3

/// Mianownik współczynnika przyrostu.
#define RULE_ARRAY_EXTEND_RATIO_DENOMINATOR 2

/// Struktura przechowująca tablicę reguł.
struct rule_array
{
    /// Tablica reguł.
    struct rule **array;
    /// Rozmiar tablicy.
    size_t size;
    /// Pojemność tablicy.
    size_t capacity;
    /// Czy tablica jest posortowana.
    int sorted;
};

/**
  Zwraca tablicę reguł.
  @return Nowa tablica reguł.
 */
struct rule_array *rule_array_new();

/**
  Tworzy nową tablicę o zadanej pojemności.
  @param[in] n Rozmiar początkowy.
  @return Tablica.
 */
struct rule_array *rule_array_new_n(size_t n);

/**
  Destrukcja tablicy reguł.
  Nie niszczy reguł.
  @param[in,out] array Tablica reguł.
 */
void rule_array_done(struct rule_array *array);

/**
  Destrukcja tablicy reguł z zawartymi regułami.
  @param[in,out] array Tablica reguł.
 */
void rule_array_kill(struct rule_array *array);

/**
  Zwraca liczbę reguł w tablicy.
  @param[in] array Tablica reguł.
  @return Liczba reguł w tablicy.
 */
static inline
size_t rule_array_size(const struct rule_array *array)
{
    return array->size;
}

/**
  Zwraca tablicę reguł.
  @param[in] array Tablica reguł.
  @return Tablica reguł.
 */
static inline
struct rule **rule_array_get(const struct rule_array *array)
{
    return array->array;
}

/**
  Dodaje regułę do tablicy.
  @param[in,out] array Tablica reguł.
  @param[in] node Dodawana reguła.
  @return 1 jeśli się udało, 0 w p.p.
 */
int rule_array_add(struct rule_array *array, struct rule *node);

/**
  Serializuje tablicę reguł.
  @param[in] a Tablica reguł.
  @param[out] s Strumień do zapisu.
  @return <0 jeśli operacja się nie powiedzie, 0 w p.p.
 */
int rule_array_serialize(struct rule_array *a, FILE *s);

/**
  Odtwarza tablicę reguł.
  @param[in] s Strumień, z którego będzie odczytana tablica.
  @return Nowa tablica z odpowiednią zawartością lub NULL przy niepowodzeniu.
 */
struct rule_array *rule_array_deserialize(FILE *s);

/**
  Sortuje tablicę rosnąco wg kosztu.
  @param[in,out] ra Tablica
 */
void rule_array_sort(struct rule_array *ra);

#endif /* __RULE_ARRAY_H__ */
