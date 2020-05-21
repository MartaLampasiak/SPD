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

// --- klasa obiektów typu Dane, posiadaj¹ca pola dla problemu FSP ---
class Dane {
public:
	int nr_zadania = 0;
	int nr_maszyny = 0;
	int p_wykonanie = 0;
};

class DaneFP2 {
public:
	int nr_zadania = 0;
	int p_wykonanie1 = 0;
	int p_wykonanie2 = 0;
};

// --- funkcja celu dla problemu FSP - szukane kryterium optymalizacyjne ---
// --- funkcja zwraca - Cmax - dla permutacji naturalnej ---
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

// --- sortowanie malej¹co po czasie trwania zadania na maszynie 1 ---
struct zadania_sort_czas_maszyna1
{
	bool operator ()(DaneFP2 zadanie1, DaneFP2 zadanie2)
	{
		if (zadanie1.p_wykonanie1 > zadanie2.p_wykonanie1)
		{
			return true;
		}
		else if (zadanie1.p_wykonanie1 < zadanie2.p_wykonanie1)
		{
			return false;
		}
		else
			return false;
	}
} SortowanieCzasZadania1;

// --- sortowanie malej¹co po czasie trwania zadania na maszynie 2 ---
struct zadania_sort__czas_maszyna2 //sortowanie malej¹co po czasie trwania zadania
{
	bool operator ()(DaneFP2 zadanie1, DaneFP2 zadanie2)
	{
		if (zadanie1.p_wykonanie2 > zadanie2.p_wykonanie2)
		{
			return true;
		}
		else if (zadanie1.p_wykonanie2 < zadanie2.p_wykonanie2)
		{
			return false;
		}
		else
			return false;
	}
} SortowanieCzasZadania2;

// --- sortowanie rosn¹co po numerze zadania
struct zadania_sort_nr_zad
{
	bool operator ()(Dane zadanie1, Dane zadanie2)
	{
		if (zadanie1.nr_zadania < zadanie2.nr_zadania)
		{
			return true;
		}
		else if (zadanie1.nr_zadania > zadanie2.nr_zadania)
		{
			return false;
		}
		else
			return false;
	}
} SortowanieNrZadania;


int BruteForce_Iteracja(vector< vector<Dane> > zadania, vector<int>& PI)
{
	int koszt = INT_MAX;
	int iloscMaszyn = zadania.size();
	int iloscZadan = zadania[0].size();
	do
	{
		int koszt_temp = FSP_funkcjaCelu(zadania);
		if (koszt_temp < koszt)
		{
			koszt = koszt_temp;
			PI.clear();
			for (int i = 0; i < iloscZadan; ++i)
			{
				PI.push_back(zadania[0][i].nr_zadania);
			}
		}
		for (int i = 1; i < iloscMaszyn; ++i)
		{
			next_permutation(zadania[i].begin(), zadania[i].end(), SortowanieNrZadania);
		}
	} while (next_permutation(zadania[0].begin(), zadania[0].end(), SortowanieNrZadania));
	return koszt;
}

