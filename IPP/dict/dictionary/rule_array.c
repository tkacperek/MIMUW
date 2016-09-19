/** @file
    Implementacja tablicy reguł.

    @ingroup dictionary
    @author Tomasz Kacperek <tk334578@students.mimuw.edu.pl>
    @date 2015-06-21
 */

#include "rule_array.h"
#include "rule.h"

/** @name Funkcje pomocnicze
  @{
 */

/**
  Dodaje regułę na końcu tablicy.
  Zakłada, że reguła zmieści się w tablicy.
  @param[in,out] array Tablica reguł.
  @param[in] node Dodawana reguła.
  @return 1 jeśli się udało, 0 w p.p.
 */
static
int push_back(struct rule_array *array,
              struct rule *node)
{
    array->array[array->size++] = node;
    return 1;
}

/**
  Zmienia pojemność tablicy reguł.
  @param[in,out] array Tablica reguł.
  @param[in] ratio_numerator Licznik współczynnika zmiany rozmiaru.
  @param[in] ratio_denominator Mianownik współczynnika zminay rozmiaru.
  @return 1 jeśli się udało, 0 w p.p.
 */
static
int resize_array(struct rule_array *array,
                 const size_t ratio_numerator, const size_t ratio_denominator)
{
    size_t i, new_array_capacity =
        (array->capacity * ratio_numerator) / ratio_denominator;
    if (new_array_capacity == array->capacity)
        ++new_array_capacity;
    if (array->size > new_array_capacity)
        return 0;
    struct rule **new_array = calloc(new_array_capacity, sizeof(struct rule *));
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
  @param[in,out] array Tablica reguł. 
  @return 1 jeśli się udało, 0 w p.p.
 */
static
int extend_array(struct rule_array *array)
{
    return resize_array(array,
                 RULE_ARRAY_EXTEND_RATIO_NUMERATOR,
                 RULE_ARRAY_EXTEND_RATIO_DENOMINATOR);
}

/**
  Komparator reguł.
  @param[in] lhs Lewa reguła.
  @param[in] rhs Prawa reguła.
  @return 0 - równe, <0 lhs<rhs, >0 w p.p.
 */
static
int rule_compare(struct rule *lhs, struct rule *rhs)
{
    return lhs->cost - rhs->cost;
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
void merge(struct rule **src, struct rule **dst, size_t left, size_t mid, size_t right)
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
            diff = rule_compare(src[left_head], src[right_head]);
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

struct rule_array *rule_array_new_n(size_t n)
{
    if (n == 0)
        n = 1;
    struct rule_array *array = malloc(sizeof(struct rule_array));
    array->size = 0;
    array->capacity = n;
    array->array = calloc(array->capacity, sizeof(struct rule *));
    array->sorted = 1;
    return array;
}

struct rule_array *rule_array_new()
{
    return rule_array_new_n(RULE_ARRAY_DEFAULT_CAPACITY);
}

void rule_array_done(struct rule_array *array)
{
    free(array->array);
    free(array);
}

void rule_array_kill(struct rule_array *array)
{
    struct rule **r = rule_array_get(array);
    for (size_t i = 0; i < array->size; ++i)
        rule_done(r[i]);
    rule_array_done(array);
}

int rule_array_add(struct rule_array *array, struct rule *node)
{
    if (array->size == array->capacity)
        extend_array(array);
    push_back(array, node);
    array->sorted = 0;
    return 1;
}

int rule_array_serialize(struct rule_array *a, FILE *s)
{
    if (!(a->sorted))
    {
        rule_array_sort(a);
        a->sorted = 1;
    }
    int ret = fprintf(s, "[%ld", a->size);
    if (ret >= 0)
    {
        for (size_t i = 0; i < a->size; ++i)
        {
            ret = rule_serialize(a->array[i], s);
            if (ret < 0)
                break;
        }
    }
    if (ret >= 0)
    ret = fprintf(s, "]");
    if (ret >= 0)
        return 0;
    else
        return -1;
}

struct rule_array *rule_array_deserialize(FILE *s)
{
    int go = 1;
    size_t n;
    if (fscanf(s, "%*c%lu", &n) < 0)
        return NULL;
    struct rule_array *ra = rule_array_new_n(n);
    for (size_t i = 0; i < n; ++i)
    {
        struct rule *r = rule_deserialize(s);
        if (!r)
        {
            go = 0;
            break;
        }
        rule_array_add(ra, r);
    }
    if (!go)
    {
        rule_array_kill(ra);
        ra = NULL;
    }
    else
        fscanf(s, "%*c");
    if (ra)
        ra->sorted = 1;
    return ra;
}

void rule_array_sort(struct rule_array *ra)
{
    if (ra->sorted)
        return;
    size_t i, width, n = ra->size;
    struct rule **A, **B, **swap;
    A = ra->array;
    B = calloc(ra->capacity, sizeof(struct rule *));
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
    ra->array = A;
    ra->sorted = 1;
}

/**@}*/
