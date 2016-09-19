/** @file
    Interfejs posortowanej listy słów bez powtórzeń.

    @ingroup dictionary
    @author Jakub Pawlewicz <pan@mimuw.edu.pl>
    @author Tomasz Kacperek <tk334578@students.mimuw.edu.pl>
    @copyright Uniwerstet Warszawski
    @date 2015-06-06
 */

#ifndef __WORD_LIST_S_H__
#define __WORD_LIST_S_H__

#include <stdlib.h>
#include <wchar.h>

/// Domyślna pojemność listy słów po zainicjalizowaniu.
#define WORD_LIST_S_DEFAULT_ARRAY_CAPACITY 40

/// Domyślna pojemność bufora po zainicjalizowaniu
#define WORD_LIST_S_DEFAULT_BUFFER_CAPACITY 512

/// Licznik współczynnika kurczenia listy.
#define WORD_LIST_S_SHRINK_RATIO_NUMERATOR 1

/// Mianownik współczynnika kurczenia listy.
#define WORD_LIST_S_SHRINK_RATIO_DENOMINATOR 2

/// Licznik współczynnika przyrostu listy.
#define WORD_LIST_S_EXTEND_RATIO_NUMERATOR 3

/// Mianownik współczynnika przyrostu listy.
#define WORD_LIST_S_EXTEND_RATIO_DENOMINATOR 2

/**
  Struktura przechowująca listę słów.
  Należy używać funkcji operujących na strukturze,
  gdyż jej implementacja może się zmienić.
 */
struct word_list_s
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
    /// Ilość znaków, które nie są potrzebne.
    size_t removed_size;
};

/**
  Inicjuje listę słów.
  @param[in,out] list Lista słów.
 */
void word_list_s_init(struct word_list_s *list);

/**
  Destrukcja listy słów.
  @param[in,out] list Lista słów.
 */
void word_list_s_done(struct word_list_s *list);

/**
  Dodaje słowo do listy.
  Lista po dodaniu jest posortowana.
  Słowo, które wcześniej znajdowało się na liście, nie zostanie dodane.
  @param[in,out] list Lista słów.
  @param[in] word Dodawane słowo.
  @return 1 jeśli się udało, 0 w p.p.
 */
int word_list_s_add(struct word_list_s *list, const wchar_t *word);

/**
  Usuwa z listy słowo z podanej pozycji.
  @param[in,out] list Lista słów.
  @param[in] index Pozycja usuwanego słowa.
  @return 1 jeśli się udało, 0 w p.p.
 */
int word_list_s_remove(struct word_list_s *list, const size_t index);

/**
  Zwraca liczbę słów w liście.
  @param[in] list Lista słów.
  @return Liczba słów w liście.
 */
static inline
size_t word_list_s_size(const struct word_list_s *list)
{
    return list->array_size;
}

/**
  Zwraca tablicę słów w liście.
  @param[in] list Lista słów.
  @return Tablica słów.
 */
static inline
const wchar_t * const * word_list_s_get(const struct word_list_s *list)
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
int word_list_s_to_argz(struct word_list_s *list, char **argz, size_t *argz_len);

#endif /* __WORD_LIST_S_H__ */