int Johnson(vector< vector<Dane> > zadania, vector< vector<Dane> >& PI)
{
	int Cmax = 0;
	int indeks_lewy = 0; //l
	int indeks_prawy = zadania[0].size() - 1; //k
	int iloscMaszyn = zadania.size();
	DaneFP2 min_maszyna1;
	vector <DaneFP2> zadaniaFP2(indeks_prawy + 1);

	// wpisanie do wektora zadaniaFP2 z typem danych DaneFP2
	for (int j = 0; j < indeks_prawy + 1; ++j)
	{
		zadaniaFP2[j].nr_zadania = j;
		zadaniaFP2[j].p_wykonanie1 = zadania[0][j].p_wykonanie;
		zadaniaFP2[j].p_wykonanie2 = zadania[1][j].p_wykonanie;
	}

	do
	{
		sort(zadaniaFP2.begin(), zadaniaFP2.end(), SortowanieCzasZadania1); // sortowanie po czasie zad na maszynie 1
		min_maszyna1 = zadaniaFP2.back(); // sprawdzenie najmniejszego czasu na maszynie 1
		sort(zadaniaFP2.begin(), zadaniaFP2.end(), SortowanieCzasZadania2); // sortuje po czasie zad na maszynie 2

		if (min_maszyna1.p_wykonanie1 > zadaniaFP2.back().p_wykonanie2) // gdy krotszy czas na maszynie 2, dodaje z prawej
		{
			PI[0][indeks_prawy].p_wykonanie = zadaniaFP2.back().p_wykonanie1;
			PI[0][indeks_prawy].nr_zadania = zadaniaFP2.back().nr_zadania;
			PI[1][indeks_prawy].p_wykonanie = zadaniaFP2.back().p_wykonanie2;
			PI[1][indeks_prawy].nr_zadania = zadaniaFP2.back().nr_zadania;

			zadaniaFP2.pop_back();
			indeks_prawy--; // przesuniecie indeksu
		}
		else // gdy krotszy czas na maszynie 1, dodaje z lewej
		{
			sort(zadaniaFP2.begin(), zadaniaFP2.end(), SortowanieCzasZadania1); // sort po czasie zadan, maszyna 1

			PI[0][indeks_lewy].p_wykonanie = zadaniaFP2.back().p_wykonanie1;
			PI[0][indeks_lewy].nr_zadania = zadaniaFP2.back().nr_zadania;
			PI[1][indeks_lewy].p_wykonanie = zadaniaFP2.back().p_wykonanie2;
			PI[1][indeks_lewy].nr_zadania = zadaniaFP2.back().nr_zadania;

			zadaniaFP2.pop_back();
			indeks_lewy++; // przesunecie indeksu
		}
	} while (!zadaniaFP2.empty());

	Cmax = FSP_funkcjaCelu(PI); // obliczenie Cmax
	return Cmax; // zwrocenie Cmax
}


/*************************************** BRANCH AND BOUND *********************************************/

//zmienne zdefiniowane globalnie dla funkcji rekurencyjnej BnB
int UB = 0;
vector<vector<Dane>> ZadaniaAll;
vector<vector<Dane>> PICmax;

//wyszukanie zadania, które zostanie usuniête ze zbioru zadañ nieuszeregowanych zbiorN
int FindElementIndeks(vector<vector<Dane>> zbiorN, vector<Dane> J)
{
	
	for (size_t i = 0; i < zbiorN.size(); i++)
	{
		if (zbiorN[i][0].nr_zadania == J[0].nr_zadania)
		{
			return i;
		}
	}
	return 0;
}

//na podstawie metody FSP_funkcjacelu, wyszukanie wektora C_(i,x) dla obliczenia LB
vector<int> FSP_Vektor(vector<vector<Dane>> PI, int iloscZadan, int iloscMaszyn)
{
	int s_rozpoczecie = 0;
	int c_zakonczenie = 0;
	vector<vector<Dane>> PIPom(iloscMaszyn, vector<Dane>(iloscZadan));
	vector< vector<int> > S(iloscMaszyn, vector<int>(iloscZadan, 0)); // wektor momentow rozpoczecia wykonywania zadan
	vector< vector<int> > C(iloscMaszyn, vector<int>(iloscZadan, 0)); // wektor momentow zakonczenia wykonywania zadan

	vector<int> Cx(iloscMaszyn);

	for (int i = 0; i < iloscZadan; i++)
	{
		for (int j = 0; j < iloscMaszyn; j++)
		{
			PIPom[j][i] = PI[i][j];
		}
	}

	for (int i = 0; i < iloscMaszyn; ++i)
	{
		for (int j = 0; j < iloscZadan; ++j)
		{
			if (i == 0)
			{
				s_rozpoczecie = c_zakonczenie;
				c_zakonczenie = s_rozpoczecie + PIPom[i][j].p_wykonanie;
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
				c_zakonczenie = s_rozpoczecie + PIPom[i][j].p_wykonanie;

			}
			S[i][j] = s_rozpoczecie;
			C[i][j] = c_zakonczenie;
		}
	}

	for (int i = 0; i < iloscMaszyn; i++)
	{
		Cx[i] = C[i][iloscZadan - 1];
	}
	return Cx;
}

