/** @file
  Implementacja posortowanej listy słów bez powtórzeń.

  @ingroup dictionary
  @author Jakub Pawlewicz <pan@mimuw.edu.pl>
  @author Tomasz Kacperek <tk334578@students.mimuw.edu.pl>
  @copyright Uniwerstet Warszawski
  @date 2015-06-06
 */

#include "word_list_s.h"

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
int push_back(struct word_list_s *list, const wchar_t *word)
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
int resize_array(struct word_list_s *list,
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
int resize_buffer(struct word_list_s *list,
                  const size_t ratio_numerator, const size_t ratio_denominator)
{
    size_t i, n, new_buffer_capacity =
        (list->buffer_capacity * ratio_numerator) / ratio_denominator;
    if (list->buffer_size - list->removed_size > new_buffer_capacity)
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
    list->removed_size = 0;
    return 1;
}

/**
  Zmniejsza rozmiar tablicy.
  @param[in,out] list Lista słów.
  @return 1 jeśli się udało, 0 w p.p.
 */
static
int shrink_array(struct word_list_s *list)
{
    return resize_array(list,
                 WORD_LIST_S_SHRINK_RATIO_NUMERATOR,
                 WORD_LIST_S_SHRINK_RATIO_DENOMINATOR);
}

/**
  Zwiększa rozmiar tablicy.
  @param[in,out] list Lista słów.
  @return 1 jeśli się udało, 0 w p.p.
 */
static
int extend_array(struct word_list_s *list)
{
    return resize_array(list,
                 WORD_LIST_S_EXTEND_RATIO_NUMERATOR,
                 WORD_LIST_S_EXTEND_RATIO_DENOMINATOR);
}

/**
  Zmniejsza rozmiar bufora.
  @param[in,out] list Lista słów.
  @return 1 jeśli się udało, 0 w p.p.
 */
static
int shrink_buffer(struct word_list_s *list)
{
    return resize_buffer(list,
                 WORD_LIST_S_SHRINK_RATIO_NUMERATOR,
                 WORD_LIST_S_SHRINK_RATIO_DENOMINATOR);
}

/**
  Zwiększa rozmiar bufora.
  @param[in,out] list Lista słów.
  @return 1 jeśli się udało, 0 w p.p.
 */
static
int extend_buffer(struct word_list_s *list)
{
    return resize_buffer(list,
                 WORD_LIST_S_EXTEND_RATIO_NUMERATOR,
                 WORD_LIST_S_EXTEND_RATIO_DENOMINATOR);
}

/**
  Usuwa ostanie słowo na liście.
  @param[in,out] list Lista słów.
  @return 1 jeśli się udało, 0 w p.p.
 */
static
int remove_last(struct word_list_s *list)
{
    if (list->array_size == 0)
        return 0;
    list->buffer_size -= wcslen(list->array[list->array_size - 1]) + 1;
    list->array_size--;
    return 1;
}


/**
  Wsortowuje ostanie słowo.
  @param[in,out] list Lista słów.
 */
static
void sort_last(struct word_list_s *list)
{
    if (list->array_size < 2)
        return;

    int cmp;
    size_t i, left_bound, right_bound, mid;
    const wchar_t * last_word = list->array[list->array_size - 1];
    left_bound = 0;
    right_bound = list->array_size - 2;
    if (wcscoll(last_word, list->array[left_bound]) < 0)
        right_bound = left_bound;
    else if (wcscoll(last_word, list->array[right_bound]) > 0)
        left_bound = right_bound;

    // now if right-left > 0 then last is between left and right 
    while (right_bound - left_bound > 0)
    {
        mid = (right_bound + left_bound) / 2;
        cmp = wcscoll(last_word, list->array[mid]);
        if (cmp == 0)
        {
            remove_last(list);
            return;
        }
        else if (cmp < 0)
            right_bound = (mid > left_bound) ? mid - 1 : left_bound;
        else
            left_bound = (mid < right_bound) ? mid + 1 : right_bound;
    }

    // now right-left = 0
    cmp = wcscoll(last_word, list->array[left_bound]);
    if (cmp == 0)
    {
        remove_last(list);
        return;
    }
    if (cmp > 0)
        ++left_bound;
    // insert
    for (i = list->array_size - 1; i > left_bound; --i)
        list->array[i] = list->array[i - 1];
    list->array[left_bound] = last_word;
}

/**
  Porządkuje bufor po usuniętych słowach.
  @param[in,out] list Lista słów.
  @return 1 jeśli się udało, 0 w p.p.
 */
static
int clean_removed(struct word_list_s *list)
{
    return resize_buffer(list, 1, 1);
}

/**@}*/

/** @name Elementy interfejsu 
   @{
 */

void word_list_s_init(struct word_list_s *list)
{
    list->array_size = 0;
    list->array_capacity = WORD_LIST_S_DEFAULT_ARRAY_CAPACITY;
    list->array = calloc(list->array_capacity, sizeof(wchar_t *));

    list->buffer_size = 0;
    list->buffer_capacity = WORD_LIST_S_DEFAULT_BUFFER_CAPACITY;
    list->buffer = calloc(list->buffer_capacity, sizeof(wchar_t));
    list->removed_size = 0;
}

void word_list_s_done(struct word_list_s *list)
{
  free(list->array);
  free(list->buffer);
}

int word_list_s_add(struct word_list_s *list, const wchar_t *word)
{
    size_t word_length = wcslen(word) + 1;
    if (word_length < 2)
        return 0;
    if (list->buffer_size + word_length > list->buffer_capacity)
    {
        if (list->removed_size >= word_length)
            clean_removed(list);
        else
            extend_buffer(list);
    }
    if (list->array_size == list->array_capacity)
        extend_array(list);
    push_back(list, word);
    sort_last(list);
    return 1;
}

int word_list_s_remove(struct word_list_s *list, const size_t index)
{
    if (list->array_size <= index)
        return 0;
    list->removed_size += wcslen(list->array[index]) + 1;
    for (size_t i = index; i < list->array_size - 1; ++i)
        list->array[i] = list->array[i + 1];
    list->array_size -= 1;
    shrink_buffer(list);
    shrink_array(list);
    return 1;
}

int word_list_s_to_argz(struct word_list_s *list, char **argz, size_t *argz_len)
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
