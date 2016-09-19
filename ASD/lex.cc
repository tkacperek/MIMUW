//----------------------------------------------------------------
// Zadanie: LEX
// Autor: Tomasz Kacperek <tk334578@students.mimuw.edu.pl>
//----------------------------------------------------------------

#include <cstdio>
#include <cstdint>
#include <vector>
#include <map>
#include <tuple>
using namespace std;

// macros
#define FORx(i, a, b) for (int32_t i = a; i < b; ++i)
#define FOR(i,n) FORx(i, 0, n)

// type aliases
using trip = tuple<int32_t, int32_t, int32_t>;

// trip comparator 
bool operator!=(const trip& lhs, const trip& rhs)
{
    tuple<int32_t, int32_t> l, r;
    l = make_tuple(get<1>(lhs), get<2>(lhs));
    r = make_tuple(get<1>(rhs), get<2>(rhs));
    return l != r;
}

// radix sort
void radix_sort(vector<trip>& buff, int& max_id)
{
    vector<vector<int32_t> > keya(max_id + 1), keyb(max_id + 1);
    for (auto& it: buff)
       keyb[get<2>(it)].push_back(get<0>(it));
    for (auto& it: keyb)
        for (auto& iti: it)
            keya[get<1>(buff[iti])].push_back(iti);
    vector<trip> tmp;
    for (auto& it: keya)
        for (auto& iti: it)
            tmp.push_back(buff[iti]);
    swap(buff, tmp);
}

//----------------------------------------------------------------
int main()
{
    // vars
    int32_t n, m;
    int8_t * word;
    map<int32_t, vector<int32_t> > bf;
    vector<trip> buffer;

    // input
    scanf("%d %d", &n, &m);
    word = new int8_t [n + 1];
    scanf("%s", word);
    FOR (i, n)
        word[i] -= 'a';

    // dictionary of basic factors

    // level 1
    bf[1] = vector<int32_t> (n);
    FOR (i, n)
        bf[1][i] = word[i];

    // next levels
    // previous length, current length, max id in prev, how many substrings 
    int32_t prev = 1, len = 2, max_id = 25, count;
    while (len <= n)
    {
        count = n - len + 1;
        bf[len] = vector<int32_t> (count);
        FOR (i, count)
            buffer.push_back(make_tuple(i, bf[prev][i], bf[prev][i + prev]));
        radix_sort(buffer, max_id);

        int32_t id = 0;
        bf[len][get<0>(buffer[0])] = id;
        FORx(i, 1, count)
        {
            if (buffer[i] != buffer[i - 1])
                ++id;
            bf[len][get<0>(buffer[i])] = id;
        }
        max_id = id;

        buffer.clear();
        prev = len;
        len *= 2;
    }

    // processing queries
    FOR(i, m)
    {
        int32_t a, b, c, d;
        scanf("%d %d %d %d", &a, &b, &c, &d);
        // make b, d lengths ob substrings
        b = b - a + 1;
        d = d - c + 1;
        // indexing from 0 alway in my <3
        --a, --c;

        // min len
        int32_t len = (b < d) ? b : d;

        // get top non-zero bit of len
        int32_t leng = 1, top_bit = 0;
        for (; leng <= len; leng <<= 1 , ++top_bit) {}
        leng >>= 1, --top_bit;

        int mask = 1 << top_bit;
        // cmp(l,r): (-1,>), (0,=), (1,<)
        int8_t cmp = 0;
        while (top_bit >= 0)
        {
            if ((mask & len) != 0)
            {
                // count cmp
                int32_t tmp =
                    bf[leng][a] - bf[leng][c];
                if (tmp < 0)
                    cmp = 1;
                else if (tmp > 0)
                    cmp = -1;

                a += leng, c += leng;
                b -= leng, d -= leng;

                if (cmp != 0)
                    break;
            }
            mask >>= 1; 
            leng >>= 1;
            --top_bit;
        }

        char out;

        if (cmp == 0)
        {
            if (b == d)
                out = '=';
            else
                out = (b > d) ? '>' : '<';
        }
        else
            out = (cmp > 0) ? '<' : '>';

        printf("%c\n", out);
    }

    delete [] word;
}
