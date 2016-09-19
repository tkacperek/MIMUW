/** @file
  Implementacja struktury reguły.

  @ingroup dictionary
  @author Tomasz Kacperek <tk334578@students.mimuw.edu.pl>
  @copyright Uniwerstet Warszawski
  @date 2015-06-21
 */

#include "rule.h"
#include <wchar.h>
#include <wctype.h>

struct rule *rule_new(const wchar_t *left, const wchar_t *right,
                      int cost, enum rule_flag flag)
{
    struct rule *r = malloc(sizeof(struct rule));
    r->cost = cost;
    r->flag = flag;
    r->llen = wcslen(left);
    r->rlen = wcslen(right);
    r->left = calloc(r->llen + 1, sizeof(wchar_t));
    r->right = calloc(r->rlen + 1, sizeof(wchar_t));
    wcscpy(r->left, left);
    wcscpy(r->right, right);
    return r;
}

void rule_done(struct rule *r)
{
    free(r->left);
    free(r->right);
    free(r);
}

int rule_serialize(const struct rule *r, FILE *s)
{
    wchar_t *lptr, *rptr, empty[2] = {L'!', (wchar_t)0};
    if (r->llen == 0)
        lptr = empty;
    else
        lptr = r->left;
    if (r->rlen == 0)
        rptr = empty;
    else
        rptr = r->right;
    if (fprintf(s, "[%ld %ld %ls %ls %d %d]", 
                r->llen, r->rlen, 
                lptr, rptr, r->cost, r->flag) >= 0)
        return 1;
    else
        return -1;
}

struct rule *rule_deserialize(FILE *s)
{
    struct rule *rule = NULL;
    size_t l, r;
    wchar_t *left, *right, *lptr, *rptr, fake[2];
    int cost, go = 1;
    enum rule_flag flag;
    if (fscanf(s, "%*c%lu %lu", &l, &r) < 0)
        return NULL;
    left = calloc(l + 1, sizeof(wchar_t));
    right = calloc(r + 1, sizeof(wchar_t));

    if (l == 0)
    {
        left[0] = (wchar_t)0;
        lptr = fake;
    }
    else
        lptr = left;

    if (r == 0)
    {
        right[0] = (wchar_t)0;
        rptr = fake;
    }
    else
        rptr = right;

    if (fscanf(s, "%ls %ls %d %u%*c", lptr, rptr, &cost, &flag) < 0)
        go = 0;
    if (go)
        rule = rule_new(left, right, cost, flag);
    free(left);
    free(right);
    return rule;
}

int rule_match(const struct rule *r, const wchar_t *str)
{
    size_t i, rn, sn;
    rn = r->llen;
    sn = wcslen(str);
    if (sn < rn)
        return 0;

    int ok = 1;
    wchar_t vars[10];
    for (i = 0; i < 10; ++i)
        vars[i] = (wchar_t)0;
    for (i = 0; i < rn; ++i)
    {
        if (iswdigit(r->left[i]))
        {
            if (vars[(size_t)(r->left[i]) - 48] == (wchar_t)0)
            {
                vars[(size_t)(r->left[i]) - 48] = str[i];
            }
            else if (str[i] != vars[(size_t)(r->left[i]) - 48])
            {
               ok = 0;
               break;
            }
        }
        else if (r->left[i] != str[i])
        {
            ok = 0;
            break;
        }
    }
    return ok;
}

