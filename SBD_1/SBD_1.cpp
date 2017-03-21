#include <fstream>
#include <conio.h>
#include <ctime>

using namespace std;

#define LICZBA_LICZB_NA_STRONIE 20 // 20 liczb na strone, a wiec 10 rekordow - mozna to jakos inaczej nazwac, liczba liczb?
#define DLUGOSC_LICZBY 11 // 4 cyfry po lewej i prawej stronie kropki, kropka, minus i null

struct liczbaDostepowDoPliku {
	int odczyt = 0;
	int zapis = 0;
};


void floatToCharBuffer(float buforLiczbowy[LICZBA_LICZB_NA_STRONIE], char buforZnakowy[LICZBA_LICZB_NA_STRONIE][DLUGOSC_LICZBY], int liczbaLiczbDoZapisania);
void charToFloatBuffer(float buforLiczbowy[LICZBA_LICZB_NA_STRONIE], char buforZnakowy[LICZBA_LICZB_NA_STRONIE][DLUGOSC_LICZBY], int liczbaLiczbDoZapisania);
void keyboardInput(float buforLiczbowy[LICZBA_LICZB_NA_STRONIE], char buforZnakowy[LICZBA_LICZB_NA_STRONIE][DLUGOSC_LICZBY], int liczbaLiczbDoZapisania);
void randomInput(float buforLiczbowy[LICZBA_LICZB_NA_STRONIE], char buforZnakowy[LICZBA_LICZB_NA_STRONIE][DLUGOSC_LICZBY], int liczbaLiczbDoZapisania);
void chooseInputMode(float buforLiczbowy[LICZBA_LICZB_NA_STRONIE], char buforZnakowy[LICZBA_LICZB_NA_STRONIE][DLUGOSC_LICZBY]);
float calculateDistance(float a, float b);
int compare(float a, float b, float c, float d);
void distributeRuns(float buforLiczbowy[LICZBA_LICZB_NA_STRONIE], float buforTasmy[LICZBA_LICZB_NA_STRONIE], int &indeksBuforaLiczbowego, int &indeksTasmy, int &aktualnaTasma, int liczbaRekordowDoOdczytania, float &aktualnyRekordX, float &aktualnyRekordY, liczbaDostepowDoPliku &licznik);
void findRuns(float buforLiczbowy[LICZBA_LICZB_NA_STRONIE], int liczbaRekordowDoOdczytania, int &indeksTasmy1, int &indeksTasmy2, float buforTasmy1[LICZBA_LICZB_NA_STRONIE], float buforTasmy2[LICZBA_LICZB_NA_STRONIE], int &aktualnaTasma, float &aktualnyRekordX, float &aktualnyRekordY, liczbaDostepowDoPliku &licznik);
void readAndSort(float buforLiczbowy[LICZBA_LICZB_NA_STRONIE], char buforZnakowy[LICZBA_LICZB_NA_STRONIE][DLUGOSC_LICZBY]);
void saveBufferToFile(char buforZnakowy[LICZBA_LICZB_NA_STRONIE][DLUGOSC_LICZBY], int liczbaLiczbDoZapisania, char* nazwaPliku);
bool merge(FILE *plik, liczbaDostepowDoPliku &licznik);
void writeToBuffer(float buforWyjsciowy[LICZBA_LICZB_NA_STRONIE], int &indeksBuforaWyjsciowego, float bufor[LICZBA_LICZB_NA_STRONIE], int &indeksBufora, liczbaDostepowDoPliku &licznik, FILE* plik);
void readPage(char buforZnakowy[LICZBA_LICZB_NA_STRONIE][DLUGOSC_LICZBY], float buforLiczbowy[LICZBA_LICZB_NA_STRONIE], int &liczbaOdczytanych, liczbaDostepowDoPliku &licznik, int &indeksBufora, FILE *tasma);
void displayFile(char *nazwaPliku);


int main() {

	char buforWejsciowyZnakowy[LICZBA_LICZB_NA_STRONIE][DLUGOSC_LICZBY]; // odpowiednik jednej strony 
	float buforWejsciowyLiczbowy[LICZBA_LICZB_NA_STRONIE]; // wersja float bufora znakowego - dla wygody obliczen


	chooseInputMode(buforWejsciowyLiczbowy, buforWejsciowyZnakowy); // wybor sposobu wczytywania danych do programu	

	readAndSort(buforWejsciowyLiczbowy, buforWejsciowyZnakowy);

	system("pause");
	return 0;
}

