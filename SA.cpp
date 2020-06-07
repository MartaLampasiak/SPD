#include <iostream>
#include <string.h>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <vector>
#include <list>
#include <chrono>
#include <queue>
#include <math.h>

using namespace std;

// --- klasa obiektów typu Dane, posiadaj¹ca pola dla problemu FP ---
class Dane {
public:
	int nr_zadania = 0;
	int nr_maszyny = 0;
	int p_wykonanie = 0;
};

// ------ funkcja celu dla problemu FSP - szukane kryterium optymalizacyjne ---
// ----------- funkcja zwraca - Cmax - dla podanej permutacji zadan -----------
int FSP_funkcjaCelu(vector< vector<Dane> > zadania)
{
	int iloscMaszyn = zadania.size();
	int iloscZadan = zadania[0].size();
	vector< vector<int> > S(iloscMaszyn, vector<int>(iloscZadan, 0)); // wektor momentow rozpoczecia wykonywania zadan
	vector< vector<int> > C(iloscMaszyn, vector<int>(iloscZadan, 0)); // wektor momentow zakonczenia wykonywania zadan
	int s_rozpoczecie = 0;
	int c_zakonczenie = 0;

	for (int i = 0; i < iloscMaszyn; ++i)
	{
		for (int j = 0; j < iloscZadan; ++j)
		{
			if (i == 0)
			{
				s_rozpoczecie = c_zakonczenie;
				c_zakonczenie = s_rozpoczecie + zadania[i][j].p_wykonanie;
			}
			else
			{
				if (j == 0)
				{
					s_rozpoczecie = C[i - 1][j];
				}
				else
				{
					s_rozpoczecie = max(C[i - 1][j], c_zakonczenie);
				}
				c_zakonczenie = s_rozpoczecie + zadania[i][j].p_wykonanie;

			}
			S[i][j] = s_rozpoczecie;
			C[i][j] = c_zakonczenie;
		}
	}
	return C[iloscMaszyn - 1][iloscZadan - 1]; // zwraca wartosci Cmax
}


//  ------- funkcja sluzaca do losowej liczby typu 'double' --------------
double RandomDouble(double lower_bound, double upper_bound)
{
	double random_d = (double)rand() / RAND_MAX;
	return lower_bound + random_d * (upper_bound - lower_bound);
}

// --------------- algorytm symulowanego wyzarzania (SA) -----------------
// przyjmowane parametry:
// - T - temperatura poczatkowa
// - T_end - temperatura koncowa
// - ilosc_epok - mo¿liwe: "N" (ilosc zadañ), 
//						   "sqrtN" (pierwiastek z ilosci zadan), 
//						   "N^2" (kwadrat ilosci zadan)
// - sasiedztwo - wybor: "swap" lub "adj" (adjacent swap)
// - ochladzanie - wybor: "-" (ochladzanie liniowe) lub "*"(geometryczne)
// - x - parametr dla ochladzania liniowego
// - alpha - parametr dla ochladzania geometrycznego
// ----------------------------------------------------------------------
int SA(vector< vector<Dane> > zadania, double T, double T_end, string ilosc_epok, string sasiedztwo, string ochladzanie, double x, double alpha)
{
	int iloscZadan = zadania[0].size();
	int i = 0, j = 0, delta_Cmax = 0;
	double r = 0.0, p = 0.0;
	int Cmax_poczatkowe = FSP_funkcjaCelu(zadania); // wartosc dla permutacji naturalnej
	int Cmax_nowe = 0;
	int Cmax_najlepsze = Cmax_poczatkowe;
	int L = 0;

	// wybór ilosc epok
	if (ilosc_epok == "N") // ochladzanie liniowe
	{
		L = zadania[0].size();
	}
	else if (ilosc_epok == "sqrtN") // ochladzanie geometryczne
	{
		L = static_cast<int>(sqrt(zadania[0].size()));
	}
	else if (ilosc_epok == "N^2")
	{
		L = pow(zadania[0].size(), 2);
	}
	else
	{
		return L;
	}

	while (T > T_end)
	{

		for (int k = 0; k < L; ++k)
		{
			i = rand() % iloscZadan; // losowanie indeksow
			j = rand() % iloscZadan;

			if (sasiedztwo == "swap") // sasiedztwo 'swap'
			{
				for (uint32_t m = 0; m < zadania.size(); ++m)
				{
					swap(zadania[m][i], zadania[m][j]);
				}
			}
			else if(sasiedztwo=="adj") // sasiedztwo 'adjacent swap'
			{
				for (uint32_t m = 0; m < zadania.size(); ++m)
				{
					for (int z = i; z < iloscZadan - i - 1; ++z)
					{
						swap(zadania[m][z], zadania[m][z + 1]);
					}
				}
			}
			Cmax_nowe = FSP_funkcjaCelu(zadania);

			if (Cmax_nowe > Cmax_poczatkowe)
			{
				r = RandomDouble(0, 1);
				delta_Cmax = Cmax_poczatkowe - Cmax_nowe;
				p = exp(delta_Cmax / T); // prawdopodobienstwo

				if (r >= p)
				{
					Cmax_nowe = Cmax_poczatkowe;
				}
			}
			Cmax_poczatkowe = Cmax_nowe;

			if (Cmax_poczatkowe < Cmax_najlepsze)
			{
				Cmax_najlepsze = Cmax_poczatkowe;
			}
		}
		if (ochladzanie == "-") // ochladzanie liniowe
		{
			T = T - x;
		}
		else if(ochladzanie=="*") // ochladzanie geometryczne
		{
			T = T * alpha;			
		}
	}
	return Cmax_najlepsze;
}


int main()
{
	// --- wczytanie danych z pliku --- //
	fstream plik;
	int iloscZadan = 0, iloscMaszyn = 0;

	string nazwa_pliku = "FSP/ta001.txt";
	string nazwa;
	plik.open(nazwa_pliku, ios::in);
	if (plik.good() == true)
	{
		plik >> nazwa >> iloscZadan >> iloscMaszyn;
		vector< vector<Dane> > zadania(iloscMaszyn, vector<Dane>(iloscZadan));
		vector< vector<Dane> > PI(iloscMaszyn, vector<Dane>(iloscZadan));
		vector<int> PI_Brute;

		for (int i = 0; i < iloscZadan; ++i)
		{
			for (int j = 0; j < iloscMaszyn; ++j)
			{
				zadania[j][i].nr_zadania = i;
				plik >> zadania[j][i].nr_maszyny >> zadania[j][i].p_wykonanie;
			}
		}
		plik.close();


		//cout << "FSP funkcja celu: " << FSP_funkcjaCelu(zadania) << endl;

		cout << "SA: " << SA(zadania, 1000, 0.0001, "N", "swap", "-", 0.1, 0.9) << endl;
		cout << "SA: " << SA(zadania, 1000, 0.0001, "N", "swap", "*", 0.1, 0.97) << endl;
		cout << "SA: " << SA(zadania, 1000, 0.0001, "N", "swap", "*", 0.1, 0.97) << endl;
		cout << "SA: " << SA(zadania, 1000, 0.0001, "N", "swap", "*", 0.1, 0.97) << endl;

	}
	else
		cout << "Nie udalo sie otworzyc pliku";

	return 0;
}