//Wyznaczenie Cmax dla metody BnB
int CALCULATE(vector<vector<Dane>> PI)
{
	int Cmax = 0;
	int iloscZadan = PI.size();
	int iloscMaszyn = PI[0].size();
	vector<vector<Dane>> PIPom(iloscMaszyn, vector<Dane>(iloscZadan));

	for (int i = 0; i < iloscZadan; i++)
	{
		for (int j = 0; j < iloscMaszyn; j++)
		{
			PIPom[j][i] = PI[i][j];
		}
	}

	Cmax = FSP_funkcjaCelu(PIPom);
	return Cmax;
}

//Wyznaczenie LB
int BOUND(vector<vector<Dane>> PI, vector<vector<Dane>> zbiorN, int metoda)
{
	int LB = 0;
	int pmin;
	//int sum;
	vector<int> C;
	int iloscZadan = PI.size();
	int iloscMaszyn = PI[0].size();
	int suma = 0;
	vector<int> P(iloscMaszyn);
	vector<int> CP(iloscMaszyn);

	C = FSP_Vektor(PI, iloscZadan, iloscMaszyn);

	//Wspolny cz³on
	for (int i = 0; i < iloscMaszyn; i++)
	{
		suma = 0;
		for (size_t j = 0; j < zbiorN.size(); j++)
		{
			suma += zbiorN[j][i].p_wykonanie;
		}
		P[i] = suma;
		CP[i] = C[i] + P[i];
	}

	switch (metoda)
	{
	case 1:
		for (int i = 0; i < iloscMaszyn; i++)
		{
			if (CP[i] > LB)
			{
				LB = CP[i];
			}
		}
		break;
	case 2:
		for (int i = 0; i < iloscMaszyn - 1; i++)
		{
			pmin = INT_MAX;
			for (size_t j = 0; j < ZadaniaAll[0].size(); j++)
			{
				if (ZadaniaAll[i + 1][j].p_wykonanie < pmin)
				{
					pmin = ZadaniaAll[i + 1][j].p_wykonanie;
				}
			}
			CP[i] += pmin;
		}
		for (int i = 0; i < iloscMaszyn; i++)
		{
			if (CP[i] > LB)
			{
				LB = CP[i];
			}
		}
		break;
	case 3:
		for (int i = 0; i < iloscMaszyn - 1; i++)
		{
			pmin = INT_MAX;
			for (size_t j = 0; j < zbiorN.size(); j++)
			{
				if (zbiorN[j][i + 1].p_wykonanie < pmin)
				{
					pmin = zbiorN[j][i + 1].p_wykonanie;
				}
			}
			CP[i] += pmin;
		}
		for (int i = 0; i < iloscMaszyn; i++)
		{
			if (CP[i] > LB)
			{
				LB = CP[i];
			}
		}
		break;
	}
	return LB;
}

void BranchandBound(vector<Dane> J, vector<vector<Dane>> zbiorN, vector<vector<Dane>> PI)
{
	int Cmax = 0;
	int LB = 0;
	vector<vector<Dane>> PIp;

	for (vector<Dane> v : PI)
	{
		PIp.push_back(v);
	}
	PIp.push_back(J);
	zbiorN.erase(zbiorN.begin() + FindElementIndeks(zbiorN, J));

	if (zbiorN.size() != 0)
	{
		LB = BOUND(PIp, zbiorN, 3);       //trzeci parametr to metoda liczenia LB (wartoœci od 1 do 3)
		if (LB <= UB)
		{
			for (vector<Dane> j : zbiorN)
			{
				BranchandBound(j, zbiorN, PIp);
			}
		}
	}
	else
	{
		Cmax = CALCULATE(PIp);
		if (Cmax < UB)
		{
			UB = Cmax;
			PICmax.clear();
			for (vector<Dane> v : PIp) //zapamiêtanie permutacji
			{
				PICmax.push_back(v);
			}
		}
	}
}