void chooseInputMode(float buforLiczbowy[LICZBA_LICZB_NA_STRONIE], char buforZnakowy[LICZBA_LICZB_NA_STRONIE][DLUGOSC_LICZBY]) {

	printf("Wybierz sposob wprowadzenia danych\n1 - z pliku\n2 - recznie\n3 - losowo\n");
	int option;
	scanf("%d", &option);
	int liczbaRekordow;

	switch (option) {
	case 1:
		// w tym wypadku nie trzeba nic robic, poniewaz obsluga pliku zajmuje sie pozniej, odczytujac z niego stronami
		break;
	case 2:
		printf("Wprowadz liczbe rekordow (par dwoch liczb) \n");
		scanf("%d", &liczbaRekordow);
		remove("wejscie.bin"); // usuniecie istniejacego pliku wejsciowego, aby zrobic miejsce na nowotworzony
		keyboardInput(buforLiczbowy, buforZnakowy, liczbaRekordow * 2); // reczne wprowadzenie rekordow
		break;
	case 3:
		printf("Wprowadz liczbe rekordow (par dwoch liczb) \n");
		scanf("%d", &liczbaRekordow);
		remove("wejscie.bin");
		randomInput(buforLiczbowy, buforZnakowy, liczbaRekordow * 2); // losowe tworzenie rekordow
		break;
	}

	if (option > 1)
		displayFile("wejscie.bin");
}

void keyboardInput(float buforLiczbowy[LICZBA_LICZB_NA_STRONIE], char buforZnakowy[LICZBA_LICZB_NA_STRONIE][DLUGOSC_LICZBY], int liczbaLiczbDoZapisania) {

	printf("Podaj rekordy:\n");
	int licznikStrony = 0;
	for (int i = 0; i < liczbaLiczbDoZapisania; i++) {
		scanf("%f", &buforLiczbowy[licznikStrony++]); // wpisz liczbê do strony i zwiêksz wartoœæ licznika aby wskazywa³ ile elementów jest ju¿ w œrodku
		if (licznikStrony == LICZBA_LICZB_NA_STRONIE) { // jeœli strona siê zape³ni
			floatToCharBuffer(buforLiczbowy, buforZnakowy, licznikStrony); // zamiana bufora float na bufor znakowy do póŸniejszego zapisu do pliku. licznik strony mówi ile rekordów nale¿y zapisaæ
			saveBufferToFile(buforZnakowy, licznikStrony, "wejscie.bin"); // zapisanie bufora char do pliku
			licznikStrony = 0;
		}
	}
	if (licznikStrony != 0) { // sprawdzenie czy jest jakaœ niepe³na strona w buforze
		floatToCharBuffer(buforLiczbowy, buforZnakowy, licznikStrony);
		saveBufferToFile(buforZnakowy, licznikStrony, "wejscie.bin"); // zapisanie bufora char do pliku
	}

}

void randomInput(float buforLiczbowy[LICZBA_LICZB_NA_STRONIE], char buforZnakowy[LICZBA_LICZB_NA_STRONIE][DLUGOSC_LICZBY], int liczbaLiczbDoZapisania) { // wygenerowanie losowych danych do bufora liczbowego

	srand(time(NULL));
	int a = -9999; // poczatek przedzialu
	int b = 9999; // koniec przedzialu
	int licznikStrony = 0;
	for (int i = 0; i < liczbaLiczbDoZapisania; i++) {
		buforLiczbowy[licznikStrony++] = ((float)rand() / (float)RAND_MAX) * (b - (a)) + (a); // wpisz liczbê do strony i zwiêksz wartoœæ licznika aby wskazywa³ ile elementów jest ju¿ w œrodku
		if (licznikStrony == LICZBA_LICZB_NA_STRONIE) { // jeœli strona siê zape³ni
			floatToCharBuffer(buforLiczbowy, buforZnakowy, licznikStrony); // zamiana bufora float na bufor znakowy do póŸniejszego zapisu do pliku. licznik strony mówi ile rekordów nale¿y zapisaæ
			saveBufferToFile(buforZnakowy, licznikStrony, "wejscie.bin"); // zapisanie bufora char do pliku
			licznikStrony = 0;
		}
	}
	if (licznikStrony != 0) { // sprawdzenie czy jest jakaœ niepe³na strona w buforze
		floatToCharBuffer(buforLiczbowy, buforZnakowy, licznikStrony);
		saveBufferToFile(buforZnakowy, licznikStrony, "wejscie.bin"); // zapisanie bufora char do pliku
	}

}

