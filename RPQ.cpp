#include <iostream>
#include <string.h>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <vector>
#include <chrono>
#include <queue>
#include <math.h>

using namespace std;

// --- klasa obiektów typu Dane, posiadaj¹ca pola dla problemu RPQ ---
class Dane {
public:
	int numer_zadania = 0;
	int r_przygotowanie = 0;
	int p_wykonanie = 0;
	int q_stygniecie = 0;
	int c_zakonczenie = 0;       //czas zakoñczenia zadania


	void wypisz()
	{
		cout << numer_zadania + 1 << " " << r_przygotowanie << " " << p_wykonanie << " " << q_stygniecie << endl;
	}

	void podajPermutacje()
	{
		cout << numer_zadania + 1 << " ";
	}
};

int UB = 10000000;

// --- struktura porownujaca dwa obiekty typu Dane ---
// --- wzglêdem parametru 'r' a nastêpnie 'q' ---
struct PorownajRQ
{
	bool operator () (Dane x1, Dane x2)
	{
		if (x1.r_przygotowanie != x2.r_przygotowanie)
		{
			if (x1.r_przygotowanie < x2.r_przygotowanie)
				return true;
			else return false;
		}
		else
		{
			if (x1.q_stygniecie < x2.q_stygniecie)
				return true;
			else return false;
		}
	}
}PorownanieRQ;

struct PorownajR
{
	bool operator ()(Dane zadanie1, Dane zadanie2)
	{
		if (zadanie1.r_przygotowanie < zadanie2.r_przygotowanie)
			return true;
		else if (zadanie1.r_przygotowanie > zadanie2.r_przygotowanie)
			return false;
		else
			return false;
	}
} PorownanieR;

// --- struktura porownujaca dwa obiekty typu Dane ---
// --- wzglêdem parametru 'q' ---
struct PorownajDaneG_Malejaco
{
	bool operator ()(Dane zadanie1, Dane zadanie2)
	{
		if (zadanie1.q_stygniecie < zadanie2.q_stygniecie)
			return true;
		else if (zadanie1.q_stygniecie > zadanie2.q_stygniecie)
			return false;
		else
			return false;
	}
} SortowanieMalejacoQ;

// --- struktura porownujaca dwa obiekty typu Dane ---
// --- wzglêdem parametru 'q' ---
struct PorownajDaneG_Rosnaco
{
	bool operator ()(Dane zadanie1, Dane zadanie2)
	{

		if (zadanie1.q_stygniecie > zadanie2.q_stygniecie)
			return true;
		else if (zadanie1.q_stygniecie < zadanie2.q_stygniecie)
			return false;
		else
			return false;
	}
} SortowanieRosnacoQ;

// --- struktura porownujaca dwa obiekty typu Dane ---
// --- wzglêdem parametru 'r' ---
struct PorownajDaneR
{
	bool operator ()(Dane zadanie1, Dane zadanie2)
	{
		if (zadanie1.r_przygotowanie > zadanie2.r_przygotowanie)
			return true;
		else if (zadanie1.r_przygotowanie < zadanie2.r_przygotowanie)
			return false;
		else
			return false;
	}
} SortowanieR;

// --- funkcja celu dla problemu RPQ - szukane kryterium optymalizacyjne ---
// --- funckja zwraca - Cmax - czas dostarczenia i zakonczenia wszystkich zadan ---
int RPQ_funkcjaCelu(int iloscZadan, vector<Dane> zadania, vector<Dane>& PI_permutacja)
{
	// --- sortowanie po RQ --- 
	sort(zadania.begin(), zadania.end(), PorownanieRQ);
	// --- sortowanie po R ---
	//sort(zadania.begin(), zadania.end(), PorownanieR);

	for (int i = 0; i < iloscZadan; i++)
	{
		PI_permutacja.push_back(zadania[i]);
	}

	vector<int> S_rozpoczecie(iloscZadan); // wektor momentow rozpoczecia wykonywania zadan
	vector<int> C_zakonczenie(iloscZadan); // wektor momentow zakonczenia wykonywania zadan
	int Cmax = 0;

	S_rozpoczecie[0] = zadania[0].r_przygotowanie;
	C_zakonczenie[0] = S_rozpoczecie[0] + zadania[0].p_wykonanie;
	Cmax = C_zakonczenie[0] + zadania[0].q_stygniecie;
	for (int i = 1; i < iloscZadan; ++i)
	{
		S_rozpoczecie[i] = max(zadania[i].r_przygotowanie, C_zakonczenie[i - 1]);
		C_zakonczenie[i] = S_rozpoczecie[i] + zadania[i].p_wykonanie;
		Cmax = max(Cmax, C_zakonczenie[i] + zadania[i].q_stygniecie);
	}
	S_rozpoczecie.clear();
	C_zakonczenie.clear();

	return Cmax; // zwraca wartosci Cmax
}

