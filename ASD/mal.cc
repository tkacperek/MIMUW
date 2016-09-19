#include <cstdio>
#include <utility>
#include <set>
using namespace std;

typedef pair<long, long> pll;

struct Less
{
    bool operator() (const pll& lhs, const pll& rhs) const
    {
        return lhs.second < rhs.first;
    }
};

typedef set<pll, Less> spll;
typedef spll::iterator splli;

void printSet(spll& s)
{
    printf("set[");
    for (splli it = s.begin(); it != s.end(); ++it)
    {
        if (it != s.begin())
            printf(", ");
        printf("(%ld, %ld)", it->first, it->second);
    }
    printf("]\n");
}

int main()
{
    long n, m, whiteCount = 0;
    // zbiór przedziałów rozłącznych
    spll whiteSet;
    Less less = Less();

    scanf("%ld", &n);
    scanf("%ld", &m);
    //printSet(whiteSet);
    for (long i = 0; i < m; ++i)
    {
        pll l, r;
        scanf("%ld %ld", &(l.first), &(r.first));
        l.second = l.first;
        r.second = r.first;
        char color;
        scanf(" %c", &color);
        if (color == 'B')
        {
            long letsAdd = r.first - l.first + 1;
            splli tmp = whiteSet.lower_bound(l);

            if (tmp != whiteSet.end() && !less(l, *tmp))
            {
                if (less(*tmp, r))
                {
                    letsAdd -= tmp->second - l.second + 1;

                    l.first = l.second = tmp->first;
                    splli tmp2 = tmp;
                    ++tmp;
                    whiteSet.erase(tmp2);
                }
                else
                {
                    letsAdd = 0;
                    tmp = whiteSet.end();
                }
            }

            while (tmp != whiteSet.end() && less(*tmp, r))
            {
                letsAdd -= tmp->second- tmp->first + 1;

                splli tmp2 = tmp;
                ++tmp;
                whiteSet.erase(tmp2);
            }

            if (tmp != whiteSet.end() && !less(*tmp, r) && !less(r, *tmp))
            {
                letsAdd -= r.first - tmp->first + 1;

                r.first = r.second = tmp->second;
                whiteSet.erase(tmp);
            }

            whiteSet.insert(pll(l.first, r.first));
            whiteCount += letsAdd;
        }
        else // 'C'
        {
            long letsRemove = 0;
            splli tmp = whiteSet.lower_bound(l);

            if (tmp != whiteSet.end() && !less(l, *tmp))
            {
                if (less(*tmp, r))
                {
                    long remove = tmp->second - l.second + 1;
                    letsRemove += remove;

                    long left = tmp->first, right = tmp->second - remove;
                    splli tmp2 = tmp;
                    ++tmp;
                    whiteSet.erase(tmp2);
                    whiteSet.insert(pll(left, right));
                }
                else
                {
                    letsRemove = r.first - l.first + 1;
                    long left = tmp->first, right = tmp->second;
                    whiteSet.erase(tmp);
                    whiteSet.insert(pll(left, l.first - 1));
                    whiteSet.insert(pll(r.first + 1, right));
                    tmp = whiteSet.end();
                }
            }

            while (tmp != whiteSet.end() && less(*tmp, r))
            {
                letsRemove += tmp->second - tmp->first + 1;

                splli tmp2 = tmp;
                ++tmp;
                whiteSet.erase(tmp2);
            }

            if (tmp != whiteSet.end())
                if (!less(*tmp, r) && !less(r, *tmp))
                {
                    long remove = r.first - tmp->first + 1;
                    letsRemove += remove;

                    long left = tmp->first + remove, right = tmp->second;
                    whiteSet.erase(tmp);
                    whiteSet.insert(pll(left, right));
                }

            whiteCount -= letsRemove;
        }
        //printSet(whiteSet);
        printf("%ld\n", whiteCount);
    }
}
