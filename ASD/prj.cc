//----------------------------------------------------------------
// Zadanie: Projekty
// Autor: Tomasz Kacperek <tk334578@students.mimuw.edu.pl>
//----------------------------------------------------------------

#include <cstdio>
#include <vector>
#include <set>
#include <utility>
#include <algorithm>
using namespace std;

int main()
{
    long n, m, k;
    scanf("%ld %ld %ld", &n, &m, &k);

    // waga wierzchołka
    vector<long> w(n);
    for (long i = 0; i < n; ++i)
        scanf("%ld", &w[i]);

    // stopnie wchodzące
    vector<long> deg(n);
    // "listy" sąsiedztwa
    vector<vector<long> > adj(n);
    for (long i = 0; i < m; ++i)
    {
        long a, b;
        scanf("%ld %ld", &a, &b);
        --a, --b;
        deg[a]++;
        adj[b].push_back(a);
    }

    // zbiór par: waga, indeks
    set<pair<long, long> > s;

    for (long i = 0; i < n; ++i)
        if (deg[i] == 0)
        {
            s.insert(make_pair(w[i], i));
            deg[i] = -1;
        }

    // maksymalna liczba pracowników
    long mx = 0;
    for (long i = 0; i < k; ++i)
    {
        long x = s.begin()->second;
        s.erase(s.begin());
        mx = max(mx, w[x]);
        for (unsigned long j = 0; j < adj[x].size(); ++j)
            if (--deg[adj[x][j]] == 0)
                s.insert(make_pair(w[adj[x][j]], adj[x][j]));
    }
    printf("%ld\n", mx);
}