void floatToCharBuffer(float buforLiczbowy[LICZBA_LICZB_NA_STRONIE], char buforZnakowy[LICZBA_LICZB_NA_STRONIE][DLUGOSC_LICZBY], int liczbaLiczbDoZapisania) {
	memset(buforZnakowy, '\0', LICZBA_LICZB_NA_STRONIE*DLUGOSC_LICZBY); // czyszczenie poprzedniej zawartosci bufora znakowego
	for (int i = 0; i < liczbaLiczbDoZapisania; i++) {
		sprintf(buforZnakowy[i], "%.4f", buforLiczbowy[i]); // zamiana liczby float na ciag znakow i zapisanie go do bufora
	}

}

void charToFloatBuffer(float buforLiczbowy[LICZBA_LICZB_NA_STRONIE], char buforZnakowy[LICZBA_LICZB_NA_STRONIE][DLUGOSC_LICZBY], int liczbaLiczbDoZapisania) {

	for (int i = 0; i < liczbaLiczbDoZapisania; i++) {
		buforLiczbowy[i] = atof(buforZnakowy[i]); // konwersja char* na float dla sprawnego operowania na liczbach
	}

}

void saveBufferToFile(char buforZnakowy[LICZBA_LICZB_NA_STRONIE][DLUGOSC_LICZBY], int liczbaLiczbDoZapisania, char* nazwaPliku) {

	FILE *plik = fopen(nazwaPliku, "ab");
	while (true) {
		if (plik == NULL) {
			printf("Nie udalo sie otworzyc pliku - probuje ponownie\n");
			plik = fopen(nazwaPliku, "ab");
		}
		else {
			break;
		}
	}
	int count = fwrite(buforZnakowy, DLUGOSC_LICZBY, liczbaLiczbDoZapisania, plik); // zapisanie zawartosci bufora znakowego w pliku wejsciowym
	if (count != liczbaLiczbDoZapisania) {
		perror("Nie udalo sie zapisac danych do pliku\n");
	}

	fclose(plik);
}

