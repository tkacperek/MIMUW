#include <cstdio>
#include <cstdint>
#include <vector>
#include <utility>
#include <algorithm>
#include <limits>
#include <cstdlib>
#include <set>
using namespace std;

// macros
#define FOR(i, a, b) for (int32_t i = a; i < b; ++i)

// types
using ipair = pair<int32_t, int32_t>;
using ilpair = pair<int64_t, int32_t>;

//----------------------------------------------------------------

// distance
inline int64_t dist(const ipair& lhs, const ipair& rhs)
{
    return min(
        abs(lhs.first - rhs.first),
        abs(lhs.second - rhs.second)
    );
}

//----------------------------------------------------------------
int main()
{
    // vars
    int32_t n;
    vector<ipair> position;
    vector<vector<int32_t> > neighbours;
    vector<int64_t> distance;
    set<ilpair> q;
   
    // input
    scanf("%d", &n);
    position.resize(n);
    neighbours.resize(n);
    distance.resize(n);
    FOR(i, 0, n)
    {
        int32_t a, b;
        scanf("%d %d", &a, &b);
        position[i] = make_pair(a, b);
        distance[i] = numeric_limits<int64_t>::max();
    }
    distance[0] = 0;

    // neighbours
    {
        vector<ipair> x, y;
        x.resize(n), y.resize(n);
        FOR(i, 0, n)
        {
            x[i] = make_pair(position[i].first, i);
            y[i] = make_pair(position[i].second, i);
        }
        sort(x.begin(), x.end());
        sort(y.begin(), y.end());

        FOR(i, 0, n)
        {
            if (i != 0)
            {
                neighbours[x[i].second].push_back(x[i - 1].second);
                neighbours[y[i].second].push_back(y[i - 1].second);
            }
            if (i != n - 1)
            {
                neighbours[x[i].second].push_back(x[i + 1].second);
                neighbours[y[i].second].push_back(y[i + 1].second);
            }
        }
    }

    // dijkstra
    q.insert(make_pair(distance[0], 0));
    while (!q.empty())
    {
        ilpair current = *q.begin();
        q.erase(q.begin());
        auto cid = current.second;
        if (cid == n - 1)
            break;
        else
        {
            for (auto x: neighbours[cid])
            {
                auto tmp = distance[cid] + dist(position[cid], position[x]);
                if (distance[x] == numeric_limits<int64_t>::max())
                {
                    distance[x] = tmp;
                    q.insert(make_pair(distance[x], x));
                }
                else if (tmp < distance[x])
                {
                    set<ilpair>::iterator ix = q.find(make_pair(distance[x], x));
                    q.erase(ix);
                    distance[x] = tmp;
                    q.insert(make_pair(distance[x], x));
                }
            }
            distance[cid] = 0;
        }
    }

    // out
    printf("%lld\n", distance[n - 1]);
}
