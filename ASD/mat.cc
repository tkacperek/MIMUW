#include <cstdio>
#include <climits>
#include <algorithm>
using namespace std;

int main()
{
	char c;
	char ast = '*';
	long n = 0;
	char prev_color = -1;

	long gap_len = 0;
	long min_gap = LONG_MAX;

	long prev_seg_len = 0;
	long seg_len = 0;

	while (! feof(stdin))
	{
		scanf("%c", &c);
		if (c == 10)
		{
			// check for maximum gap
			if (min_gap > n)
				min_gap = n - 1;

			// add last asterisks to the last segment
			prev_seg_len += seg_len;
			seg_len = 0;
			break;
		}
		else
		{
			++n;

			if (c == ast)
			{
				// find shortest gap

				// if prev_color == -1 then skip, else
				if (prev_color != -1)
					++gap_len;

				// find length of last segment
				++seg_len;
			}
			else if (c == prev_color)
			{
				// find shortest gap
				gap_len = 0;

				// find length of last segment

				++seg_len;
				prev_seg_len += seg_len;
				seg_len = 0;
			}
			// different color
			else
			{
				// find shortest gap

				if (gap_len < min_gap && prev_color != -1)
					min_gap = gap_len;
				gap_len = 0;

				// find length of last segment

				++seg_len;
				prev_seg_len = seg_len;
				seg_len = 0;

				prev_color = c;
			}
		}
	}
	printf("%ld\n", n - min(min_gap, prev_seg_len - 1));

	return 0;
}
