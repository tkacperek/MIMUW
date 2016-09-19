/** @file
  Implementacja słownika.

  @ingroup dictionary
  @author Jakub Pawlewicz <pan@mimuw.edu.pl>
  @author Tomasz Kacperek <tk334578@students.mimuw.edu.pl>
  @copyright Uniwerstet Warszawski
  @date 2015-06-08
 */

#include "word_list_s.h"
#include "trie.h"
#include "dictionary.h"
#include "conf.h"
#include "rule_array.h"
#include "rule.h"
#include "state_array.h"
#include "state.h"
#include <stdio.h>
#include <wctype.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

/// Ścieżka pliku z listą słowników.
#define CONF_FILE CONF_PATH "/dictlist"

/// maksymalna długość nazwy słownika
#define MAX_NAME_LENGTH 255

/// Struktura przechowująca słownik.
struct dictionary
{
    /// Drzewo słów.
    struct trie *trie;
    /// Maksymalny koszt dla podpowiedzi.
    int max_cost;
    /// Reguły dotyczące słownika.
    struct rule_array *rules;
};

/** @name Funkcje pomocnicze
  @{
 */

/**
  Sprowadza słowo do poprawnej postaci, jeżeli się da.
  Poprawna postać - małe litery w aktualnym locale.
  @param[in] in Słowo do przetworzenia.
  @return Wynik parsowania lub NULL przy niepowodzeniu. 
 */
static
wchar_t *parse_word(const wchar_t *in)
{
    int error = 0;
    size_t n = wcslen(in);
    wchar_t *str = calloc(n + 1, sizeof(wchar_t));
    for (size_t i = 0; i < n; ++i)
    {
        if (!iswalpha(in[i]))
        {
            error = 1;
            break;
        }
        else
            str[i] = towlower(in[i]);
    }
    str[n] = L'\0';
    if (error)
    {
        free(str);
        return NULL;
    }
    return str;
}

/**
  Zlicza szerokie znaki w pierwszej linii pliku.
  @param[in] stream Strumień pliku.
  @return Liczba szerokich znaków.
 */
static
size_t file_wc_count(FILE *stream)
{
    rewind(stream);
    size_t count = 0;
    wchar_t c = (wchar_t)0;
    while (fscanf(stream, "%lc", &c) != EOF && (size_t)c != 10)
        ++count;
    rewind(stream);
    return count;
}

/**
  Porządkuje katalog konfiguracyjny.
  1. Jeśli katalog nie istnieje - utwórz.
  2. Jeżeli nie ma pliku dictlist - utwórz.
  @return 0 gdy wystąpił problem, 1 w p.p.
 */
static
int directory_cleanup()
{
    int ret = 1;
    mkdir(CONF_PATH, S_IRWXU);

    FILE *f = fopen(CONF_FILE, "a");
    if(!f) {
        perror("File operation failed\n");
        ret = 0;
    }
    fclose(f);
    return ret;
}

/**
  Wczytuje listę słowników z pliku konfiguracyjnego.
  @param[out] wlist Lista nazw.
  @return 1 jeżeli się udało, 0 w p.p.
 */
static
int read_dict_list(struct word_list_s *wlist)
{
    int error = 0;
    word_list_s_init(wlist);
    FILE *f = fopen(CONF_FILE, "r");
    char buf[MAX_NAME_LENGTH + 2];
    while (fgets(buf, MAX_NAME_LENGTH + 2, f))
    {
        size_t n = strlen(buf);
        for (size_t i = 0; i < n - 1; ++i)
            if (! isgraph(buf[i]))
            {
                error = 1;
                break;
            }
        if (error)
            break;
        if (buf[n - 1] == '\n')
            buf[n - 1] = '\0';
        wchar_t wide_buff[MAX_NAME_LENGTH + 2];
        for (size_t i = 0; i < MAX_NAME_LENGTH + 2; ++i)
            wide_buff[i] = (wchar_t) buf[i];
        word_list_s_add(wlist, wide_buff);
    }
    if (ferror(f))
    {
        fclose(f);
        return 0;
    }
    fclose(f);
    if (error)
    {
        remove(CONF_FILE);
        directory_cleanup();
        word_list_s_done(wlist);
        word_list_s_init(wlist);
        return 0;
    }
    return 1;
}