// --- algorytm Schrage dla problemu RPQ z wykorzystaniem kolejki priorytetowej ---
// --- zwraca Cmax - czas dostarczenia i zakonczenia wszystkich zadan ---
int SchrageQueue(int iloscZadan, vector<Dane> zadania, vector<Dane>& PI_permutacja)
{
	priority_queue <Dane, vector <Dane>, PorownajDaneR> N_nieuszeregowane;
	priority_queue <Dane, vector <Dane>, PorownajDaneG_Malejaco> G_gotowe;
	int Cmax = 0;

	for (int i = 0; i < iloscZadan; ++i)
	{
		N_nieuszeregowane.push(zadania[i]);
	}

	int t_czas = N_nieuszeregowane.top().r_przygotowanie;

	while (!G_gotowe.empty() || !N_nieuszeregowane.empty())
	{
		while (!N_nieuszeregowane.empty() && (N_nieuszeregowane.top().r_przygotowanie <= t_czas))
		{
			G_gotowe.push(N_nieuszeregowane.top()); // dodanie na listê gotowych zadañ
			N_nieuszeregowane.pop(); // usuniêcie z listy nieuszeregowanych
		}
		if (!G_gotowe.empty())
		{
			t_czas = t_czas + G_gotowe.top().p_wykonanie;           // aktualizacja czasu
			PI_permutacja.push_back(G_gotowe.top());                // zapamietanie kolejnosci zadan
			PI_permutacja.back().c_zakonczenie = t_czas;
			Cmax = max(Cmax, t_czas + G_gotowe.top().q_stygniecie); // zaktualizowanie calkowitego czasu Cmax
			G_gotowe.pop(); // usuniecie z gotowych
		}
		else
			t_czas = N_nieuszeregowane.top().r_przygotowanie;
	}
	return Cmax;
}

// --- algorytm Schrage dla problemu RPQ wyszukujacy element max i min ---
// --- poprzez sortowanie zbioru G ---
// --- zwraca Cmax - czas dostarczenia i zakonczenia wszystkich zadan ---
int Schrage(int iloscZadan, vector<Dane> zadania, vector<Dane>& PI_permutacja)
{
	vector<Dane> N_nieuszeregowane;
	vector<Dane> G_gotowe;
	int Cmax = 0;

	// sortowanie po R zbioru zadan przed wpisaniem w N_nieuszeregowane
	sort(zadania.begin(), zadania.end(), SortowanieR);
	for (int i = 0; i < iloscZadan; ++i)
	{
		N_nieuszeregowane.push_back(zadania[i]);
	}

	int t_czas = N_nieuszeregowane.back().r_przygotowanie;

	while (!G_gotowe.empty() || !N_nieuszeregowane.empty())
	{
		while (!N_nieuszeregowane.empty() && (N_nieuszeregowane.back().r_przygotowanie <= t_czas))
		{
			G_gotowe.push_back(N_nieuszeregowane.back()); // dodanie na listê gotowych zadañ
			N_nieuszeregowane.pop_back(); // usuniêcie z listy nieuszeregowanych
		}
		if (!G_gotowe.empty())
		{
			// sortowanie zbioru G po parametrze 'q'
			sort(G_gotowe.begin(), G_gotowe.end(), SortowanieMalejacoQ);
			t_czas = t_czas + G_gotowe.back().p_wykonanie;          // aktualizacja czasu
			PI_permutacja.push_back(G_gotowe.back());                // zapamietanie kolejnosci zadan
			PI_permutacja.back().c_zakonczenie = t_czas;
			Cmax = max(Cmax, t_czas + G_gotowe.back().q_stygniecie); // zaktualizowanie calkowitego czasu Cmax
			G_gotowe.pop_back(); // usuniecie z gotowych
		}
		else
			t_czas = N_nieuszeregowane.back().r_przygotowanie;
	}
	N_nieuszeregowane.clear();
	G_gotowe.clear();

	return Cmax;
}


