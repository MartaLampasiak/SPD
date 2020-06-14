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
class Dane
{
public:
	int nr_zadania = 0;
	int nr_maszyny = 0;
	int p_wykonanie = 0;
	int p_end = 0;
};
class DaneW
{
public:
	int nr_zadania = 0;
	int suma = 0;
};
int Cmax(vector< vector<Dane> > zadania)
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
// --- sortowanie rosn¹co po sumie czasow
struct sort_suma
{
	bool operator ()(DaneW zadanie1, DaneW zadanie2)
	{
		if (zadanie1.suma < zadanie2.suma)
		{
			return true;
		}
		else if (zadanie1.suma > zadanie2.suma)
		{
			return false;
		}
		else
			return false;
	}
} SortowanieSumaZadan;
vector<Dane> SzukanieWektoraZadan(vector<vector<Dane>> zbiorN, int nr_zadania)
{
	vector<Dane> znalezioneZadanie;
	for (vector<Dane> i : zbiorN)
	{
		if (i[0].nr_zadania == nr_zadania)
		{
			znalezioneZadanie = i;
			break;
		}
	}
	return znalezioneZadanie;
}
int SzukajIndexZadania(vector<Dane> j, vector<vector<Dane>> zadania)
{
	for (int i = 0; i < zadania.size(); i++)
	{
		if (j[0].nr_zadania == zadania[i][0].nr_zadania)
		{
			return i;
		}
	}
}
int CALCULATE(vector<vector<Dane>> zadania)
{
	int iloscZadan = zadania.size();
	int iloscMaszyn = zadania[0].size();
	vector<vector<Dane>> tempZadCmax(iloscMaszyn, vector<Dane>(iloscZadan));
	for (int i = 0; i < iloscZadan; i++)
	{
		for (int j = 0; j < iloscMaszyn; j++)
		{
			tempZadCmax[j][i] = zadania[i][j];
		}
	}
	return Cmax(tempZadCmax);
}
void Copy(const vector<vector<Dane>> PItemp1, vector<vector<Dane>>& PItemp2)
{
	PItemp2.clear();
	for (vector<Dane> i : PItemp1)
	{
		PItemp2.push_back(i);
	}
}

vector<Dane> PATH(vector<vector<Dane>> zadania)
{
	int iloscZadan = zadania.size();
	int iloscMaszyn = zadania[0].size();
	int start = 0;
	int end = 0;
	vector<vector<Dane>> tempZad(iloscMaszyn, vector<Dane>(iloscZadan));

	vector<vector<int>> vstart(iloscMaszyn, vector<int>(iloscZadan));
	vector<vector<Dane>> vend(iloscMaszyn, vector<Dane>(iloscZadan));
	vector<Dane> zadScKrytyczna;

	for (int i = 0; i < iloscZadan; i++)
	{
		for (int j = 0; j < iloscMaszyn; j++)
		{
			tempZad[j][i] = zadania[i][j];
		}
	}

	vend = tempZad;

	for (int i = 0; i < iloscMaszyn; i++)
	{
		for (int j = 0; j < iloscZadan; j++)
		{
			if (i == 0)
			{
				start = end;
				end = start + tempZad[i][j].p_wykonanie;
			}
			else
			{
				if (j == 0)
				{
					start = vend[i - 1][j].p_wykonanie;
				}
				else
				{
					start = max(vend[i - 1][j].p_end, end);
				}
				end = start + tempZad[i][j].p_wykonanie;
			}
			vstart[i][j] = start;
			vend[i][j].p_end = end;
		}
	}
	//Wybieramy zadanie ze scie¿ki krytycznej
	int i = 0;
	int j = 0;
	zadScKrytyczna.push_back(vend[i][j]);
	while (true)
	{
		if (i < iloscMaszyn - 1 && j < iloscZadan - 1)
		{
			if (vend[i + 1][j].p_end > vend[i][j + 1].p_end)
			{
				zadScKrytyczna.push_back(vend[i + 1][j]);
				i = i + 1;
			}
			else
			{
				zadScKrytyczna.push_back(vend[i][j + 1]);
				j = j + 1;
			}
		}
		else if (i == iloscMaszyn - 1 && j < iloscZadan - 1)
		{
			zadScKrytyczna.push_back(vend[i][j + 1]);
			j = j + 1;
		}
		else if (j == iloscZadan - 1 && i < iloscMaszyn - 1)
		{
			zadScKrytyczna.push_back(vend[i + 1][j]);
			i = i + 1;
		}

		if (i == iloscMaszyn - 1 && j == iloscZadan - 1)
		{
			break;
		}
	}

	return zadScKrytyczna;
}