/**
  Tworzy tablicę reguł dla sufiksów słowa.
  Tablica jest indeksowana długościami sufiksów.
  Reguły są posortowane rosnąco wg. kosztów.
  @param[in,out] dict Słownik.
  @param[in] str Słowo.
  @param[out] sr_size Rozmiar tablicy.
  @return Utworzona tablica.
 */
static
struct rule_array **get_suffix_rules(const struct dictionary *dict, 
        const wchar_t *str, size_t *sr_size)
{
    size_t i, j, n;
    struct rule_array **ra;
    struct rule **r;

    rule_array_sort(dict->rules);

    n = rule_array_size(dict->rules);
    r = rule_array_get(dict->rules);

    *sr_size = wcslen(str) + 1;
    ra = calloc(*sr_size, sizeof(struct rule_array *)); 
    for (i = 0; i < *sr_size; ++i)
        ra[i] = rule_array_new();

    for (i = 0; i < *sr_size; ++i)
        for (j = 0; j < n; ++j)
        {
            if (r[j]->flag == RULE_BEGIN && i != 0)
                continue;
            else if (r[j]->flag == RULE_END && r[j]->llen != *sr_size - 1 - i)
                continue;
            else if (rule_match(r[j], str + i))
                rule_array_add(ra[*sr_size - 1 - i], r[j]);
        }
    return ra;
}

/**
  Rozwija stan i wpisuje otrzymany zbiór stanów to podanej tablicy.
  @param[in] dict Słownik.
  @param[in] s Rozwijany stan.
  @param[in,out] dest Gdzie wpisać wynik.
 */
static
void extend_state(const struct dictionary *dict, struct state *s,
                  struct state_array *dest)
{
    struct state *current;
    struct trie_node *ch;
    size_t n = wcslen(s->suffix);

    current = s;
    state_array_add(dest, current);
    for (size_t i = 0; i < n; ++i)
    {
        ch = trie_node_get_child(current->node, current->suffix[0]);
        if (ch)
        {
            current = state_new(current->suffix + 1, ch, current->prev, 
                        current->cost, current->final);
            state_array_add(dest, current);
        }
        else
            break;
    }
}

/**
  Stosuje regułę do stanu.
  Zapisuje rozwinięte wyniki.
  @param[in] dict Słownik.
  @param[in] s Rozwijany stan.
  @param[in] r Reguła.
  @param[out] out Tablica stanów, do której zostaną wpisane wyniki. 
 */