void BranchandBoundInit(vector<vector<Dane>> zadania, int iloscZadan, int iloscMaszyn)
{
	vector< vector<Dane> > zbiorN(iloscZadan, vector<Dane>(iloscMaszyn));
	vector< vector<Dane> > PI;
	vector<Dane> zad(iloscMaszyn);

	for (int i = 0; i < iloscMaszyn; i++)
	{
		for (int j = 0; j < iloscZadan; j++)
		{
			zbiorN[j][i] = zadania[i][j];
		}
	}

	for (vector<Dane> j : zbiorN)
	{
		BranchandBound(j, zbiorN, PI);
	}
}

vector< vector<Dane> > RandomOrder(vector<vector<Dane>> zadania)
{	
	int silnia = 1;	
	int iloscMaszyn = zadania.size();
	int iloscZadan = zadania[0].size();	

	for (int i = 1; i <= iloscZadan; i++)
	{
		silnia *= i;
	}
	srand((unsigned int)time(NULL));
	int iloscZmian = rand() % silnia + 1;

	for (int i = 0; i < iloscZmian; ++i)
	{
		for (int j = 0; j < iloscMaszyn; ++j)
		{
			next_permutation(zadania[j].begin(), zadania[j].end(), SortowanieNrZadania);
		}
	}
	return zadania;
}

int main()
{
	// --- wczytanie danych z pliku --- //
	fstream plik;
	int iloscZadan = 0, iloscMaszyn = 0;

	string nazwa_pliku = "data001.txt";
	plik.open(nazwa_pliku, ios::in);
	if (plik.good() == true)
	{
		plik >> iloscZadan >> iloscMaszyn;
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

		cout << "FSP funkcja celu: " << FSP_funkcjaCelu(zadania) << endl;
		// -------------------------------------------------------------------------------
		cout << "Brute Force: " << BruteForce_Iteracja(zadania, PI_Brute) << endl;
		for (int i = 0; i < iloscZadan; ++i)
		{
			cout << PI_Brute[i] + 1 << ' ';
		}
		cout << endl;
		PI_Brute.clear();
		// -------------------------------------------------------------------------------
		cout << "Johnson: " << Johnson(zadania, PI) << endl;
		for (int i = 0; i < iloscZadan; ++i)
		{
			cout << PI[0][i].nr_zadania + 1 << ' ';
		}
		cout << endl;
		PI.clear();
		/*************  Branch and Bound  **************/
		// ------------- wybór górnego ogranicznenia -------------------------------------
		UB = INT_MAX;                                                  //UB dla INT_MAX - UB1

		//UB = FSP_funkcjaCelu(zadania);                               //UB dla permutacji naturalnej - UB2

		//vector< vector<Dane> > zadaniaDoUB = RandomOrder(zadania);   // losowe Cmax - UB3
		//UB = FSP_funkcjaCelu(zadaniaDoUB);
		// ------------------------------------------------------------------------------
		for (vector<Dane> v : zadania)
		{
			ZadaniaAll.push_back(v);      //do wyznaczania LB2
		}
		cout << "Branch and Bound ograniczenie: " << UB << endl;
		BranchandBoundInit(zadania, iloscZadan, iloscMaszyn);
		auto end = std::chrono::system_clock::now(); // pomiar czasu - stop
		cout << "Branch and Bound: " << UB << endl;
		if (!(PICmax.size() == 0))
		{
			for (int i = 0; i < iloscZadan; ++i)
			{
				cout << PICmax[i][0].nr_zadania + 1 << ' ';
			}
		}
		zadania.clear();
		ZadaniaAll.clear();
		PICmax.clear();
	}
	else
		cout << "Nie udalo sie otworzyc pliku";

	return 0;
}