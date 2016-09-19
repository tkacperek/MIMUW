/** @file
  Implementacja posortowanej listy słów bez powtórzeń.

  @ingroup dictionary
  @author Jakub Pawlewicz <pan@mimuw.edu.pl>
  @author Tomasz Kacperek <tk334578@students.mimuw.edu.pl>
  @copyright Uniwerstet Warszawski
  @date 2015-06-06
 */

#include "word_list.h"

/** @name Funkcje pomocnicze
  @{
 */

/**
  Dodaje niepuste słowo na końcu listy.
  Zakłada, że słowo zmieści się na liście.
  @param[in,out] list Lista słów.
  @param[in] word Dodawane słowo.
  @return 1 jeśli się udało, 0 w p.p. (puste słowo).
 */
static
int push_back(struct word_list *list, const wchar_t *word)
{
    // assuming that word fits in the list
    // +1 is for \0
    size_t word_length = wcslen(word) + 1;
    if (word_length <= 1)
        return 0;
    wchar_t *pos = list->buffer + list->buffer_size;
    list->array[list->array_size++] = pos;
    wcscpy(pos, word);
    list->buffer_size += word_length;
    return 1;
}

/**
  Zmienia pojemność tablicy słów.
  @param[in,out] list Lista słów.
  @param[in] ratio_numerator Licznik współczynnika zmiany rozmiaru.
  @param[in] ratio_denominator Mianownik współczynnika zminay rozmiaru.
  @return 1 jeśli się udało, 0 w p.p. 
 */
static
int resize_array(struct word_list *list,
                 const size_t ratio_numerator, const size_t ratio_denominator)
{
    size_t i, new_array_capacity =
        (list->array_capacity * ratio_numerator) / ratio_denominator;
    if (list->array_size > new_array_capacity)
        return 0;
    const wchar_t **new_array = calloc(new_array_capacity, sizeof(wchar_t *));
    if (! new_array)
        return 0;
    for (i = 0; i < list->array_size; ++i)
        new_array[i] = list->array[i];
    free (list->array);
    list->array_capacity = new_array_capacity;
    list->array = new_array;
    return 1;
}

/**
  Zmienia pojemność bufora listy słów.
  Sprząta usunięte słowa.
  @param[in,out] list Lista słów.
  @param[in] ratio_numerator Licznik współczynnika zmiany rozmiaru.
  @param[in] ratio_denominator Mianownik współczynnika zminay rozmiaru.
  @return 1 jeśli się udało, 0 w p.p. 
 */
static
int resize_buffer(struct word_list *list,
                  const size_t ratio_numerator, const size_t ratio_denominator)
{
    size_t i, n, new_buffer_capacity =
        (list->buffer_capacity * ratio_numerator) / ratio_denominator;
    if (list->buffer_size > new_buffer_capacity)
        return 0;
    wchar_t *old_buffer, *new_buffer;
    new_buffer = calloc(new_buffer_capacity, sizeof(wchar_t));
    if (! new_buffer)
        return 0;
    old_buffer = list->buffer;
    list->buffer_capacity = new_buffer_capacity;
    list->buffer = new_buffer;
    n = list->array_size;
    list->array_size = list->buffer_size = 0;
    for (i = 0; i < n; ++i)
        push_back(list, list->array[i]);
    free(old_buffer);
    return 1;
}

/**
  Zwiększa rozmiar tablicy.
  @param[in,out] list Lista słów.
  @return 1 jeśli się udało, 0 w p.p.
 */
static
int extend_array(struct word_list *list)
{
    return resize_array(list,
                 WORD_LIST_EXTEND_RATIO_NUMERATOR,
                 WORD_LIST_EXTEND_RATIO_DENOMINATOR);
}

/**
  Zwiększa rozmiar bufora.
  @param[in,out] list Lista słów.
  @return 1 jeśli się udało, 0 w p.p.
 */
static
int extend_buffer(struct word_list *list)
{
    return resize_buffer(list,
                 WORD_LIST_EXTEND_RATIO_NUMERATOR,
                 WORD_LIST_EXTEND_RATIO_DENOMINATOR);
}

/**@}*/

/** @name Elementy interfejsu 
   @{
 */

void word_list_init(struct word_list *list)
{
    list->array_size = 0;
    list->array_capacity = WORD_LIST_DEFAULT_ARRAY_CAPACITY;
    list->array = calloc(list->array_capacity, sizeof(wchar_t *));

    list->buffer_size = 0;
    list->buffer_capacity = WORD_LIST_DEFAULT_BUFFER_CAPACITY;
    list->buffer = calloc(list->buffer_capacity, sizeof(wchar_t));
}

void word_list_init_from_s(struct word_list *list, struct word_list_s *src)
{
    size_t i;

    list->array_size = 0; 
    list->array_capacity = src->array_size;
    list->array = calloc(list->array_capacity, sizeof(wchar_t *));

    list->buffer_size = 0; 
    list->buffer_capacity = src->buffer_size;
    list->buffer = calloc(list->buffer_capacity, sizeof(wchar_t));

    for (i = 0; i < src->array_size; ++i)
        word_list_add(list, src->array[i]);
}

void word_list_done(struct word_list *list)
{
  free(list->array);
  free(list->buffer);
}

int word_list_add(struct word_list *list, const wchar_t *word)
{
    size_t word_length = wcslen(word) + 1;
    if (word_length < 2)
        return 0;
    if (list->buffer_size + word_length > list->buffer_capacity)
        extend_buffer(list);
    if (list->array_size == list->array_capacity)
        extend_array(list);
    push_back(list, word);
    return 1;
}

int word_list_to_argz(struct word_list *list, char **argz, size_t *argz_len)
{
    resize_buffer(list, 1, 1);
    int error = 0;
    size_t i, n = list->buffer_size;
    if (n == 0)
    {
        *argz = NULL;
        *argz_len = 0;
        return 1;
    }
    char *vector = calloc(n + 1, sizeof(char));
    for (i = 0; i < n; ++i)
    {
        // ASCII test
        if ((size_t) list->buffer[i] > 126)
        {
            error = 1;
            break;
        }
        vector[i] = list->buffer[i];
    }
    if (error)
    {
        free(vector);
        return 0;
    }
    vector[n] = '\0';
    *argz = vector;
    *argz_len = n;
    return 1;
}

/**@}*/