// --- algorytm Schrage dla problemu RPQ z przerwaniami z wykorzystaniem kolejki priorytetowej ---
// --- zwraca Cmax - czas dostarczenia i zakonczenia wszystkich zadan ---
int SchrageQueuePMTN(int iloscZadan, vector<Dane> zadania)
{
	priority_queue <Dane, vector <Dane>, PorownajDaneR> N_nieuszeregowane;
	priority_queue <Dane, vector <Dane>, PorownajDaneG_Malejaco> G_gotowe;
	Dane l_aktualne, e_dodane;
	int Cmax = 0;

	for (int i = 0; i < iloscZadan; ++i)
	{
		N_nieuszeregowane.push(zadania[i]);
	}

	int t_czas = N_nieuszeregowane.top().r_przygotowanie;


	while (!G_gotowe.empty() || !N_nieuszeregowane.empty())
	{
		while (!N_nieuszeregowane.empty() && (N_nieuszeregowane.top().r_przygotowanie <= t_czas))
		{
			e_dodane = N_nieuszeregowane.top();
			G_gotowe.push(e_dodane); // dodanie na listê gotowych zadañ
			N_nieuszeregowane.pop(); // usuniêcie z listy nieuszeregowanych

			// jezeli 'q' nowo dodanego zadania jest wieksze niz 'q' zadania aktualnego
			if (e_dodane.q_stygniecie > l_aktualne.q_stygniecie)
			{
				// pomniejszenie czasu wykonywania o tyle, ile zostalo juz wykonane
				l_aktualne.p_wykonanie = t_czas - e_dodane.r_przygotowanie;
				t_czas = e_dodane.r_przygotowanie;
				if (l_aktualne.p_wykonanie > 0)
				{
					G_gotowe.push(l_aktualne);
				}
			}
		}
		if (!G_gotowe.empty())
		{
			t_czas = t_czas + G_gotowe.top().p_wykonanie; // aktualizacja czasu
			Cmax = max(Cmax, t_czas + G_gotowe.top().q_stygniecie); // zaktualizowanie calkowitego czasu Cmax
			l_aktualne = G_gotowe.top();
			G_gotowe.pop(); // usuniecie z gotowych
		}
		else
			t_czas = N_nieuszeregowane.top().r_przygotowanie;
	}
	return Cmax;
}

// --- algorytm Schrage dla problemu RPQ z przerwaniami z wyborem elementu max i min ---
// --- poprzez sortowanie zbioru G ---
// --- zwraca Cmax - czas dostarczenia i zakonczenia wszystkich zadan ---
int SchragePMTN(int iloscZadan, vector<Dane> zadania)
{
	vector<Dane> N_nieuszeregowane;
	vector<Dane> G_gotowe;
	Dane l_aktualne, e_dodane;
	int Cmax = 0;

	// sortowanie po R zbioru zadan przed wpisaniem w N_nieuszeregowane
	sort(zadania.begin(), zadania.end(), SortowanieR);
	for (int i = 0; i < iloscZadan; ++i)
	{
		N_nieuszeregowane.push_back(zadania[i]);
	}

	int t_czas = N_nieuszeregowane.back().r_przygotowanie;

	while (!G_gotowe.empty() || !N_nieuszeregowane.empty())
	{
		while (!N_nieuszeregowane.empty() && (N_nieuszeregowane.back().r_przygotowanie <= t_czas))
		{
			e_dodane = N_nieuszeregowane.back();
			G_gotowe.push_back(e_dodane); // dodanie na listê gotowych zadañ
			N_nieuszeregowane.pop_back(); // usuniêcie z listy nieuszeregowanych

			if (e_dodane.q_stygniecie > l_aktualne.q_stygniecie)
			{
				// pomniejszenie czasu wykonywania o tyle, ile zostalo juz wykonane
				l_aktualne.p_wykonanie = t_czas - e_dodane.r_przygotowanie;
				t_czas = e_dodane.r_przygotowanie;
				if (l_aktualne.p_wykonanie > 0)
				{
					G_gotowe.push_back(l_aktualne);
				}
			}
		}
		if (!G_gotowe.empty())
		{
			// sortowanie zbioru G po q
			sort(G_gotowe.begin(), G_gotowe.end(), SortowanieMalejacoQ);
			t_czas = t_czas + G_gotowe.back().p_wykonanie; // aktualizacja czasu
			Cmax = max(Cmax, t_czas + G_gotowe.back().q_stygniecie); // zaktualizowanie calkowitego czasu Cmax
			l_aktualne = G_gotowe.back();
			G_gotowe.pop_back(); // usuniecie z gotowych
		}
		else
			t_czas = N_nieuszeregowane.back().r_przygotowanie;
	}
	N_nieuszeregowane.clear();
	G_gotowe.clear();

	return Cmax;
}