void readAndSort(float buforLiczbowy[LICZBA_LICZB_NA_STRONIE], char buforWejsciowyZnakowy[LICZBA_LICZB_NA_STRONIE][DLUGOSC_LICZBY]) {

	char *nazwaPlikuWejsciowego = "wejscie.bin";
	FILE *plik;
	liczbaDostepowDoPliku licznik;
	

	bool czyPosortowane = false;
	bool wypisuj = true;

	int indeksTasmy1 = 0, indeksTasmy2 = 0; // na tasmach moze byc jeszcze miejsce, dlatego przechowuje o tym informacje miedzy wywolaniami

	int aktualnaTasma;
	float ostatniRekordX, ostatniRekordY;

	float buforTasmy1[LICZBA_LICZB_NA_STRONIE], buforTasmy2[LICZBA_LICZB_NA_STRONIE]; // zawartosc buforow tasm ma znaczenie przy kolejnym wywolaniu, dlatego nie jest ona kasowana przy kolejnych wywolaniach (dopiero po zapisie)
	char buforZnakowy[LICZBA_LICZB_NA_STRONIE][DLUGOSC_LICZBY];

	printf("Czy chcesz wypisywac zawartosci tasm?\nT/N:");
	char tn;
	scanf("%c", &tn);
	scanf("%c", &tn);
	if (tn == 't' || tn == 'T')
		wypisuj = true;
	else if (tn == 'n' || tn == 'N')
		wypisuj = false;


	while (!czyPosortowane) {
		plik = fopen(nazwaPlikuWejsciowego, "rb"); // otwarty na czytanie
		remove("tasma1.bin");
		remove("tasma2.bin");

		aktualnaTasma = 1; // zresetowanie tasmy po kazdym cyklu odczyt, podzial, merge
		ostatniRekordX = ostatniRekordY = 0;
		memset(buforTasmy1, 0, sizeof(float)*LICZBA_LICZB_NA_STRONIE);
		memset(buforTasmy2, 0, sizeof(float)*LICZBA_LICZB_NA_STRONIE);
		memset(buforZnakowy, '\0', LICZBA_LICZB_NA_STRONIE*DLUGOSC_LICZBY);
		indeksTasmy1 = 0;
		indeksTasmy2 = 0;

		// wczytaj i podziel na tasmy
		while (true) {

			int liczbaWczytanychZnakow = fread(buforWejsciowyZnakowy, DLUGOSC_LICZBY, LICZBA_LICZB_NA_STRONIE, plik);
			licznik.odczyt++;
			if (liczbaWczytanychZnakow == LICZBA_LICZB_NA_STRONIE) {

				charToFloatBuffer(buforLiczbowy, buforWejsciowyZnakowy, liczbaWczytanychZnakow);

				findRuns(buforLiczbowy, liczbaWczytanychZnakow, indeksTasmy1, indeksTasmy2, buforTasmy1, buforTasmy2, aktualnaTasma, ostatniRekordX, ostatniRekordY, licznik);


			}
			else if (liczbaWczytanychZnakow == 0) { // jesli ostatnia wczytana strona by³a ostatni¹ stron¹ wejœciow¹

				// to przepisz zawartosc buforow tasm do tasm/plikow
				if (indeksTasmy1 > 0) {
					floatToCharBuffer(buforTasmy1, buforZnakowy, LICZBA_LICZB_NA_STRONIE);

					saveBufferToFile(buforZnakowy, indeksTasmy1, "tasma1.bin");
					licznik.zapis++;
				}
				if (indeksTasmy2 > 0) {
					floatToCharBuffer(buforTasmy2, buforZnakowy, LICZBA_LICZB_NA_STRONIE);

					saveBufferToFile(buforZnakowy, indeksTasmy2, "tasma2.bin");
					licznik.zapis++;
				}

				break;
			}
			else { // ostatnia strona jest niepe³na ( < LICZBA_LICZB_NA_STRONIE)

				charToFloatBuffer(buforLiczbowy, buforWejsciowyZnakowy, liczbaWczytanychZnakow);

				findRuns(buforLiczbowy, liczbaWczytanychZnakow, indeksTasmy1, indeksTasmy2, buforTasmy1, buforTasmy2, aktualnaTasma, ostatniRekordX, ostatniRekordY, licznik);


				if (indeksTasmy1 > 0) {
					floatToCharBuffer(buforTasmy1, buforZnakowy, LICZBA_LICZB_NA_STRONIE);

					saveBufferToFile(buforZnakowy, indeksTasmy1, "tasma1.bin");
					licznik.zapis++;
				}
				if (indeksTasmy2 > 0) {
					floatToCharBuffer(buforTasmy2, buforZnakowy, LICZBA_LICZB_NA_STRONIE);

					saveBufferToFile(buforZnakowy, indeksTasmy2, "tasma2.bin");
					licznik.zapis++;
				}

				break;
			}
		}

		remove("wyjscie.bin"); // usuniecie poprzedniego bufora - dane i tak s¹ na tasmach
		fclose(plik);
		nazwaPlikuWejsciowego = "wyjscie.bin";
		plik = fopen(nazwaPlikuWejsciowego, "wb");

		czyPosortowane = merge(plik, licznik);
		fclose(plik);
		
		if (wypisuj) {
			printf("Tasma 1:\n");
			displayFile("tasma1.bin");
			 
			printf("Tasma 2:\n");
			displayFile("tasma2.bin");

			if (czyPosortowane == false) {
				printf("Tasma scaleniowa:\n");
				displayFile("wyjscie.bin");
			}

			printf("Aby kontynuowac wypisywanie wcisnij spacje, aby przerwac wcisnij esc\n");
			char znak;
			while (znak = getch()) {
				if (znak == ' ')
					break;
				else if (znak == 27) { // kod escape
					wypisuj = false;
					break;
				}
			}
		}
	}
	printf("Tasma wyjsciowa:\n");
	plik = fopen(nazwaPlikuWejsciowego, "rb");

	char buforOdczytuTasmy[LICZBA_LICZB_NA_STRONIE][DLUGOSC_LICZBY];
	float buforLiczbowyOdczytu[LICZBA_LICZB_NA_STRONIE];
	while (true) {
		int i = fread(buforOdczytuTasmy, DLUGOSC_LICZBY, LICZBA_LICZB_NA_STRONIE, plik);
		charToFloatBuffer(buforLiczbowyOdczytu, buforOdczytuTasmy, i);
		if (i == 0)
			break;
		for (int j = 0; j < i; j += 2) {
			printf("(%.4f, %.4f): %.4f\n", buforLiczbowyOdczytu[j], buforLiczbowyOdczytu[j + 1], calculateDistance(buforLiczbowyOdczytu[j], buforLiczbowyOdczytu[j + 1]));
		}
	}
	fclose(plik);

	printf("Liczba odczytow: %d\nLiczba zapisow: %d\nLaczna liczba dostepow do plikow: %d\n", licznik.zapis, licznik.odczyt, licznik.odczyt + licznik.zapis);

}

