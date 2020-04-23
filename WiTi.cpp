#include <iostream>
#include <string.h>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <vector>
#include <chrono>
#include <math.h>
#include <limits.h>

using namespace std;

class Dane {
public:
	int numer_zadania = 0;
	int p_wykonanie = 0;
	int w_wspolczynnik_kary = 0;
	int d_deadline = 0;

	void operator = (int nr)
	{
		numer_zadania = nr;
	}

	bool operator < (const Dane& dana1)
	{
		return numer_zadania < dana1.numer_zadania;
	}

	void wypisz()
	{
		cout << numer_zadania + 1 << " " << p_wykonanie << " " << w_wspolczynnik_kary << " " << d_deadline << endl;
	}

	void podajPermutacje()
	{
		cout << numer_zadania + 1 << " ";
	}
};

struct d_sort //sortowanie rosn¹co po D
{
	bool operator ()(Dane zadanie1, Dane zadanie2)
	{
		if (zadanie1.d_deadline < zadanie2.d_deadline)
		{
			return true;
		}
		else if (zadanie1.d_deadline > zadanie2.d_deadline)
		{
			return false;
		}
		else
			return false;
	}
} SortowanieD;

struct zadania_sort //sortowanie rosn¹co po Z
{
	bool operator ()(Dane zadanie1, Dane zadanie2)
	{
		if (zadanie1.numer_zadania < zadanie2.numer_zadania)
		{
			return true;
		}
		else if (zadanie1.numer_zadania > zadanie2.numer_zadania)
		{
			return false;
		}
		else
			return false;
	}
} SortowanieZ;

int WiTi_funkcjaCelu(vector<Dane> zadania, vector<Dane>& PI_permutacja, bool czySort)
{
	int ilosc_zadan = zadania.size();
	vector<int> S_rozpoczecie(ilosc_zadan); // wektor momentow rozpoczecia wykonywania zadan
	vector<int> C_zakonczenie(ilosc_zadan); // wektor momentow zakonczenia wykonywania zadan
	vector<int> T_spoznienie(ilosc_zadan); // spoznienie
	int F_suma_opoznien = 0;

	if (czySort) // sortowanie po D, 1 - sortuj, 0 - nie
	{
		sort(zadania.begin(), zadania.end(), SortowanieD);
	}	

	for (int i = 0; i < zadania.size(); i++)
	{
		PI_permutacja.push_back(zadania[i]);
	}

	S_rozpoczecie[0] = 0;
	C_zakonczenie[0] = S_rozpoczecie[0] + zadania[0].p_wykonanie;
	T_spoznienie[0] = max(C_zakonczenie[0] - zadania[0].d_deadline, 0);
	for (int i = 1; i < ilosc_zadan; i++)
	{
		S_rozpoczecie[i] = C_zakonczenie[i - 1];
		C_zakonczenie[i] = S_rozpoczecie[i] + zadania[i].p_wykonanie;
		T_spoznienie[i] = max(C_zakonczenie[i] - zadania[i].d_deadline, 0);
	}
	for (int i = 0; i < ilosc_zadan; i++)
	{
		F_suma_opoznien += T_spoznienie[i] * zadania[i].w_wspolczynnik_kary;
	}
	S_rozpoczecie.clear();
	C_zakonczenie.clear();
	T_spoznienie.clear();

	return F_suma_opoznien;
}

void swap(Dane& a, Dane& b)
{
	Dane temp = a;
	a = b;
	b = temp;
}

int BruteForce_Rekurencja(vector<Dane> zadania, int ix, vector<Dane>& PI_permutacja, int &koszt)
{
	int ilosc_zadan = zadania.size();
	if (ix < ilosc_zadan - 1)
	{
		for (int i = ix; i < ilosc_zadan; i++)
		{
			swap(zadania[ix], zadania[i]);
			BruteForce_Rekurencja(zadania, ix + 1, PI_permutacja, koszt);
			swap(zadania[ix], zadania[i]);
		}
	}
	else
	{
		int koszt_temp = WiTi_funkcjaCelu(zadania, PI_permutacja, false);
		if (koszt_temp < koszt)
		{
			koszt = koszt_temp;
			PI_permutacja.clear();
			for (int i = 0; i < zadania.size(); i++)
			{
				PI_permutacja.push_back(zadania[i]);
			}
		}
	}
	return koszt;
}

int BruteForce_Rekurencja(vector<Dane> zadania, vector<Dane> &PI_permutacja)
{
	int koszt = INT_MAX;
	int ix = 0;
	return BruteForce_Rekurencja(zadania, ix, PI_permutacja, koszt);
}

int BruteForce_Iteracja(vector<Dane> zadania, vector<Dane>& PI_permutacja)
{
	int koszt = INT_MAX;
	int ilosc_zadan = zadania.size();
	sort(zadania.begin(), zadania.end(), SortowanieZ);

	do {
		int koszt_temp = WiTi_funkcjaCelu(zadania, PI_permutacja, false);
		if (koszt_temp < koszt)
		{
			koszt = koszt_temp;
			PI_permutacja.clear();
			for (int i = 0; i < zadania.size(); i++)
			{
				PI_permutacja.push_back(zadania[i]);
			}
		}
	} while (next_permutation(zadania.begin(), zadania.end()));

	return koszt;
}

