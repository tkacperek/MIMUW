/** @file
    Implementacja tablicy stanów.

    @ingroup dictionary
    @author Tomasz Kacperek <tk334578@students.mimuw.edu.pl>
    @date 2015-06-21
 */

#include "state_array.h"
#include "state.h"
#include <assert.h>

/** @name Funkcje pomocnicze
  @{
 */

/**
  Dodaje stan na końcu tablicy.
  Zakłada, że stan zmieści się w tablicy.
  @param[in,out] array Tablica stanów.
  @param[in] node Dodawana stan.
  @return 1 jeśli się udało, 0 w p.p.
 */
static
int push_back(struct state_array *array,
              struct state *node)
{
    array->array[array->size++] = node;
    return 1;
}

/**
  Zmienia pojemność tablicy stanów.
  @param[in,out] array Tablica stanów.
  @param[in] ratio_numerator Licznik współczynnika zmiany rozmiaru.
  @param[in] ratio_denominator Mianownik współczynnika zminay rozmiaru.
  @return 1 jeśli się udało, 0 w p.p.
 */
static
int resize_array(struct state_array *array,
                 const size_t ratio_numerator, const size_t ratio_denominator)
{
    size_t i, new_array_capacity =
        (array->capacity * ratio_numerator) / ratio_denominator;
    if (new_array_capacity == array->capacity)
        ++new_array_capacity;
    if (array->size > new_array_capacity)
        return 0;
    struct state **new_array = calloc(new_array_capacity, sizeof(struct state *));
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
  Zwiększa rozmiar tablicy.
  @param[in,out] array Tablica stanów. 
  @return 1 jeśli się udało, 0 w p.p.
 */
static
int extend_array(struct state_array *array)
{
    return resize_array(array,
                 STATE_ARRAY_EXTEND_RATIO_NUMERATOR,
                 STATE_ARRAY_EXTEND_RATIO_DENOMINATOR);
}

/**
  Łączy 2 posortowane bloki tablicy w 1 posortowany blok.
  @param[in] src Tablica z 2 blokami.
  @param[out] dst Tablica, do której będzie wpisany wynik.
  @param[in] left Indeks początku pierwszego bloku.
  @param[in] mid Indeks ostatniego elementu pierwszego bloku.
  @param[in] right Indeks ostatniego elementu drugiego bloku.
 */
static
void merge(struct state **src, struct state **dst, size_t left, size_t mid, size_t right)
{
    int diff;
    size_t i, left_head, right_head;
    left_head = left;
    right_head = mid + 1;
    for (i = left; i <= right; ++i)
    {
        if (left_head > mid)
            dst[i] = src[right_head++];
        else if (right_head > right)
            dst[i] = src[left_head++];
        else
        {
            diff = state_compare(src[left_head], src[right_head]);
            if (diff <= 0)
                dst[i] = src[left_head++];
            else
                dst[i] = src[right_head++];
        }
    }
}

/**
  Zwraca mniejszą liczbę.
  @param[in] lhs Lewa liczba.
  @param[in] rhs Prawa liczba.
  @return Mniejsza liczba.
 */
static
inline
size_t min(size_t lhs, size_t rhs)
{
    return (lhs <= rhs) ? lhs : rhs;
}

/**@}*/

/** @name Elementy interfejsu
   @{
 */

struct state_array *state_array_new_n(size_t n)
{
    if (n == 0)
        n = 1;
    struct state_array *array = malloc(sizeof(struct state_array));
    array->size = 0;
    array->capacity = n;
    array->array = calloc(array->capacity, sizeof(struct state *));
    return array;
}

struct state_array *state_array_new()
{
    return state_array_new_n(STATE_ARRAY_DEFAULT_CAPACITY);
}

void state_array_done(struct state_array *array)
{
    free(array->array);
    free(array);
}

void state_array_kill(struct state_array *array)
{
    struct state **r = state_array_get(array);
    for (size_t i = 0; i < array->size; ++i)
        state_done(r[i]);
    state_array_done(array);
}

int state_array_add(struct state_array *array, struct state *node)
{
    assert(node != NULL);
    if (array->size == array->capacity)
        extend_array(array);
    push_back(array, node);
    return 1;
}

int state_array_add_copy(struct state_array *array, struct state *node)
{
    state_array_add(array, state_copy(node));
    return 1;
}

void state_array_sort(struct state_array *array)
{
    size_t i, width, n = array->size;
    struct state **A, **B, **swap;
    A = array->array;
    B = calloc(array->capacity, sizeof(struct state *));
    // width - szerokość połówki do złączenia
    for (width = 1; width < n; width *= 2)
    {
        // i - początek bloku (2 połówek)
        for (i = 0; i < n; i += 2 * width)
        {
           merge(A, B, i, i + width - 1, min(i + 2 * width - 1, n - 1)); 
        }
        swap = A;
        A = B;
        B = swap;
    }
    free(B);
    array->array = A;
}

/**@}*/
