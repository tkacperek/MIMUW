/** @file
    Interfejs listy słów.

    @ingroup dictionary
    @author Jakub Pawlewicz <pan@mimuw.edu.pl>
    @author Tomasz Kacperek <tk334578@students.mimuw.edu.pl>
    @copyright Uniwerstet Warszawski
    @date 2015-06-06
 */

#ifndef __WORD_LIST_H__
#define __WORD_LIST_H__

#include <stdlib.h>
#include <wchar.h>
#include "word_list_s.h"

/// Domyślna pojemność listy słów po zainicjalizowaniu.
#define WORD_LIST_DEFAULT_ARRAY_CAPACITY 40

/// Domyślna pojemność bufora po zainicjalizowaniu
#define WORD_LIST_DEFAULT_BUFFER_CAPACITY 512

/// Licznik współczynnika przyrostu listy.
#define WORD_LIST_EXTEND_RATIO_NUMERATOR 3

/// Mianownik współczynnika przyrostu listy.
#define WORD_LIST_EXTEND_RATIO_DENOMINATOR 2

/**
  Struktura przechowująca listę słów.
  Należy używać funkcji operujących na strukturze,
  gdyż jej implementacja może się zmienić.
 */
struct word_list
{
    /// Tablica słów.
    const wchar_t **array;
    /// Liczba słów.
    size_t array_size;
    /// Pojemność tablicy słów.
    size_t array_capacity;

    /// Bufor, w którym pamiętane są słowa.
    wchar_t *buffer;
    /// Łączna liczba znaków.
    size_t buffer_size;
    /// Pojemność bufora.
    size_t buffer_capacity;
};

/**
  Inicjuje listę słów.
  @param[in,out] list Lista słów.
 */
void word_list_init(struct word_list *list);

/**
  Destrukcja listy słów.
  @param[in,out] list Lista słów.
 */
void word_list_done(struct word_list *list);

/**
  Dodaje słowo do listy.
  Lista po dodaniu jest posortowana.
  Słowo, które wcześniej znajdowało się na liście, nie zostanie dodane.
  @param[in,out] list Lista słów.
  @param[in] word Dodawane słowo.
  @return 1 jeśli się udało, 0 w p.p.
 */
int word_list_add(struct word_list *list, const wchar_t *word);

/**
  Zwraca liczbę słów w liście.
  @param[in] list Lista słów.
  @return Liczba słów w liście.
 */
static inline
size_t word_list_size(const struct word_list *list)
{
    return list->array_size;
}

/**
  Zwraca tablicę słów w liście.
  @param[in] list Lista słów.
  @return Tablica słów.
 */
static inline
const wchar_t * const * word_list_get(const struct word_list *list)
{
    return list->array;
}

/**
  Konwertuje listę do wektora GNU Argz w kodowaniu ASCII.
  Utworzony niepusty wektor należy zwolnić przy pomocy free(*vector).
  @param[in,out] list Lista słów.
  @param[out] argz Wektor.
  @param[out] argz_len Rozmiar utworzonego wektora.
  @return 1 jeśli się udało, 0 gdy lista zawiera niepoprawne znaki.
 */
int word_list_to_argz(struct word_list *list, char **argz, size_t *argz_len);

/**
  Inicjuje listę słów z danymi z listy posortowanej.
  @param[in,out] list Lista słów.
  @param[in] src Kopiowana lista.
 */
void word_list_init_from_s(struct word_list *list, struct word_list_s *src);

#endif /* __WORD_LIST_H__ */
