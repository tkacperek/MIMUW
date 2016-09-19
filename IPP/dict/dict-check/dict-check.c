/** @defgroup dict-check Moduł dict-check
    Program sprawdzający pisownię w danym tekście.
 */
/** @file
    Główny plik modułu dict-check
    @ingroup dict-check
    @author Tomasz Kacperek <tk334578@students.mimuw.edu.pl>
    @date 2015-06-08
 */

#include "dictionary.h"
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wctype.h>

/// Maksymalna długość nazwy pliku.
#define MAX_FILENAME_LENGTH 511
/// Maksymalna długość słowa.
#define MAX_WORD_LENGTH 100
/// Rozmiar wczytywanego fragmentu tekstu.
#define BATCH_SIZE 512

/**
  Obsługuje argumenty CLI.
  Jedyne poprawne argumenty to: -v, nazwa_pliku_słownika.
  @param[in] argc Liczba argumentów.
  @param[in] argv Argumenty.
  @param[in,out] show_hints Informacja, czy wypisywać podpowiedzi.
  @param[in,out] dict_filename Nazwa pliku ze słownikiem.
 */
void handle_cli_arguments(int argc, char **argv,
                          int *show_hints, char *dict_filename)
{   
    int name_found = 0;
    *show_hints = 0;
    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "-v") == 0)
            *show_hints = 1;
        else if (name_found == 0 && strlen(argv[i]) <=  MAX_FILENAME_LENGTH)
        {
            strcpy(dict_filename, argv[i]);
            name_found = 1;
        }
        else
        {
            fprintf(stderr, "Invalid argument: %s\n", argv[i]);
            exit(1);
        }
    }
    if (! name_found)
    {
        fprintf(stderr, "Missing argument: dictionary filename\n");
        exit(1);
    }
}

/**
  Wczytuje słownik z pliku.
  @param[in] dict_filename Nazwa pliku.
  @return Słownik.
 */
struct dictionary *load_dictionary(const char *dict_filename)
{
    FILE *f = fopen(dict_filename, "r");
    struct dictionary *new_dict;
    if (!f || !(new_dict = dictionary_load(f)))
    {
        fprintf(stderr, "Failed to load dictionary\n");
        exit(1);
    }
    fclose(f);
    return new_dict; 
}

/**
  Wypisuje na wyjście nie-słowa z bufora.
  @param[in] buffer Bufor.
  @param[in,out] pos Pozycja w buforze.
  @param[in,out] line Numer linii.
  @param[in,out] character Numer znaku.
  @return 0 gdy koniec bufora, 1 w p.p.
 */
int filter_trash(const wchar_t *buffer, size_t *pos,
                 size_t *line, size_t *character)
{
    while (buffer[*pos] != L'\0' && ! iswalpha(buffer[*pos]))
    {
        if (buffer[*pos] == L'\n')
        {
            ++(*line);
            (*character) = 1;
        }
        else
            ++(*character);
        putwchar(buffer[(*pos)++]);
    }
    if (buffer[*pos] == L'\0')
        return 0;
    else
        return 1;
}

/**
  Wypisuje podpowiedzi.
  @param[in] dict Słownik.
  @param[in,out] line Numer linii.
  @param[in,out] character Numer znaku.
  @param[in] word Słowo.
 */
static
void hints(const struct dictionary *dict, size_t *line,
           size_t *character, const wchar_t *word)
{
    fprintf(stderr, "%ld,%ld %ls:", *line, *character, word);
    struct word_list list;
    dictionary_hints(dict, word, &list);
    const wchar_t * const *a = word_list_get(&list);
    for (size_t i = 0; i < word_list_size(&list); ++i)
        fprintf(stderr, " %ls", a[i]);
    if (word_list_size(&list) == 0)
        fprintf(stderr, " ");
    fprintf(stderr, "\n");
    word_list_done(&list); 
}

/**
  Wypisuje na wyjście słowo z bufora i ewentualnie podpowiedzi.
  @param[in] dict Słownik.
  @param[in,out] buffer Bufor.
  @param[in,out] pos Pozycja w buforze.
  @param[in,out] buff_offset Przesunięcie w buforze.
  @param[in] show_hints Czy wypisać podpowiedzi.
  @param[in,out] line Numer linii.
  @param[in,out] character Numer znaku.
 */
void process_word(const struct dictionary *dict, wchar_t *buffer,
                  size_t *pos, size_t *buff_offset, int show_hints,
                  size_t *line, size_t *character)
{
    // Sprawdź długość.
    size_t i, n, tmp_pos = *pos;
    while (iswalpha(buffer[tmp_pos]))
        ++tmp_pos;
    n = tmp_pos - *pos;
    // Jeżeli koniec bufora, to przesuń na początek.
    if (buffer[tmp_pos] == L'\0')
    {
        for (i = 0; i < n + 1; ++i)
            buffer[i] = buffer[i + *pos];
        *buff_offset = n; 
        *pos = n;
    }
    // W p.p. przetwórz.
    else
    {
        wchar_t *word = calloc(n + 1, sizeof(wchar_t));
        wcsncpy(word, buffer + *pos, n);
        word[n] = L'\0';
        if (! dictionary_find(dict, word))
        {
            putwchar(L'#');
            if (show_hints)
                hints(dict, line, character, word);
        }
        fputws(word, stdout);
        *character += n;
        free(word);
        *pos = tmp_pos;
    }
}

/**
  Funkcja main.
  @param[in] argc Liczba argumentów.
  @param[in] argv Argumenty. Jedyne poprawne to: -v, plik_słownika.
  @return Kod wyjścia.
 */
int main(int argc, char **argv)
{
    setlocale(LC_ALL, "pl_PL.UTF-8");
    int show_hints;
    char dict_filename[MAX_FILENAME_LENGTH + 1];
    handle_cli_arguments(argc, argv, &show_hints, dict_filename);
    struct dictionary *dict = load_dictionary(dict_filename);
    wchar_t buffer[BATCH_SIZE];
    size_t line = 1, character = 1, pos = 0, buff_offset = 0;
    while (fgetws(buffer + buff_offset, BATCH_SIZE - buff_offset, stdin))
    {
        buff_offset = pos = 0;
        while (filter_trash(buffer, &pos, &line, &character))
        {
            process_word(dict, buffer, &pos, &buff_offset, show_hints, &line, &character);
        }
    }
    dictionary_done(dict);
    return 0; 
}
