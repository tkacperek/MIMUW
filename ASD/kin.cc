/**
 *  Zadanie: KIN
 *  Autor: Tomasz Kacperek <tk334578@students.mimuw.edu.pl>
 */
#include <cstdio>
using namespace std;

const long MOD = 1000000000;

int main ()
{
    // rozmiar tablicy, rozmiar drzewa bez liści, rozmiar inwersji
    unsigned n, m, k;
    // liczba k-inwersji modulo MOD 
    long result;
    // aktualnie rozważamy inv-inwersje
    int inv;
    // badana permutacja
    int *perm;
    // count [i] mówi ile inwersji kończy się w i-tym elemencie tablicy
    long *count;
    // drzewo przedziałowe zaimplementowane w tablicy
    // liście odpowiadają kolejno liczbom 1..n
    // ignoruję tree[0] dla ułatwienia indeksowania
    long *tree;

    // ustalenie rozmiarów
    scanf("%i %i", &n, &k);
    // m = 2^ceil(log(n, 2))
    m = 1;
    while (m < n)
        m *= 2;

    // przygotowanie tablic
    perm = new int [n];
    count = new long [n];
    tree = new long [2 * m];

    // wczytanie permutacji
    for (int i = 0; i < n; ++i)
        scanf("%d", perm + i);

    // 1-inwersje
    inv = 1;
    for (int i = 0; i < n; ++i)
        count [i] = 1;

    // pozostałe inwersje
    for (inv = 2; inv <= k; ++inv)
    {
        // czyszczenie drzewa
        for (int i = 1; i < 2 * m; ++i)
            tree [i] = 0;

        // po elementach permutacji
        for (int i = 0; i < n; ++i)
        {
            // aktualny indeks w drzewie
            int curr = m - 1 + perm [i];
            // do zapamiętania starej i budowania nowej wartości w count
            long old_count = count [i], new_count = 0;
            // czy poprzedni węzeł był lewym dzieckiem 
            // na początu patrzymy na liść, który nie ma dzieci,
            // więc wartość false jest poprawna :-)
            bool was_left = false; 

            // od liścia do korzenia włącznie
            while (curr > 0)
            {
                // aktualizowanie wartośći
                tree [curr] = (tree [curr] + old_count) % MOD;
                // jeżeli przyszliśmy z lewego dziecka
                if (was_left)
                    // to w prawym jest liczba większych elementów
                    // poprzedzających bieżący, do których można
                    // "dokleić" bieżący element
                    new_count = (new_count + tree [curr * 2 + 1]) % MOD;
                // krok w górę drzewa
                was_left = (curr + 1) % 2;
                curr /= 2;
            }

            // zachowanie policzonej wartości
            count [i] = new_count;
        }
    }

    // obliczenie wyniku
    result = 0;
    for (int i = 0; i < n; ++i)
        result = (result + count [i]) % MOD;

    printf("%ld\n", result);

    delete [] perm;
    delete [] count;
    delete [] tree;
}