static
void apply_rule(const struct dictionary *dict, const struct state *s,
        const struct rule *r, struct state_array *out)
{
    if (r->flag == RULE_BEGIN)
    {
        if (s->prev != NULL || s->node != dict->trie->root 
                || s->cost != 0)
            return;
    }
    if (r->flag == RULE_END && s->final)
        return;
    if (r->flag == RULE_SPLIT)
    {
        // Czy był split.
        if (s->prev != NULL)
            return;
        // Czy początek.
        if (s->node == dict->trie->root && r->rlen == 0)
            return;
    }
    long i, j;
    // Odkodowuje zmienne.
    wchar_t vars[10];
    for (i = 0; i < 10; ++i)
        vars[i] = (wchar_t)(1);
    for (i = 0; i < r->rlen; ++i)
        if (iswdigit(r->left[i]))
                vars[(size_t)(r->left[i]) - 48] = s->suffix[i];
    // Wstępnie przetwarza prawą stronę.
    long wildcard = -1;
    wchar_t tmp[r->rlen + 1];
    wcscpy(tmp, r->right);
    for (i = r->rlen - 1; i >= 0; --i)
        if (iswdigit(tmp[i]))
        {
            tmp[i] = vars[(size_t)(tmp[i]) - 48];
            if (tmp[i] == (wchar_t)(1))
                wildcard = i;
        }
    // Sufiksy do przetworzenia.
    size_t n;
    wchar_t **suffs;
    struct trie_node *child, *current, *tmp_node;
    current = s->node;
    // Jeżeli nie było wolnej zmiennej.
    if (wildcard == -1)
    {
        n = 1;
        suffs = malloc(sizeof(wchar_t *));
        suffs[0] = calloc(r->rlen + 1, sizeof(wchar_t));
        wcscpy(suffs[0], tmp);
    }
    // Jeżeli była, to schodzi do jej pierwszego wystąpienia.
    else
    {
        for (i = 0; i < wildcard; ++i)
        {
            child = trie_node_get_child(current, tmp[i]);
            if (child)
                current = child;
            else
                // Nie ma szukanego słowa.
                return;
        }
        n = trie_node_child_count(current);
        if (n == 0)
            // Nie ma czego podstawić pod zmienną wolną.
            return;
        else
        {
            suffs = calloc(n, sizeof(wchar_t *));
            for (i = 0; i < n; ++i)
            {
                suffs[i] = calloc(r->rlen + 1 - wildcard, sizeof(wchar_t));
                wcscpy(suffs[i], tmp + wildcard);
                suffs[i][0] = trie_node_get_children(current)[i]->character;
                j = 1;
                while (suffs[i][j])
                {
                    if (suffs[i][j] == 1)
                        suffs[i][j] = suffs[i][0];
                    ++j;
                }
            }
        }
    }
    // Mam już current, suffs o rozmiarze n. Wystarczy zejść.
    // Dla każdego suffs'a.
    int ok;
    struct state *new_state;
    for (i = 0; i < n; ++i)
    {
        ok = 1;
        tmp_node = current;
        j = 0;
        while (suffs[i][j] && ok)
        {
            child = trie_node_get_child(tmp_node, suffs[i][j]);
            if (child)
                tmp_node = child;
            else
                ok = 0;
            ++j;
        }
        // Jeżeli nie udało się zejść do końca.
        if (!ok)
            continue;
        // W p.p.
        if (r->flag == RULE_SPLIT)
        {
            if (tmp_node->ends_word)
            {
                new_state = state_new(s->suffix + r->llen, dict->trie->root, 
                        tmp_node, s->cost + r->cost, s->final);
                extend_state(dict, new_state, out);
            }
        }
        else if (r->flag == RULE_END) 
        {
            if (tmp_node->ends_word)
            {
                new_state = state_new(s->suffix + r->llen, tmp_node, 
                                s->prev, s->cost + r->cost, s->final);
                state_array_add(out, new_state);
            }
        }
        else
        {
            new_state = state_new(s->suffix + r->llen, tmp_node, 
                            s->prev, s->cost + r->cost, s->final);
            extend_state(dict, new_state, out);
        }
    }

    // Zwalnianie zmiennych.
    for (i = 0; i < n; ++i)
        free(suffs[i]);
    free(suffs);
}

/**
  Usuwa z wybranej warstwy stany, które występują w niższych warstwach.
  @param[in,out] layer Tablica warstw.
  @param[in] n Numer przetwarzanej warstwy.
 */
