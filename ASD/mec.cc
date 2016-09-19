#include <cstdio>
#include <vector>
#include <algorithm>
using namespace std;

int main() {
	long j, n, hn, a;
	int i, m;
	scanf("%ld %d", &n, &m);
	vector<long long> players (n, 0);
	hn = n / 2;

	for (i = 0; i < m; ++i)
		for (j = 1; j <= n; ++j) {
			scanf("%ld", &a);
			if (j > hn)
				players[a - 1] = players[a - 1] | (1LL << i);
		}
	sort(players.begin(), players.end());

	bool ok = true;
	for (j = 0; j < n - 1 && ok; ++j)
		if (players[j] == players[j + 1])
			ok = false;
	printf("%s\n", (ok ? "TAK" : "NIE"));
}