void findRuns(float buforLiczbowy[LICZBA_LICZB_NA_STRONIE], int liczbaRekordowDoOdczytania, int &indeksTasmy1, int &indeksTasmy2, float buforTasmy1[LICZBA_LICZB_NA_STRONIE], float buforTasmy2[LICZBA_LICZB_NA_STRONIE], int &aktualnaTasma, float &ostatniRekordX, float &ostatniRekordY, liczbaDostepowDoPliku &licznik) { // szukaj ciagow rosnacych w danej stronie i rozdzielaj je na tasmy stronami

	int indeksBuforaLiczbowego = 0; // indeks bufora zeruje gdyz z kazdym wywolaniem funkcji dostaje nowa strone i wskaznik musi byc ustawiony na jej poczatek


	while (indeksBuforaLiczbowego != liczbaRekordowDoOdczytania) { // dopoki nie skonczy przetwarzac jednej strony
		switch (aktualnaTasma) {
		case 1:
			distributeRuns(buforLiczbowy, buforTasmy1, indeksBuforaLiczbowego, indeksTasmy1, aktualnaTasma, liczbaRekordowDoOdczytania, ostatniRekordX, ostatniRekordY, licznik);
			break;
		case 2:
			distributeRuns(buforLiczbowy, buforTasmy2, indeksBuforaLiczbowego, indeksTasmy2, aktualnaTasma, liczbaRekordowDoOdczytania, ostatniRekordX, ostatniRekordY, licznik);
			break;
		}
	}

}

void distributeRuns(float buforLiczbowy[LICZBA_LICZB_NA_STRONIE], float buforTasmy[LICZBA_LICZB_NA_STRONIE], int &indeksBuforaLiczbowego, int &indeksTasmy, int &aktualnaTasma, int liczbaRekordowDoOdczytania, float &ostatniRekordX, float &ostatniRekordY, liczbaDostepowDoPliku &licznik) {


	char buforZnakowy[LICZBA_LICZB_NA_STRONIE][DLUGOSC_LICZBY];

	while (true) {

		if (indeksTasmy == LICZBA_LICZB_NA_STRONIE && indeksBuforaLiczbowego == liczbaRekordowDoOdczytania) { // jeœli zarówno koniec strony wrzucanej na taœmê jak i koniec bufora wejœciowego

			floatToCharBuffer(buforTasmy, buforZnakowy, LICZBA_LICZB_NA_STRONIE);

			switch (aktualnaTasma) {
			case 1:
				saveBufferToFile(buforZnakowy, LICZBA_LICZB_NA_STRONIE, "tasma1.bin");
				licznik.zapis++;
				break;
			case 2:
				saveBufferToFile(buforZnakowy, LICZBA_LICZB_NA_STRONIE, "tasma2.bin");
				licznik.zapis++;
				break;
			}

			memset(buforTasmy, 0, sizeof(buforTasmy)* LICZBA_LICZB_NA_STRONIE);
			indeksTasmy = 0;

			break;
		}

		else if (indeksTasmy == LICZBA_LICZB_NA_STRONIE && indeksBuforaLiczbowego < liczbaRekordowDoOdczytania) { // strona wrzucana na taœmê zape³niona, ale wci¹¿ s¹ dane wejœciowe

			floatToCharBuffer(buforTasmy, buforZnakowy, LICZBA_LICZB_NA_STRONIE);

			switch (aktualnaTasma) {
			case 1:
				saveBufferToFile(buforZnakowy, LICZBA_LICZB_NA_STRONIE, "tasma1.bin");
				licznik.zapis++;
				break;
			case 2:
				saveBufferToFile(buforZnakowy, LICZBA_LICZB_NA_STRONIE, "tasma2.bin");
				licznik.zapis++;
				break;
			}

			memset(buforTasmy, 0, sizeof(buforTasmy)* LICZBA_LICZB_NA_STRONIE);
			indeksTasmy = 0;

		}
		else if (indeksTasmy < LICZBA_LICZB_NA_STRONIE && indeksBuforaLiczbowego == liczbaRekordowDoOdczytania) { // automatycznie wyklucza 1 sytuacje - tutaj chodzi o koniec bufora wejsciowego, ale nie strony wrzucanej na tasme

			break; // bo chce wczytac kolejna strone

		}
		else if (indeksTasmy < LICZBA_LICZB_NA_STRONIE && indeksBuforaLiczbowego < liczbaRekordowDoOdczytania) { // glowny przypadek - jest miejsce na stronie i sa nieodczytane rekordy z bufora

			if (compare(ostatniRekordX, ostatniRekordY, buforLiczbowy[indeksBuforaLiczbowego], buforLiczbowy[indeksBuforaLiczbowego + 1]) == 0) { // jesli nowy rekord jest wiekszy od poprzedniego - jesli jest seria
				ostatniRekordX = buforTasmy[indeksTasmy++] = buforLiczbowy[indeksBuforaLiczbowego++];
				ostatniRekordY = buforTasmy[indeksTasmy++] = buforLiczbowy[indeksBuforaLiczbowego++];
			}
			else {
				ostatniRekordX = 0, ostatniRekordY = 0; // przywrocenie wartosci domyslnych, aby druga tasma dodala do siebie bezwarunkowo
				if (aktualnaTasma == 1)
					aktualnaTasma = 2;
				else
					aktualnaTasma = 1;

				break;
			}
		}
	}

}