static
void filter_layer(struct state_array **layer, size_t n)
{
    struct state *current, *candidate;
    struct state_array *new_layer = state_array_new();
    size_t i, j, size = 0;
    for (i = 0; i <= n; ++i)
        size += state_array_size(layer[i]);
    struct state_array *states = state_array_new_n(size);
    for (i = 0; i <= n; ++i)
        for (j = 0; j < state_array_size(layer[i]); ++j)
            state_array_add(states, layer[i]->array[j]);
    state_array_sort(states);

    int lower_found = 0;
    current = NULL;
    candidate = NULL; 
    for (i = 0; i < size; ++i)
        if (state_compare_abs(current, states->array[i]) != 0)
        {
            if (candidate)
            {
                if (lower_found)
                    candidate->ignore = 1;
                state_array_add_copy(new_layer, candidate);
                candidate = NULL;
                lower_found = 0;
            }
            assert(candidate == NULL && lower_found == 0);

            current = states->array[i];
            if (current->cost == n)
            {
                if (current->final)
                    state_array_add_copy(new_layer, current);
                else
                {
                    candidate = current;
                }
            }
        }
        else if (candidate)
        {
            if (states->array[i]->cost < candidate->cost)
                lower_found = 1;
        }
    if (candidate)
    {
        if (lower_found)
            candidate->ignore = 1;
        state_array_add_copy(new_layer, candidate);
    }

    state_array_kill(layer[n]);
    layer[n] = new_layer;
    state_array_done(states);
}

/**@}*/

/** @name Elementy interfejsu 
  @{
 */

struct dictionary * dictionary_new()
{
    struct dictionary *d = malloc(sizeof(struct dictionary));
    d->trie = trie_new();
    d->max_cost = 0;
    d->rules = rule_array_new();
    return d;
}

void dictionary_done(struct dictionary *dict)
{
    trie_kill(dict->trie);
    rule_array_kill(dict->rules);
    free(dict);
}

int dictionary_insert(struct dictionary *dict, const wchar_t* word)
{
    wchar_t *str;
    int ret;
    if ((str = parse_word(word)))
    {
        ret = trie_insert(dict->trie, str);
        free(str);
    }
    else
        ret = 0;
    return ret;
}

int dictionary_delete(struct dictionary *dict, const wchar_t* word)
{
    wchar_t *str;
    int ret;
    if ((str = parse_word(word)))
    {
        ret = trie_delete(dict->trie, str);
        free(str);
    }
    else
        ret = 0;
    return ret; 
}

bool dictionary_find(const struct dictionary *dict, const wchar_t* word)
{
    wchar_t *str;
    int ret;
    if ((str = parse_word(word)))
    {
        ret = trie_find(dict->trie, str);
        free(str);
    }
    else
        ret = 0;

    if (ret)
        return true;
    else
        return false;
}

int dictionary_save(const struct dictionary *dict, FILE *stream)
{
    if (trie_serialize(dict->trie, stream) < 0)
        return -1;
    if (fprintf(stream, "\n%d\n", dict->max_cost) < 0)
        return -1;
    if (rule_array_serialize(dict->rules, stream) < 0)
        return -1;
    return 0;
}

struct dictionary * dictionary_load(FILE* stream)
{
    if (! stream)
        return NULL;
    struct dictionary *ret;
    size_t size = file_wc_count(stream);
    wchar_t *str = calloc(size + 1, sizeof(wchar_t));
    if (fscanf(stream, "%ls", str) == EOF)
        ret = NULL;
    else
    {
        struct dictionary *dict;
        struct trie *trie = trie_deserialize(str);
        if (trie == NULL)
            ret = NULL;
        else
        {
            dict = dictionary_new();
            trie_kill(dict->trie);
            dict->trie = trie;
            ret = dict;
        }
    }
    if (ret)
    {
        wchar_t x;
        fscanf(stream, "%lc", &x);
        if (!x)
        {
            dictionary_done(ret);
            ret = NULL;
        }
        else
        {
            fscanf(stream, "%d%lc", &(ret->max_cost), &x);
            ret->rules = rule_array_deserialize(stream);
            if (ret->rules == NULL)
            {
                dictionary_done(ret);
                ret = NULL;
            }
        }
    }
    free(str);
    return ret;
}

void dictionary_hints(const struct dictionary *dict, const wchar_t* word,
                      struct word_list *list)
{
    // Liczniki, rozmiar sufiksowej tablicy reguł.
    long i, j, k, l;
    size_t sr_size;
    // Tablica reguł dla sufiksow.
    struct rule_array **suffix_rules;
    // Słowo, dla którego szukamy podpowiedzi.
    wchar_t *str;
    // Wskaźnik do stanu.
    struct state *tmp_state;
    // Warstwy stanów pogrupowanych wg. kosztów.
    struct state_array *layer[dict->max_cost + 1];
    struct state_array *end_states_array[dict->max_cost + 1];