vector<Dane> SELECT(vector<Dane> j, const vector<vector<Dane>> perm, int wersja)
{
	int index = 0;
	int CmaxAll;
	int CmaxAllTemp;
	vector<Dane> zad;
	vector<vector<Dane>> zadaniaTemp;
	vector<vector<Dane>> zadania;
	vector<Dane> lista;
	zadania = perm;

	index = SzukajIndexZadania(j, zadania);
	zadania.erase(zadania.begin() + index);

	Copy(zadania, zadaniaTemp);
	if (wersja == 1)
	{
		int maxOper = 0;
		int nrZadanaia;
		//Zadanie zawierajace najdluzsza operacje na sciezce krytycznej
		lista = PATH(zadania);
		//Znaldujemy zadanie o najdluzszej operacji
		for (int i = 0; i < lista.size(); i++)
		{
			if (lista[i].p_wykonanie > maxOper)
			{
				maxOper = lista[i].p_wykonanie;
				nrZadanaia = lista[i].nr_zadania;
			}
		}

		zad = SzukanieWektoraZadan(zadania, nrZadanaia);
	}
	else if (wersja == 2)
	{
		//Zadanie zawierajace najwieksza sume operacji...
	}
	else if (wersja == 3)
	{
		//Zadanie zawierajace....
	}
	else if (wersja == 4)
	{
		//Zadanie zawierajace n....
		CmaxAll = CALCULATE(zadania);

		for (int i = 0; i < zadaniaTemp.size(); i++)
		{
			vector<Dane> removedZadanie = zadaniaTemp[i];
			zadaniaTemp.erase(zadaniaTemp.begin() + i);
			CmaxAllTemp = CALCULATE(zadaniaTemp);
			if (CmaxAllTemp <= CmaxAll)
			{
				CmaxAll = CmaxAllTemp;
				zad = removedZadanie;
			}
			Copy(zadania, zadaniaTemp);
		}
	}
	return zad;
}
int NEH_PLUS(vector< vector<Dane> > zadania, vector< DaneW > zbiorW, vector< vector<Dane> >& PI)
{
	int index;
	int iloscZadan = zadania.size();
	int iloscMaszyn = zadania[0].size();
	DaneW najdluzszeZadanie;
	vector<Dane> znalezioneZadania;
	vector<Dane> x;
	vector<vector<Dane>> PItemp1;
	vector<vector<Dane>> PItemp2;
	int k = 0;
	while (zbiorW.size() > 0)
	{
		najdluzszeZadanie = zbiorW.back();
		znalezioneZadania = SzukanieWektoraZadan(zadania, najdluzszeZadanie.nr_zadania);
		PItemp1.push_back(znalezioneZadania);
		PItemp2 = PItemp1;
		for (int l = 0; l < k; l++)
		{
			if (k > 0)
			{
				swap(PItemp2[k - l], PItemp2[k - l - 1]);
			}
			if (CALCULATE(PItemp2) <= CALCULATE(PItemp1))
			{
				PItemp1 = PItemp2;
			}
		}
		if (k > 1)
		{
			x = SELECT(znalezioneZadania, PItemp1, 1);
			index = SzukajIndexZadania(x, PItemp1);     //Szukam pozycji zadania x w wektorze PItemp1 aby go usun¹æ
			PItemp1.erase(PItemp1.begin() + index);     //Usuwam zadanie x z wektora PItemp1
			PItemp1.push_back(x);                       //Wstawiam zadanie x na koniec wektora PItemp1
			PItemp2 = PItemp1;
			//Przestawiam zadanie x i sprawdzam jakie Cmax
			for (int l = 0; l < k; l++)
			{
				if (k > 0)
				{
					swap(PItemp2[k - l], PItemp2[k - l - 1]);
				}
				if (CALCULATE(PItemp2) <= CALCULATE(PItemp1))
				{
					PItemp1 = PItemp2;
				}
			}
		}
		k += 1;
		zbiorW.pop_back();
	}
	PI.clear();
	for (vector<Dane> i : PItemp2)
	{
		PI.push_back(i);
	}
	return CALCULATE(PItemp1);;
}
int NEH(vector< vector<Dane> > zadania, vector< DaneW > zbiorW, vector< vector<Dane> >& PI)
{
	int iloscZadan = zadania.size();
	int iloscMaszyn = zadania[0].size();
	DaneW najdluzszeZadanie;
	vector<Dane> znalezioneZadania;
	vector<vector<Dane>> PItemp1;
	vector<vector<Dane>> PItemp2;
	int k = 0;
	while (zbiorW.size() > 0)
	{
		najdluzszeZadanie = zbiorW.back();
		znalezioneZadania = SzukanieWektoraZadan(zadania, najdluzszeZadanie.nr_zadania);
		PItemp1.push_back(znalezioneZadania);
		PItemp2 = PItemp1;
		for (int l = 0; l < k; l++)
		{
			if (k > 0)
			{
				swap(PItemp2[k - l], PItemp2[k - l - 1]);
			}
			if (CALCULATE(PItemp2) <= CALCULATE(PItemp1))
			{
				PItemp1 = PItemp2;
			}
		}
		k += 1;
		zbiorW.pop_back();
	}
	PI.clear();
	for (vector<Dane> i : PItemp2)
	{
		PI.push_back(i);
	}
	return CALCULATE(PItemp1);
}
int NEH_Init(vector< vector<Dane> > zadania, vector< vector<Dane> >& PI, int neh)
{
	int iloscMaszyn = zadania.size();
	int iloscZadan = zadania[0].size();
	vector< vector<Dane> > zbiorN(iloscZadan, vector<Dane>(iloscMaszyn));
	vector< DaneW > zadaniaW(iloscZadan);
	for (int i = 0; i < iloscMaszyn; i++)
	{
		for (int j = 0; j < iloscZadan; j++)
		{
			zbiorN[j][i] = zadania[i][j];
		}
	}
	//Sumuje zadania dla maszyn
	int suma = 0;
	int nr_zadania;
	for (int i = 0; i < iloscZadan; i++)
	{
		for (int j = 0; j < iloscMaszyn; j++)
		{
			suma += zbiorN[i][j].p_wykonanie;
			nr_zadania = zbiorN[i][j].nr_zadania;
		}
		zadaniaW[i].nr_zadania = nr_zadania;
		zadaniaW[i].suma = suma;
		suma = 0;
	}
	sort(zadaniaW.begin(), zadaniaW.end(), SortowanieSumaZadan);
	if (neh == 1)
	{
		return NEH(zbiorN, zadaniaW, PI);
	}
	else
	{
		return NEH_PLUS(zbiorN, zadaniaW, PI);
	}
}
int main()
{
	int Cmax;
	// --- wczytanie danych z pliku ---
	fstream plik;
	int iloscZadan = 0, iloscMaszyn = 0;
	string nazwa;
	string nazwa_pliku = "FSP/ta001.txt";
	plik.open(nazwa_pliku, ios::in);
	if (plik.good() == true)
	{
		plik >> nazwa;
		plik >> iloscZadan >> iloscMaszyn;
		vector< vector<Dane> > zadania(iloscMaszyn, vector<Dane>(iloscZadan));
		vector< vector<Dane> > PI;
		for (int i = 0; i < iloscZadan; ++i)
		{
			for (int j = 0; j < iloscMaszyn; ++j)
			{
				zadania[j][i].nr_zadania = i;
				plik >> zadania[j][i].nr_maszyny >> zadania[j][i].p_wykonanie;
			}
		}
		plik.close();
		//NEH  
		//auto start = std::chrono::system_clock::now(); // pomiar czasu - poczatek
		//Cmax = NEH_Init(zadania, PI, 1);
		//auto end = std::chrono::system_clock::now(); // pomiar czasu - stop
		//std::chrono::duration<double> elapsed_seconds = end - start; // wynik - czas
		//cout << "--NEH: " << Cmax << endl;
		//cout << " " << elapsed_seconds.count() << " "; // wyswietlenie czasu
		//for (vector<Dane> i : PI)
		//{
		//  cout << i[0].nr_zadania + 1 << "  ";
		//}
		//cout << endl << endl;
		//PI.clear();


		//NEH PLUS
		auto startplus = std::chrono::system_clock::now(); // pomiar czasu - poczatek
		Cmax = NEH_Init(zadania, PI, 2);
		auto endplus = std::chrono::system_clock::now(); // pomiar czasu - stop
		std::chrono::duration<double> elapsed_secondsplus = endplus - startplus; // wynik - czas
		cout << "--NEH+: " << Cmax << endl;
		cout << " " << elapsed_secondsplus.count() << " "; // wyswietlenie czasu
		for (vector<Dane> i : PI)
		{
			cout << i[0].nr_zadania + 1 << "  ";
		}
		cout << endl << endl;
		PI.clear();
		zadania.clear();
	}
	return 0;
}