bool merge(FILE* plik, liczbaDostepowDoPliku &licznik) {

	FILE *tasma1, *tasma2;

	tasma1 = fopen("tasma1.bin", "rb");
	tasma2 = fopen("tasma2.bin", "rb");
	
	if (tasma2 == 0) { // przypadek dla pustej tasmy2 (czyli posortowane rekordy juz na wejsciu)
		fclose(tasma1);
		remove("tasma2.bin");
		fclose(plik);
		remove("wyjscie.bin");
		rename("tasma1.bin", "wyjscie.bin");
		return true; // nie odczytalo nic z drugiej tasmy, a wiec na tasmie 1 sa posortowane rekordy
	}
	
	float bufor1[LICZBA_LICZB_NA_STRONIE], bufor2[LICZBA_LICZB_NA_STRONIE], buforWyjsciowy[LICZBA_LICZB_NA_STRONIE], popRek1X, popRek1Y, popRek2X, popRek2Y;
	char buforZnakowy1[LICZBA_LICZB_NA_STRONIE][DLUGOSC_LICZBY], buforZnakowy2[LICZBA_LICZB_NA_STRONIE][DLUGOSC_LICZBY], buforZnakowyWyjsciowy[LICZBA_LICZB_NA_STRONIE][DLUGOSC_LICZBY];
	int indeksBufora1, indeksBufora2, indeksBuforaWyjsciowego;
	popRek1X = popRek1Y = popRek2X = popRek2Y = 0; // najmniejszy mozliwy rekord (wszystkie inne beda od niego >= )
	indeksBufora1 = indeksBufora2 = indeksBuforaWyjsciowego = 0;
	int liczbaOdczytanych1, liczbaOdczytanych2;
	liczbaOdczytanych1 = fread(buforZnakowy1, DLUGOSC_LICZBY, LICZBA_LICZB_NA_STRONIE, tasma1);
	liczbaOdczytanych2 = fread(buforZnakowy2, DLUGOSC_LICZBY, LICZBA_LICZB_NA_STRONIE, tasma2);
	charToFloatBuffer(bufor1, buforZnakowy1, LICZBA_LICZB_NA_STRONIE);
	charToFloatBuffer(bufor2, buforZnakowy2, LICZBA_LICZB_NA_STRONIE);
	bool czyBuforWyjsciowyJestJednymCiagiem = true;
	float ostatniZapisanyRekordX = 0, ostatniZapisanyRekordY = 0;
	

	while (true) { // dopoki jest cos na tasmie1 i tasmie2 

		if (compare(popRek1X, popRek1Y, bufor1[indeksBufora1], bufor1[indeksBufora1 + 1]) == 0 && compare(popRek2X, popRek2Y, bufor2[indeksBufora2], bufor2[indeksBufora2 + 1]) == 0){//jesli poprzedni rekord jest mniejszy w obu
			// jeœli w obu trwa seria (lub w³aœnie siê rozpoczê³a)
			if (compare(bufor1[indeksBufora1], bufor1[indeksBufora1 + 1], bufor2[indeksBufora2], bufor2[indeksBufora2 + 1]) == 0) { // jesli drugi rekord jest wiekszy lub rowny
				// to wrzuc mniejszy do buforaWyjsciowego
				popRek1X = bufor1[indeksBufora1];
				popRek1Y = bufor1[indeksBufora1 + 1];

				if (compare(ostatniZapisanyRekordX, ostatniZapisanyRekordY, popRek1X, popRek1Y) == 1) { // jesli poprzedni rekord byl wiekszy
					czyBuforWyjsciowyJestJednymCiagiem = false;
				}
				ostatniZapisanyRekordX = popRek1X;
				ostatniZapisanyRekordY = popRek1Y;

				writeToBuffer(buforWyjsciowy, indeksBuforaWyjsciowego, bufor1, indeksBufora1, licznik, plik);
			}
			else {
				popRek2X = bufor2[indeksBufora2];
				popRek2Y = bufor2[indeksBufora2 + 1];

				if (compare(ostatniZapisanyRekordX, ostatniZapisanyRekordY, popRek2X, popRek2Y) == 1) { // jesli poprzedni rekord byl wiekszy
					czyBuforWyjsciowyJestJednymCiagiem = false;
				}
				ostatniZapisanyRekordX = popRek2X;
				ostatniZapisanyRekordY = popRek2Y;

				writeToBuffer(buforWyjsciowy, indeksBuforaWyjsciowego, bufor2, indeksBufora2, licznik, plik);
			}
		}
		else if (compare(popRek1X, popRek1Y, bufor1[indeksBufora1], bufor1[indeksBufora1 + 1]) == 0) { // jesli seria w 2 sie skonczyla, a w 1 wciaz trwa
			popRek1X = bufor1[indeksBufora1];
			popRek1Y = bufor1[indeksBufora1 + 1];

			if (compare(ostatniZapisanyRekordX, ostatniZapisanyRekordY, popRek1X, popRek1Y) == 1) { // jesli poprzedni rekord byl wiekszy
				czyBuforWyjsciowyJestJednymCiagiem = false;
			}
			ostatniZapisanyRekordX = popRek1X;
			ostatniZapisanyRekordY = popRek1Y;

			writeToBuffer(buforWyjsciowy, indeksBuforaWyjsciowego, bufor1, indeksBufora1, licznik, plik);

		}
		else if (compare(popRek2X, popRek2Y, bufor2[indeksBufora2], bufor2[indeksBufora2 + 1]) == 0){ // jesli seria w 1 sie skonczyla, a w 2 wciaz trwa
			popRek2X = bufor2[indeksBufora2];
			popRek2Y = bufor2[indeksBufora2 + 1];

			if (compare(ostatniZapisanyRekordX, ostatniZapisanyRekordY, popRek2X, popRek2Y) == 1) { // jesli poprzedni rekord byl wiekszy
				czyBuforWyjsciowyJestJednymCiagiem = false;
			}
			ostatniZapisanyRekordX = popRek2X;
			ostatniZapisanyRekordY = popRek2Y;

			writeToBuffer(buforWyjsciowy, indeksBuforaWyjsciowego, bufor2, indeksBufora2, licznik, plik);
		}
		else { // jesli obie sie skonczyly
			popRek1X = popRek1Y = popRek2X = popRek2Y = 0;
		}


		// procedury sprawdzaj¹ce przypadki specjalne

		if (indeksBufora1 == liczbaOdczytanych1) { // jesli skonczyly sie rekordy w buforze1 do scalania
		
			readPage(buforZnakowy1, bufor1, liczbaOdczytanych1, licznik, indeksBufora1, tasma1);
			if (liczbaOdczytanych1 == 0) { // jesli skonczyly sie juz dane na tasmie 1
				// wrzuc cala zawartosc drugiej tasmy
				while (liczbaOdczytanych2 != 0) {
					if (indeksBufora2 != liczbaOdczytanych2) {

						popRek2X = bufor2[indeksBufora2];
						popRek2Y = bufor2[indeksBufora2 + 1];

						if (compare(ostatniZapisanyRekordX, ostatniZapisanyRekordY, popRek2X, popRek2Y) == 1) { // jesli poprzedni rekord byl wiekszy
							czyBuforWyjsciowyJestJednymCiagiem = false;
						}
						ostatniZapisanyRekordX = popRek2X;
						ostatniZapisanyRekordY = popRek2Y;

						writeToBuffer(buforWyjsciowy, indeksBuforaWyjsciowego, bufor2, indeksBufora2, licznik, plik);
					}
					else {
						readPage(buforZnakowy2, bufor2, liczbaOdczytanych2, licznik, indeksBufora2, tasma2);
					}
				}
				break;
			}
		}

		if (indeksBufora2 == liczbaOdczytanych2) {

			readPage(buforZnakowy2, bufor2, liczbaOdczytanych2, licznik, indeksBufora2, tasma2);
			if (liczbaOdczytanych2 == 0) { // jesli skonczyly sie juz dane na tasmie 1
				// wrzuc cala zawartosc drugiej tasmy
				while (liczbaOdczytanych1 != 0) { 
					if (indeksBufora1 != liczbaOdczytanych1) {

						popRek1X = bufor1[indeksBufora1];
						popRek1Y = bufor1[indeksBufora1 + 1];

						if (compare(ostatniZapisanyRekordX, ostatniZapisanyRekordY, popRek1X, popRek1Y) == 1) { // jesli poprzedni rekord byl wiekszy
							czyBuforWyjsciowyJestJednymCiagiem = false;
						}
						ostatniZapisanyRekordX = popRek1X;
						ostatniZapisanyRekordY = popRek1Y;

						writeToBuffer(buforWyjsciowy, indeksBuforaWyjsciowego, bufor1, indeksBufora1, licznik, plik);
					}
					else {
						readPage(buforZnakowy1, bufor1, liczbaOdczytanych1, licznik, indeksBufora1, tasma1);
					}
				}
				break;
			}
		}


	}

	if (indeksBuforaWyjsciowego != 0) {
		floatToCharBuffer(buforWyjsciowy, buforZnakowyWyjsciowy, LICZBA_LICZB_NA_STRONIE);
		fwrite(buforZnakowyWyjsciowy, DLUGOSC_LICZBY, indeksBuforaWyjsciowego, plik);
		licznik.zapis++;
	}

	fclose(tasma1);
	fclose(tasma2);

	return czyBuforWyjsciowyJestJednymCiagiem; // nie mamy do czynienia z ciagiem
}