// --- funkcja do wyszukiwania zadania a z grupy (algorytm Carlier)
int find_a(int b, int Cmax_Schrage, const vector<Dane> PI_permutacja)
{
	int sum_a = 0;
	int a;
	for (a = 0; a <= b; a++)
	{
		sum_a = 0;
		for (int i = a; i <= b; i++)
		{
			sum_a += PI_permutacja[i].p_wykonanie;
		}

		if (Cmax_Schrage == (PI_permutacja[a].r_przygotowanie + sum_a + PI_permutacja[b].q_stygniecie))
		{
			return a;
		}
	}
	return a;
}

// --- funkcja do wyszukiwania zadania b z grupy (algorytm Carlier)
int find_b(int iloscZadan, int Cmax_Schrage, const vector<Dane> PI_permutacja)
{
	int b = iloscZadan - 1;
	for (int i = iloscZadan - 1; i > 0; i--)
	{
		if (Cmax_Schrage == (PI_permutacja[i].c_zakonczenie + PI_permutacja[i].q_stygniecie))
		{
			b = i;
			break;
		}
	}
	return b;
}

// --- funkcja do wyszukiwania zadania c z grupy (algorytm Carlier)
int find_c(int a, int b, const vector<Dane> PI_permutacja)
{
	int c = -1;
	for (int i = b - 1; i >= a; i--)
	{
		if (PI_permutacja[i].q_stygniecie < PI_permutacja[b].q_stygniecie)
		{
			c = i;
			break;
		}
	}
	return c;
}

// --- algorytm Carlier dla problemu RPQ 
// --- zwraca Cmax - czas dostarczenia i zakonczenia wszystkich zadan ---
int Carlier(int iloscZadan, vector<Dane> zadania, vector<Dane>& PI_permutacja)
{
	int Cmax = 0;
	int p_sum = 0, nr_zadania_dla_c = 0;
	int r_dla_c = 0, q_dla_c = 0;
	int r_new_for_c = 10000000;
	int q_new_for_c = 10000000;
	int U = 0, LB = 0;
	int a = 0, b = 0, c = -1;

	PI_permutacja.clear();

	// Wykonanie algorytmu Schrage
	U = SchrageQueue(iloscZadan, zadania, PI_permutacja);

	if (U < UB)
		UB = U;

	// Znajdowanie zadañ a,b,c z grupy
	b = find_b(iloscZadan, U, PI_permutacja);
	a = find_a(b, U, PI_permutacja);
	c = find_c(a, b, PI_permutacja);

	// Jesli zadanie c nie zosta³o znalezione wyznaczony z SchrageQueue czas jest optymalny
	if (c == -1)
		return UB;


	// Wyszukujemy min r, q oraz sumê p z nowej grupy od indeksu c+1 do b
	for (int i = c + 1; i <= b; i++)
	{
		r_new_for_c = min(r_new_for_c, PI_permutacja[i].r_przygotowanie);
		q_new_for_c = min(q_new_for_c, PI_permutacja[i].q_stygniecie);
		p_sum += PI_permutacja[i].p_wykonanie;
	}
	// Zapamietujemy wartoœci r i q dla zadania o indeksie c.
	// Bedzie trzeba je potem odtworzyæ
	nr_zadania_dla_c = PI_permutacja[c].numer_zadania;
	r_dla_c = PI_permutacja[c].r_przygotowanie;
	q_dla_c = PI_permutacja[c].q_stygniecie;

	// Przypisujemy now¹ wartoœæ r dla zadania o indeksie c
	PI_permutacja[c].r_przygotowanie = max(PI_permutacja[c].r_przygotowanie, r_new_for_c + p_sum);

	// Szacowanie za pomoc¹ algorytmu z przerwaniami
	LB = SchrageQueuePMTN(iloscZadan, PI_permutacja);

	// Jesli szaczowanie wysz³o lepsze wywo³ujemy rekurencyjnie Carlier
	if (LB < UB)
	{
		Carlier(iloscZadan, PI_permutacja, PI_permutacja);
	}

	// Przywracanie zapamiêtanej wartoœci r
	for (int i = 0; i < iloscZadan; i++)
	{
		if (nr_zadania_dla_c == PI_permutacja[i].numer_zadania)
		{
			PI_permutacja[i].r_przygotowanie = r_dla_c;
		}
	}

	// Przypisujemy now¹ wartoœæ q dla zadania o indeksie c
	PI_permutacja[c].q_stygniecie = max(PI_permutacja[c].q_stygniecie, q_new_for_c + p_sum);

	LB = SchrageQueuePMTN(iloscZadan, PI_permutacja);

	if (LB < UB)
	{
		Carlier(iloscZadan, PI_permutacja, PI_permutacja);
	}

	// Przywracanie zapamiêtanej wartoœci q
	for (int i = 0; i < iloscZadan; i++)
	{
		if (nr_zadania_dla_c == PI_permutacja[i].numer_zadania)
		{
			PI_permutacja[i].q_stygniecie = q_dla_c;
		}
	}

	return UB;
}