int PD_Iteracja(vector<Dane> I)
{
	int ilosc_zadan = I.size();
	const int ilosc_permutacji = static_cast<int>(pow(2.0, static_cast<double>(ilosc_zadan)));
	vector<int> F(ilosc_permutacji, INT_MAX);
	F[0] = 0;
	int C_zakonczenia = 0;

	for (int i = 1; i < ilosc_permutacji; ++i)
	{
		C_zakonczenia = 0;
		for (int j = 1, k = 0; k < ilosc_zadan; j <<= 1, ++k)
		{
			if (i & j) // iloczyn bitowy
			{
				C_zakonczenia += I[k].p_wykonanie; // zlicza C, czasy zakonczen zadan
			}
		}
		for (int j = 1, k = 0; k < ilosc_zadan; j <<= 1, ++k)
		{
			if (i & j)
			{
				// wyliczenie funkcji celu
				F[i] = min(F[i], max(C_zakonczenia - I[k].d_deadline, 0) * I[k].w_wspolczynnik_kary + F[i & (~j)]);
			}
		}
	}
	return F.back(); // zwrocenie Fmin, najmniejszej wartosci kar
}

int PD_Rekurencja(vector<Dane> I, vector<int>& F, int ilosc_permutacji)
{
	int ilosc_zadan = I.size();
	int F_min = INT_MAX;
	int C_zakonczenia = 0, bit = 0;

	for (int i = 0; i < ilosc_zadan; ++i)
	{
		if (ilosc_permutacji & (1 << i))
		{
			bit = ilosc_permutacji & (~(1 << i));
			C_zakonczenia = 0;
			for (int j = 0; j < ilosc_zadan; ++j)
			{
				if (ilosc_permutacji & (1 << j))
				{
					C_zakonczenia += I[j].p_wykonanie; // zlicza C, czasy zakonczen zadan
				}
			}

			if (ilosc_permutacji == 0)
			{
				return 0; // stop dla rekurencji
			}
			if (F[bit] == -1) // jezeli -1, czyli niepoliczone
			{
				F[bit] = PD_Rekurencja(I, F, bit);
			}
			F_min = min(F_min, max(C_zakonczenia - I[i].d_deadline, 0) * I[i].w_wspolczynnik_kary + F[bit]);
		}
	}
	return F_min;
}

int PD_Rekurencja(vector<Dane> I)
{
	int ilosc_zadan = I.size();
	const int ilosc_permutacji = static_cast<int>(pow(2.0, static_cast<double>(ilosc_zadan))) - 1;
	vector<int> F(ilosc_permutacji, -1); // wypelnienie wartoscia -1
	F[0] = 0;
	return PD_Rekurencja(I, F, ilosc_permutacji);
}


int main()
{	
	fstream plik;
	int iloscZadan = 0, iloscDanych = 0;

	plik.open("C:\\Users\\Widmo\\source\\repos\\Problem_WiTi\\Problem_WiTi\\Data\\data_test.txt", ios::in); //zmiana œcie¿ki do pliku
	if (plik.good() == true)
	{
		plik >> iloscZadan >> iloscDanych;
		vector<Dane> zadania(iloscZadan);
		vector<Dane> PI_permutacja;

		for (int i = 0; i < iloscZadan; ++i)
		{
			zadania[i].numer_zadania = i;
			plik >> zadania[i].p_wykonanie >> zadania[i].w_wspolczynnik_kary >> zadania[i].d_deadline;
		}
		plik.close();


		/* jesli chcemy permutacje - odkomentowaæ for*/
		cout << "\n" << "#WiTi: "  << WiTi_funkcjaCelu(zadania, PI_permutacja, true) << endl;
		/*for (int i = 0; i < iloscZadan; ++i)
		{
			cout << PI_permutacja[i].numer_zadania << " ";
		}*/
		PI_permutacja.clear();
		cout << "\n-------------" << endl;
		cout << "#Brute_Iteracja: " << BruteForce_Iteracja(zadania, PI_permutacja) << endl;
		/*for (int i = 0; i < iloscZadan; ++i)
		{
			cout << PI_permutacja[i].numer_zadania << " ";
		}*/
		PI_permutacja.clear();
		cout << "\n" << "-------------\n";
		cout << "#Brute_Rekurencja: " << BruteForce_Rekurencja(zadania, PI_permutacja) << endl;
		/*for (int i = 0; i < iloscZadan; ++i)
		{
			cout << PI_permutacja[i].numer_zadania << " ";
		}*/
		PI_permutacja.clear();
		cout << "\n" << "------------- \n";
		cout << "#PD_Iteracja: " << PD_Iteracja(zadania) << endl;
		cout << "\n" << "------------- \n";
		cout << "#PD_Rekurencja: " << PD_Rekurencja(zadania) << endl;



		// -------- INSTRUKCJE DO MIERZENIA CZASU NA POTEM DO SPRAWKA ------- //
		//auto start = std::chrono::system_clock::now(); // pomiar czasu - poczatek
		//BruteForce_Rekurencja(zadania, PI_permutacja);
		//auto end = std::chrono::system_clock::now(); // pomiar czasu - stop
		//std::chrono::duration<double> elapsed_seconds = end - start; // wynik - czas
		//cout << " " << elapsed_seconds.count() << " "; // wyswietlenie czasu

		zadania.clear();
	}
	else
		cout << "Nie udalo sie otworzyc pliku";

	return 0;
}