void readPage(char buforZnakowy[LICZBA_LICZB_NA_STRONIE][DLUGOSC_LICZBY], float buforLiczbowy[LICZBA_LICZB_NA_STRONIE], int &liczbaOdczytanych, liczbaDostepowDoPliku &licznik, int &indeksBufora, FILE *tasma) {

	liczbaOdczytanych = fread(buforZnakowy, DLUGOSC_LICZBY, LICZBA_LICZB_NA_STRONIE, tasma);
	licznik.odczyt++;
	charToFloatBuffer(buforLiczbowy, buforZnakowy, LICZBA_LICZB_NA_STRONIE);
	indeksBufora = 0;

}

void writeToBuffer(float buforWyjsciowy[LICZBA_LICZB_NA_STRONIE], int &indeksBuforaWyjsciowego, float bufor[LICZBA_LICZB_NA_STRONIE], int &indeksBufora, liczbaDostepowDoPliku &licznik, FILE* plik) {

	char buforZnakowyWyjsciowy[LICZBA_LICZB_NA_STRONIE][DLUGOSC_LICZBY];

	buforWyjsciowy[indeksBuforaWyjsciowego++] = bufor[indeksBufora++];
	buforWyjsciowy[indeksBuforaWyjsciowego++] = bufor[indeksBufora++];

	if (indeksBuforaWyjsciowego == LICZBA_LICZB_NA_STRONIE) { // jesli strona wyjsciowa sie zapelnila
		floatToCharBuffer(buforWyjsciowy, buforZnakowyWyjsciowy, LICZBA_LICZB_NA_STRONIE);
		fwrite(buforZnakowyWyjsciowy, DLUGOSC_LICZBY, LICZBA_LICZB_NA_STRONIE, plik);
		licznik.zapis++;
		indeksBuforaWyjsciowego = 0;
		memset(buforWyjsciowy, 0, sizeof(float)*LICZBA_LICZB_NA_STRONIE);
	}
}