    // Inicjalizuje listę podpowiedzi.
    word_list_init(list);

    // Sprawdza, czy słowo jest poprawne.
    if ((str = parse_word(word)) == NULL)
        return;
    // Inicjalizuje tablicę reguł dla sufiksów.
    suffix_rules = get_suffix_rules(dict, str, &sr_size);
    // Inicjalizuje tablicę warstw.
    for (i = 0; i <= dict->max_cost; ++i)
    {
        layer[i] = state_array_new();
        end_states_array[i] = state_array_new();
    }

    // Tworzy stan początkowy.
    tmp_state = state_new(str, dict->trie->root, NULL, 0, 0);
    size_t last_layer = dict->max_cost, end_state_count = 0;
    // Rozwija stan początkowy.
    extend_state(dict, tmp_state, layer[0]);
    // Zlicza stany końcowe.
    for (j = 0; j < layer[0]->size; ++j)
        if (layer[0]->array[j]->end)
        {
            state_array_add(end_states_array[0], layer[0]->array[j]);
            ++end_state_count;
        }
    // Dla każdej następnej warstwy.
    for (i = 1; i <= dict->max_cost; ++i)
    {
        // Dla każdej poprzedniej warstwy.
        for (j = i - 1; j >= 0; --j)
            // Dla każdego stanu w warstwie.
            for (k = 0; k < layer[j]->size; ++k)
            {
                tmp_state = layer[j]->array[k];
                size_t n = wcslen(tmp_state->suffix);
                // Dla każdej pasującej reguły.
                for (l = 0; l < suffix_rules[n]->size; ++l)
                {
                    // Łączny koszt.
                    int sum = suffix_rules[n]->array[l]->cost + j;
                    // Omija reguły o złych kosztach. 
                    if (sum < i)
                        continue;
                    else if (sum > i)
                        break;
                    // Zastosuj regułę i wpisz rozwinięte wyniki do warstwy. 
                    apply_rule(dict, tmp_state, 
                            suffix_rules[n]->array[l], layer[i]);
                }
            }
        // Usuwa śmieci.
        filter_layer(layer, i);
        // Zlicza stany końcowe.
        for (j = 0; j < layer[i]->size; ++j)
            if (layer[i]->array[j]->end && layer[i]->array[j]->ignore != 1)
            {
                state_array_add(end_states_array[i], layer[i]->array[j]);
                ++end_state_count;
            }
        // Sprawdza ograniczenie liczby stanów końcowych.
        if (end_state_count >= DICTIONARY_MAX_HINTS)
        {
            last_layer = i;
            break;
        }
    }
    // Wyłuskanie wyników.
    struct word_list_s result_list[last_layer + 1];
    wchar_t *extracted_string;
    for (i = 0; i < last_layer + 1; ++i)
    {
        word_list_s_init(result_list + i);
        for (j = 0; j < end_states_array[i]->size; ++j)
        {
            extracted_string = state_extract(end_states_array[i]->array[j]);
            word_list_s_add(result_list + i, extracted_string);
            free(extracted_string);
        }
    }
    size_t final_hints_count = (end_state_count >= DICTIONARY_MAX_HINTS) ? 
        DICTIONARY_MAX_HINTS : end_state_count;
    k = 0;
    for (i = 0; i < last_layer + 1; ++i)
    {
        for (j = 0; j < word_list_s_size(result_list + i); ++j)
        {
            word_list_add(list, result_list[i].array[j]);
            ++k;
            if (k >= final_hints_count)
                break;
        }
        if (k >= final_hints_count)
            break;
    }
    // Zwalnia pamięć.
    for (i = 0; i < last_layer + 1; ++i)
        word_list_s_done(result_list + i);
    for (i = 0; i < dict->max_cost; ++i)
    {
        state_array_kill(layer[i]);
        state_array_done(end_states_array[i]);
    }
    for (i = 0; i < sr_size; ++i)
        rule_array_done(suffix_rules[i]);
    free(suffix_rules);
    free(str);
}