int main()
{
	fstream plik;
	int iloscZadan = 0, iloscDanych = 0;

	plik.open("data10.txt", ios::in);
	if (plik.good() == true)
	{
		plik >> iloscZadan >> iloscDanych;
		vector<Dane> zadania(iloscZadan);
		vector<Dane> PI_permutacja;

		for (int i = 0; i < iloscZadan; ++i)
		{
			zadania[i].numer_zadania = i;
			plik >> zadania[i].r_przygotowanie >> zadania[i].p_wykonanie >> zadania[i].q_stygniecie;
			//zadania[i].wypelnij(plik);       
		}
		plik.close();


		// --- wyœwietlenie wyników --- //
		// --- tam gdzie mo¿na, najpierw permutacja ---//
		// --- potem Cmax --- //
		int wynik = 0;
		// wyswietlenie dla funkcji celu RPQ
		cout << "Wyniki dzialania algorytmow" << "\n\n";
		wynik = RPQ_funkcjaCelu(iloscZadan, zadania, PI_permutacja);
		for (int i = 0; i < iloscZadan; ++i)
		{
			cout << PI_permutacja[i].numer_zadania << " ";
		}
		cout << "\n" << "#RPQ " << wynik << endl;
		PI_permutacja.clear();
		cout << "-------------" << endl;
		// wyswietlenie wyniku dla SchrageQueue (z kolejk¹)
		wynik = SchrageQueue(iloscZadan, zadania, PI_permutacja);
		for (int i = 0; i < iloscZadan; ++i)
		{
			cout << PI_permutacja[i].numer_zadania << " ";
		}
		cout << "\n" << "#SchrageQueue " << wynik << endl;
		PI_permutacja.clear();
		cout << "-------------" << endl;
		// wyswietlenie wyniku dla Schrage (bez kolejki)
		wynik = Schrage(iloscZadan, zadania, PI_permutacja);
		for (int i = 0; i < iloscZadan; ++i)
		{
			cout << PI_permutacja[i].numer_zadania << " ";
		}
		cout << "\n" << "#Schrage " << wynik << endl;
		PI_permutacja.clear();
		cout << "-------------" << endl;
		// wyswietlenie wyniku dla Schrage PMTN (z kolejk¹)
		wynik = SchrageQueuePMTN(iloscZadan, zadania);
		cout << "#SchrageQueuePMTN " << wynik << endl;
		cout << "-------------" << endl;
		// wyswietlenie wyniku dla Schrage PMTN (bez kolejki)
		wynik = SchragePMTN(iloscZadan, zadania);
		cout << "#SchragePMTN " << wynik << endl;
		cout << "-------------" << endl;
		// wyswietlenie wyniku dla Carlier'a
		wynik = Carlier(iloscZadan, zadania, PI_permutacja);
		for (int i = 0; i < iloscZadan; ++i)
		{
			cout << PI_permutacja[i].numer_zadania << " ";
		}
		cout << "\n" << "#Carlier " << wynik << endl;
		cout << "-------------" << endl;


		//// --- POMIARY---
		//auto start = std::chrono::high_resolution_clock::now(); // pomiar czasu - poczatek
		//for (int i = 0; i < 100; i++)
		//{
		//	RPQ_funkcjaCelu(iloscZadan, zadania,PI_permutacja);
		//}
		//auto end = std::chrono::high_resolution_clock::now(); // pomiar czasu - stop
		//std::chrono::duration<double> elapsed_seconds = end - start; // wynik - czas
		//cout << " " << elapsed_seconds.count() / 100 << " "; // wyswietlenie czasu

		zadania.clear();
	}
	else
		cout << "Nie udalo sie otworzyc pliku";

	return 0;
}
