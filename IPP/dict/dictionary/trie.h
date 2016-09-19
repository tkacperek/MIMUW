/** @file
    Interfejs drzewa trie.

    @ingroup dictionary
    @author Tomasz Kacperek <tk334578@students.mimuw.edu.pl>
    @date 2015-06-07
 */

#ifndef __TRIE_H__
#define __TRIE_H__

#include "trie_node.h"
#include "word_list_s.h"

/// Struktura reprezentująca drzewo trie.
struct trie
{
    /// Korzeń drzewa.
    struct trie_node *root;
    /// Liczba węzłów w drzewie.
    size_t size;
    /// Lista znaków występujących w drzewie.
    struct word_list_s characters;
};

/**
  Tworzy nowe puste drzewo.
  @return Drzewo.
 */
struct trie *trie_new();

/**
  Niszczy drzewo i jego zawartość.
  @param[in,out] trie Drzewo.
 */
void trie_kill(struct trie *trie);

/**
  Dodaje słowo do drzewa.
  @param[in,out] trie Drzewo.
  @param[in] word Słowo.
  @return 0 jeśli słowo było już w drzewie, 1 jeśli udało się wstawić.
 */
int trie_insert(struct trie *trie, const wchar_t* word);

/**
  Usuwa słowo ze słownika.
  @param[in,out] trie Drzewo.
  @param[in] word Słowo.
  @return 1 jeśli udało się usunąć, 0 w p.p.
 */
int trie_delete(struct trie *trie, const wchar_t* word);

/**
  Sprawdza, czy słowo znajduje się w drzewie.
  @param[in] trie Drzewo.
  @param[in] word Słowo.
  @return 1 jeżeli znaleziono, 0 w p.p.
 */
int trie_find(const struct trie *trie, const wchar_t* word);

/**
  Serializuje drzewo.
  @param[in] trie Drzewo.
  @param[out] stream Plik, do którego będzie zapisywane drzewo.
  @return -1 przy niepowodzeniu, 0 w p.p.
 */
int trie_serialize(const struct trie *trie, FILE *stream);

/**
  Odtwarza drzewo na podstawie zapisu w łańcuchu.
  @param[in] str Łańcuch z zserializowanym drzewem.
  @return Nowe drzewo z odpowiednią zawartością lub NULL przy niepowodzeniu.
 */
struct trie *trie_deserialize(const wchar_t *str);

/**
  Zwraca listę znaków występujących w drzewie.
  @param[in] trie Drzewo.
  @return Lista znaków.
 */
static inline
const struct word_list_s *trie_get_characters(const struct trie *trie)
{
    return &trie->characters;
}

#endif /* __TRIE_H__ */
