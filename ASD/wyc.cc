/*
	Zadanie: Wycieczka
	Autor: Tomasz Kacperek <tk334578@students.mimuw.edu.pl>
*/

#include <cstdio>
using namespace std;

// maksymalny rozmiar tablicy;
const int MAX_N = 1000;
// maksymalny koszt ścieżki + 1
// będzie używany jako "nieskończoność"
const long COST_BOUND = 2001001;

void draw_array(long cost[][MAX_N / 2 + 1], int cost_size) {
	int i, j;
	for (i = 0; i < cost_size; ++i) {
		for (j = 0; j < cost_size; ++j) {
			printf("%8ld", cost[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

int main() {
	// liczba miast - 1, maksymalna liczba taksówek
	int n, k;
	// indeksy do głównej pętli: pociąg, taksówka, samolot
	int t, c, p;
	// modyfikatory indeksów do poprzedniego dworca / lotniska
	int prev_t, prev_p;
	// koszt tymczasowy, kandydat na koszt i koszt kroku
	long tmp_cost, cost_candidate, step_cost;
	// wynik
	long total = COST_BOUND;
	// koszty: taksówek, pociągów, samolotów
	int cab_cost[MAX_N + 1],
		train_cost[MAX_N + 1],
		plane_cost[MAX_N + 1];
	// efektywny rozmiar tablicy cost
	int cost_size;
	/* Tablica cost reprezentuje aktualnie przetwarzaną warstwę w 3-wymiarowej
		tablicy, w której komórka [t, p, c] zawiera minimalny koszt
		dotarcia do miasta t+p przy pomocy t pociągów, p samolotów oraz
		c taksówek; ponadto c określa czy docieramy na dworzec czy lotnisko:
		jeżeli c jest parzyste to na dworzec,
		w przeciwnym przypadku na lotnisko. */
	long cost[MAX_N / 2 + 1][MAX_N / 2 + 1];
	
	/* koszt pociągu i taksówki do miasta 0 jest nieistotny, ale istnienie
		 tych komórek ułatwia indeksowanie */
	train_cost[0] = plane_cost[0] = -1;

	// wczytywanie danych
	scanf("%d %d", &n, &k);
	cost_size = n / 2 + 1;
	scanf("%d", &cab_cost[0]);
	for (int i = 0; i < n; ++i)
		scanf("%d %d %d",
			&train_cost[i + 1],
			&cab_cost[i + 1],
			&plane_cost[i + 1]);

	/* Pierwszą warstwę łatwiej wypełnić "ręcznie", ponieważ bez taksówek
		można poruszać się tylko pociągami. */
	for (t = 0; t < cost_size; ++t)
		for (p = 0; p < cost_size; ++p)
			cost[t][p] = COST_BOUND;
	cost[0][0] = 0;
	for (t = 1; t < cost_size; ++t)
		cost[t][0] = cost[t - 1][0] + train_cost[t];

	// główna pętla
	for (c = 1; c <= k; ++c) {
		for (t = 0; t < cost_size; ++t)
			for (p = 0; p < cost_size; ++p) {
				// kończymy na dworcu
				if (c % 2 == 0) {
					prev_t = -1;
					prev_p = 0;
					step_cost = train_cost[t + p];
				}
				//kończymy na lotnisku
				else {
					prev_t = 0;
					prev_p = -1;
					step_cost = plane_cost[t + p];
				}
				cost_candidate = COST_BOUND;
				// krok "w mieście"
				tmp_cost = cost[t][p] + cab_cost[t + p];
				if (tmp_cost < cost_candidate)
					cost_candidate = tmp_cost;
				// krok do poprzedniego dworca/lotniska
				if (t + prev_t >= 0 && p + prev_p >= 0) {
					tmp_cost = cost[t + prev_t][p + prev_p] + step_cost;
					if (tmp_cost < cost_candidate)
						cost_candidate = tmp_cost;
				}
				cost[t][p] = cost_candidate;
			}
		/* tylko nieparzysta liczba przejazdów taksówką może doprowadzić
			na lotnisko w ostatnim mieście */
		if (c % 2 == 1)
			if (cost[cost_size - 1][cost_size - 1] < total)
				total = cost[cost_size - 1][cost_size - 1];
	}
	

	// wypisanie wyniku
	printf("%ld\n", total);
}