int dictionary_lang_list(char **list, size_t *list_len)
{
    if (! directory_cleanup())
        return -1;
    int ret;
    struct word_list_s wlist;
    ret = read_dict_list(&wlist);
    if (ret == 1)
        ret = word_list_s_to_argz(&wlist, list, list_len);
    word_list_s_done(&wlist);
    return (ret == 1) ? 0 : -1;
}

struct dictionary * dictionary_load_lang(const char *lang)
{
    if (! directory_cleanup())
        return NULL;
    char path[strlen(CONF_PATH) + strlen(lang) + 1];
    strcpy(path, CONF_PATH);
    strcat(path, "/");
    strcat(path, lang);
    FILE *f = fopen(path, "r");
    struct dictionary *new_dict;
    if (!f || !(new_dict = dictionary_load(f)))
        return NULL;
    fclose(f);
    return new_dict;
}

int dictionary_save_lang(const struct dictionary *dict, const char *lang)
{
    if (! directory_cleanup())
        return -1;
    char path[strlen(CONF_PATH) + strlen(lang) + 1];
    strcpy(path, CONF_PATH);
    strcat(path, "/");
    strcat(path, lang);
    FILE *f = fopen(path, "w");
    if (!f || dictionary_save(dict, f))
        return -1;
    fclose(f);
    f = fopen(CONF_FILE, "a");
    fprintf(f, "\n%s", lang);
    fclose(f);
    return 0;
}

int dictionary_hints_max_cost(struct dictionary *dict, int new_cost)
{
    int ret = dict->max_cost;
    if (new_cost >=0)
        dict->max_cost = new_cost;
    return ret;
}

void dictionary_rule_clear(struct dictionary *dict)
{
    rule_array_kill(dict->rules);
    dict->rules = rule_array_new();
}

int dictionary_rule_add(struct dictionary *dict,
                        const wchar_t *left, const wchar_t *right,
                        bool bidirectional,
                        int cost,
                        enum rule_flag flag)
{
    // Sprawdza koszt i flagę.
    if (cost <= 0 || flag > RULE_SPLIT)
        return 0;

    // Sprawdza pustość stron.
    if (flag != RULE_SPLIT && 
            wcslen(left) == 0 && wcslen(right) == 0)
        return 0;

    int i, illegal_character, used_variables[2][10], var_diff[2] = {0, 0};
    long j;
    size_t n;
    const wchar_t *testStrings[2] = {left, right};

    // Sprawdza poprawność znaków i zlicza zmienne.
    illegal_character = 0;
    for (i = 0; i < 10; ++i)
        used_variables[0][i] = used_variables[1][i] = 0;
    for (i = 0; i < 2; ++i)
    {
        n = wcslen(testStrings[i]);
        for (j = 0; j < n; ++j)
        {
            if (!iswalnum(testStrings[i][j]))
            {
                illegal_character = 1;
                break;
            }
            else if (iswdigit(testStrings[i][j]))
                used_variables[i][testStrings[i][j] - 48] = 1;
        }
        if (illegal_character)
            break;
    }
    if (illegal_character)
        return 0;

    // Oblicza różnice ilości zmiennych.
    for (i = 0; i < 10; ++i)
        var_diff[0] += used_variables[1][i] - used_variables[0][i];
    var_diff[1] = -var_diff[0];

    n = 1;
    if (bidirectional)
        ++n;

    j = 0;
    for (i = 0; i < n; ++i)
    {
        // Sprawdza ilość wolnych zmiennych.
        if (var_diff[i] > 1)
            continue;

        // Dodaje.
        struct rule *r = rule_new(testStrings[i], testStrings[(i + 1) % 2], cost, flag);
        rule_array_add(dict->rules, r);
        ++j;
    }
    return j;
}

/**@}*/