float calculateDistance(float a, float b) {

	float distance = sqrt(a*a + b*b);

	return distance;
}

int compare(float a, float b, float c, float d) {

	// indeks wskazuje na pierwsza z czterech liczb ktore beda uwzglednione w porownaniu

	if (calculateDistance(a, b) > calculateDistance(c, d)) { // jesli pierwszy punkt znajduje sie dalej od srodka ukladu od drugiego
		return 1;
	}
	else { // jesli pierwszy punkt znajduje sie w tej samej odleglosci co drugi lub blizej srodka ukladu - czyli jesli mamy ciag
		return 0;
	}
}

void displayFile(char* nazwaPliku) {

	FILE *plik;

	plik = fopen(nazwaPliku, "rb");
	if (plik != NULL) {
		char buforOdczytuTasmy[LICZBA_LICZB_NA_STRONIE][DLUGOSC_LICZBY];
		float buforLiczbowyOdczytu[LICZBA_LICZB_NA_STRONIE];
		while (true) {
			int i = fread(buforOdczytuTasmy, DLUGOSC_LICZBY, LICZBA_LICZB_NA_STRONIE, plik);
			charToFloatBuffer(buforLiczbowyOdczytu, buforOdczytuTasmy, i);
			if (i == 0)
				break;
			for (int j = 0; j < i; j += 2) {
				printf("(%.4f, %.4f): %.4f\n", buforLiczbowyOdczytu[j], buforLiczbowyOdczytu[j + 1], calculateDistance(buforLiczbowyOdczytu[j], buforLiczbowyOdczytu[j + 1]));
			}
		}
		fclose(plik);
	}
	
}