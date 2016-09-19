/** @file
    Interfejs struktury reguły.

    @ingroup dictionary
    @author Tomasz Kacperek <tk334578@students.mimuw.edu.pl>
    @date 2015-06-21
 */

#ifndef __RULE_H__
#define __RULE_H__

#include "dictionary.h"

/// Struktura przechowująca regułę.
struct rule
{
    /// Lewa strona.
    wchar_t *left;
    /// Prawa strona.
    wchar_t *right;
    /// Długość lewej strony.
    size_t llen;
    /// Długość prawej strony.
    size_t rlen;
    /// Koszt.
    int cost;
    /// Flaga.
    enum rule_flag flag;
};

/**
  Tworzy nową regułę.
  @param[in] left Lewa strona.
  @param[in] right Prawa strona.
  @param[in] cost Koszt.
  @param[in] flag Flaga.
  @return Nowa reguła lub NULL.
 */
struct rule *rule_new(const wchar_t *left, const wchar_t *right,
                      int cost, enum rule_flag flag);

/**
  Niszczy regułę.
  @param[in,out] r Reguła.
 */
void rule_done(struct rule *r);

/**
  Serializuje regułę.
  @param[in] r Reguła.
  @param[out] s Strumień do zapisu.
  @return <0 jeśli operacja się nie powiedzie, 0 w p.p.
 */
int rule_serialize(const struct rule *r, FILE *s);

/**
  Odtwarza regułę.
  @param[in] s Strumień, z którego będzie odczytana reguła.
  @return Nowa reguła z odpowiednią zawartością lub NULL przy niepowodzeniu.
 */
struct rule *rule_deserialize(FILE *s);

/**
  Sprawdza, czy lewa strona reguły pasuje do podanego łańcucha.
  @param[in] r Reguła.
  @param[in] str Łańcuch.
  @return 1 jeżeli pasuje, 0 w p.p.
 */
int rule_match(const struct rule *r, const wchar_t *str);

#endif /* __RULE_H__ */
