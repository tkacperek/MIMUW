#include <cstdio>
#include <ctime>
using namespace std;

int main()
{
	long n, m, k, i;
	long *products;
	long *pre_max_even, *pre_max_odd;
	long *post_min_even, *post_min_odd;
	long long *answer, mod1, mod2;


	scanf("%ld", &n);
	products = new long [n];
	pre_max_even = new long [n];
	pre_max_odd = new long [n];
	post_min_odd = new long [n];
	post_min_even = new long [n];
	answer = new long long [n];


	pre_max_odd[0] = pre_max_even[0] = -1;
	for (i = 0; i < n - 1; ++i)
	{
		scanf("%ld", &products[i]);

		if (products[i] % 2 == 0)
		{
			pre_max_even[i + 1] = products[i];
			pre_max_odd[i + 1] = pre_max_odd[i];
		}
		else
		{
			pre_max_odd[i + 1] = products[i];
			pre_max_even[i + 1] = pre_max_even[i];
		}
	}
	scanf("%ld", &products[n - 1]);


	answer[n - 1] = products [n - 1];
	if (products[n - 1] % 2 == 0)
	{
		post_min_even[n - 1] = products[n - 1];
		post_min_odd[n - 1] = -1;
	}
	else
	{
		post_min_odd[n - 1] = products[n - 1];
		post_min_even[n - 1] = -1;
	}
	for (i = n - 2; i >= 0; --i)
	{
		if (products[i] % 2 == 0)
		{
			post_min_even[i] = products[i];
			post_min_odd[i] = post_min_odd[i + 1];
		}
		else
		{
			post_min_odd[i] = products[i];
			post_min_even[i] = post_min_even[i + 1];
		}
		answer[i] = answer[i + 1] + products[i];
	}


	for (i = n - 1; i >= 0; --i)
	{
		if (answer[i] % 2 == 0)
		{
			if (pre_max_odd[i] == -1 || post_min_even[i] == -1)
				mod1 = 1;
			else
				mod1 = pre_max_odd[i] - post_min_even[i];

			if (pre_max_even[i] == -1 || post_min_odd[i] == -1)
				mod2 = 1;
			else
				mod2 = pre_max_even[i] - post_min_odd[i];

			if (mod1 + mod2 == 2)
				answer[i] = -1;
			else if (mod1 == 1 || mod2 == 1)
				answer[i] += ((mod1 <= mod2) ? mod1 : mod2);
			else
				answer[i] += ((mod1 >= mod2) ? mod1 : mod2);
		}
	}


	delete [] products;
	delete [] pre_max_even;
	delete [] pre_max_odd;
	delete [] post_min_odd;
	delete [] post_min_even;


	scanf("%ld", &m);
	for (i = 0; i < m; ++i)
	{
		scanf("%ld", &k);
		printf("%lld\n", answer[n - k]);
	}
	delete [] answer;


	return 0;
}
