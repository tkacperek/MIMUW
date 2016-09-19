/** @file
    Interfejs struktury stanu.

    @ingroup dictionary
    @author Tomasz Kacperek <tk334578@students.mimuw.edu.pl>
    @date 2015-06-22
 */

#ifndef __STATE_H__
#define __STATE_H__

#include "trie_node.h"

/// Struktura przechowująca stan.
struct state
{
    /// Aktualny sufiks.
    wchar_t *suffix;
    /// Aktualny węzeł.
    struct trie_node *node;
    /// Węzeł poprzedniego słowa (może być NULL).
    struct trie_node *prev;
    /// Koszt dojścia do stanu.
    size_t cost;
    /// Czy stan jest nierozwijalny.
    int final;
    /// Czy stan końcowy.
    int end;
    /// Czy ignorować przy zliczaniu wyników.
    int ignore;
};

/**
  Tworzy nowy stan.
  @param[in] s Sufiks.
  @param[in] n Węzeł.
  @param[in] p Węzeł poprzedniego słowa.
  @param[in] c Koszt.
  @param[in] f Czy jest ostateczny (nierozwijalny).
  @return Nowy stan.
 */
struct state *state_new(const wchar_t *s, struct trie_node *n, 
        struct trie_node *p, size_t c, int f);

/**
  Tworzy kopię stanu.
  @param[in] s Kopiowany stan.
  @return Kopia stanu.
 */
struct state *state_copy(const struct state *s);

/**
  Niszczy stan.
  @param[in,out] s Stan.
 */
void state_done(struct state *s);

/**
  Porównuje dwa stany bez uwzględnienia kosztu i rozwijalności.
  Kryteria: suffix, node, prev.
  @param[in] s1 Stan.
  @param[in] s2 Stan.
  @return 0 gdy równe, <0 gdy s1<s2, >0 w p.p.
 */
int state_compare_abs(struct state *s1, struct state *s2);

/**
  Porównuje dwa stany. 
  Kryteria: suffix, node, prev, cost.
  @param[in] s1 Stan.
  @param[in] s2 Stan.
  @return 0 gdy równe, <0 gdy s1<s2, >0 w p.p.
 */
int state_compare(struct state *s1, struct state *s2);

/**
  Wyłuskuje słow(o/a) ze stanu.
  Zwrócone słowo należy zwolnić (free).
  @param[in] s Stan.
  @return Słowo.
 */
wchar_t *state_extract(struct state *s);

#endif /* __STATE_H__ */
