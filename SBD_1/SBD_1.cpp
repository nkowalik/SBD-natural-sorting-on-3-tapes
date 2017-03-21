/* Program sortuj�cy plik (jedn� z metod scalania)
- ta�my realizowane w formie plik�w dyskowych;
- rekordy do posortowania generowane zar�wno losowo, jak i z klawiatury;
- wy�wietlanie zawarto�ci pliku przed posortowaniem i po posortowaniu;
- mo�liwo�� wy�wietlenia pliku po ka�dej fazie sortowania;
- wy�wietlenie liczby faz sortowania oraz liczby odczyt�w i zapis�w stron na dysk (na samym ko�cu)
- mo�liwo�� wczytywania danych testowych z pliku testowego */
/*
5. Rekordy pliku: zbiory znak�w (maksymalnie 30).
Uporz�dkowanie zgodnie z zasad�: dla dw�ch zbior�w znak�w A i B,
A <= B je�li po odrzuceniu znak�w znajduj�cych si� jednocze�nie w obu zbiorach,
w B istnieje znak po�o�ony w kodzie ASCII dalej od wszystkich znak�w w A.
*/


#include <iostream>
#include <ctime>
#include <cstdio>
using namespace std;

#define MAX_SIZE 30		// maksymalna liczba znak�w w rekordzie
#define PAGE_SIZE 20	// sta�a liczba rekord�w mieszcz�cych si� na stronie

void generateRandom(const char* fileName);
void getRecordsFromKeyboard(const char* fileName);
void getRecordsFromFile(const char* fileName1, const char* fileName2);
int compareRecords(char record1[], char record2[]);
void displayFile(const char* fileName);
void distribute(const char* tapeFile1, const char* tapeFile2, const char* inputFile, long long int* diskAccesses, int display, bool* tapes);
bool merge(const char* tapeFile1, const char* tapeFile2, const char* outputFile, long long int* diskAccesses, int display, bool* tapes);


int main() {
	const char* inputFile = "records.bin";
	const char* fileName = "file.bin";
	const char* tapeFile1 = "tape1.bin";
	const char* tapeFile2 = "tape2.bin";
	long long int diskAccesses[2] = { 0, 0 }; // odczyt i zapis do dysku
	int phasesNumber = 0, method = 0, display = 0; // liczba faz
	bool sorted = false;
	
	// Tworzenie pliku testowego:
	//getRecordsFromKeyboard(inputFile); 
	//generateRandom(inputFile);

	cout << "Wybierz metode wczytywania rekordow: " << endl;
	cout << "Wpisz 1 dla wczytywania rekordow z klawiatury \nWpisz 2 dla losowego generowania rekordow \nWpisz 3 dla wczytywania z pliku testowego\n";
	cin >> method;
	cout << "Czy chcesz wyswietlac plik po kazdej fazie sortowania? \n1 - tak \n2 - nie \n";
	cin >> display;
	while (display != 1 && display != 2) {
		cout << "Wprowadz liczbe jeszcze raz: \n1 - wyswietlaj plik po kazdej fazie sortowania \n2 - nie wyswietlaj \n";
		cin >> display;
	}

	switch (method) {
	case 1:
		getRecordsFromKeyboard(fileName);
		break;
	case 2:
		generateRandom(fileName);
		break;
	case 3:
		getRecordsFromFile(inputFile, fileName);
		break;
	}

	cout << endl << endl << "Plik przed posortowaniem: \n\n";
	displayFile(fileName);

	while (sorted == false) {
		bool tapes[2] = { false, false }; // czy na ta�mach 1|2 istniej� rekordy
		distribute(tapeFile1, tapeFile2, fileName, diskAccesses, display, tapes);
		if (tapes[0] == true) // Je�li istniej� jakie� rekordy na ta�mach 
			sorted = merge(tapeFile1, tapeFile2, fileName, diskAccesses, display, tapes);
		else sorted = true;
		phasesNumber++;
	}

	cout << "Plik po posortowaniu: \n\n";
	displayFile(fileName);

	cout << "Liczba odczytow: " << diskAccesses[0] << endl;
	cout << "Liczba zapisow: " << diskAccesses[1] << endl;
	cout << "Liczba faz: " << phasesNumber << endl;

	getchar(); getchar();
	return 0;
}


// Funkcja por�wnuj�ca dwa elementy; 
// wska�nik do funkcji 'compare' przekazywany jako argument w qsort pozwala na malej�ce porz�dkowanie znak�w w zbiorach.
inline int compare(const void* a, const void* b) {
	if (*(char*)a <  *(char*)b) return 1;
	if (*(char*)a == *(char*)b) return 0;
	if (*(char*)a >  *(char*)b) return -1;
}

// Zwraca numer wi�kszego rekordu (1, 2 lub 0, gdy r�wne).
int compareRecords(char record1[], char record2[]) {
	char buforPomocniczy1[MAX_SIZE + 1] = {}, buforPomocniczy2[MAX_SIZE + 1] = {};
	strcpy(buforPomocniczy1, record1);
	strcpy(buforPomocniczy2, record2);
	buforPomocniczy1[MAX_SIZE] = '\0';
	buforPomocniczy2[MAX_SIZE] = '\0';

	// Sortowanie znak�w w recordach malej�co za pomoc� algorytmu quicksort
	qsort(buforPomocniczy1, MAX_SIZE, sizeof(char), compare);
	qsort(buforPomocniczy2, MAX_SIZE, sizeof(char), compare);

	// Szukanie najwi�kszego znaku w zbiorach 
	// i usuwanie znak�w, kt�re si� powtarzaj� w obu zbiorach
	for (int i = 0, j = 0;;) {
		char najwiekszyZnak = buforPomocniczy1[i];

		if (najwiekszyZnak < buforPomocniczy2[j])		// od razu wiadomo, �e drugi rekord jest wi�kszy od pierwszego
			return 2;
		else if (najwiekszyZnak > buforPomocniczy2[j])	// pierwszy rekord jest wi�kszy
			return 1;
		else if (najwiekszyZnak == '\0') // oba rekordy s� r�wne
			return 0;
		else { // znak wyst�puje w obu zbiorach co najmniej raz
			i++; j++;
			while (buforPomocniczy1[i] == najwiekszyZnak) i++;	// usuwanie powtarzaj�cych si� znak�w w obu rekordach
			while (buforPomocniczy2[j] == najwiekszyZnak) j++;
		}

		if (i == MAX_SIZE && j == MAX_SIZE)  // w obu skonczyly sie znaki
			return 0;
		else if (i == MAX_SIZE)				// w pierwszym buforze sko�czy�y si� znaki, a w drugim co� jeszcze jest
			return 2;
		else if (j == MAX_SIZE)
			return 1;
	}
}

// Wy�wietla zawarto�� wybranego pliku.
void displayFile(const char* fileName) {
	char buffer[PAGE_SIZE][MAX_SIZE + 1] = {};

	FILE *pFile = fopen(fileName, "rb"); // Otwiera istniej�cy plik w trybie do odczytu.
	while (pFile == NULL) pFile = fopen(fileName, "rb");

	while (!feof(pFile)) {
		int recordsLeft = fread(buffer, MAX_SIZE + 1, PAGE_SIZE, pFile);

		for (int i = 0; i < recordsLeft && buffer[i][0] >= 32; i++) {
			qsort(buffer[i], MAX_SIZE, sizeof(char), compare);
			cout << buffer[i] << endl;
		}

		memset(buffer, '\0', PAGE_SIZE*(MAX_SIZE + 1)); 	// Czyszczenie strony
	}

	cout << endl << endl;

	fclose(pFile);
}

// Tworzy losowy rekord.
inline void getRandom(char* record) {
	int charNumber = 0;
	charNumber = rand() % MAX_SIZE + 1;		// Liczba znak�w: od 1 do 30
	for (int j = 0; j < charNumber; j++)
		record[j] = rand() % 92 + 32;		// Losowanie znak�w od spacji (32) do ~ (126)
}

// Zapisuje x losowych rekord�w do pliku.
void generateRandom(const char* fileName) {
	srand(time(NULL));
	int recordsNumber = 0; // liczba znak�w
	char record[MAX_SIZE + 1] = {};

	cout << "Podaj liczbe rekordow do wygenerowania" << endl;
	cin >> recordsNumber;

	FILE * pFile = fopen(fileName, "wb+");
	while (pFile == NULL) {
		cout << "Nie udalo sie otworzyc pliku, probuje ponownie\n";
		pFile = fopen(fileName, "wb+");
	}

	for (int i = 0; i < recordsNumber; i++) {
		getRandom(record);
		fwrite(record, sizeof(char), MAX_SIZE + 1, pFile);	// Zapisywanie rekordami do pliku
		memset(record, '\0', sizeof(record));	// Czyszczenie rekordu
	}

	fclose(pFile);
}

// Zapisuje rekordy ze strony do wybranego pliku.
inline void saveToFile(char buffer[][MAX_SIZE + 1], const char* fileName, int recordsNumber, long long int & diskAccesses) {
	FILE *pFile = fopen(fileName, "ab"); // dopisz na koniec pliku

	while (pFile == NULL) {
		cout << "Nie udalo sie otworzyc pliku, probuje ponownie\n";
		pFile = fopen(fileName, "ab");
	}

	fwrite(buffer, MAX_SIZE + 1, recordsNumber, pFile);
	diskAccesses++;
	fclose(pFile);
}

// Pobiera rekordy z wybranego pliku i zapisuje do pliku binarnego.
void getRecordsFromFile(const char* fileName1, const char* fileName2) {
	char record[MAX_SIZE + 1] = {};
	FILE *pFile1 = fopen(fileName1, "r"); // Otwiera istniej�cy plik w trybie do odczytu
	FILE *pFile2 = fopen(fileName2, "wb+");

	while (pFile1 == NULL) {
		cout << "Nie udalo sie otworzyc pliku, probuje ponownie\n";
		pFile1 = fopen(fileName1, "r");
	}
	while (pFile2 == NULL) {
		cout << "Nie udalo sie otworzyc pliku, probuje ponownie\n";
		pFile2 = fopen(fileName2, "wb+");
	}

	while (!feof(pFile1)) {
		fread(record, sizeof(char), MAX_SIZE + 1, pFile1);
		fwrite(record, sizeof(char), MAX_SIZE + 1, pFile2);
		memset(record, '\0', MAX_SIZE + 1);
	}
	fclose(pFile1);
	fclose(pFile2);
}

// Pobiera rekordy i zapisuje do pliku.
void getRecordsFromKeyboard(const char* fileName) {
	int recordsNumber = 0;
	char record[MAX_SIZE + 1] = {};
	FILE * pFile = fopen(fileName, "wb+"); // Tworzy nowy plik i ustawia uchwyt w trybie do odczytu jak i zapisu

	while (pFile == NULL) {
		cout << "Nie udalo sie otworzyc pliku, probuje ponownie\n";
		pFile = fopen(fileName, "wb+");
	}

	cout << "Podaj liczbe rekordow, a nastepnie rekordy" << endl;
	cin >> recordsNumber;
	cin.ignore(); // wczytanie znaku nowej linii po wprowadzeniu liczby rekordow

	for (int i = 0; i < recordsNumber; i++) {
		memset(record, '\0', sizeof(record)); 	// Czyszczenie rekordu
		cin.getline(record, MAX_SIZE + 1);
		fwrite(record, sizeof(char), MAX_SIZE + 1, pFile);
	}

	fclose(pFile);
}

// Zapisuje do bufora-ta�my rekord z bufora wej�ciowego.
inline int writeToTapeBuffer(char buffer[PAGE_SIZE][MAX_SIZE + 1], char inputBuffer[PAGE_SIZE][MAX_SIZE + 1], int & indexTape, int & inputIndex, int previous) {
	strcpy(buffer[indexTape], inputBuffer[inputIndex]);
	indexTape++;
	inputIndex++;
	return previous;
}

// Wrzuca stron� z buforu na ta�m�, czy�ci bufor i indeks.
inline int saveToTapeFile(char* lastRecord, char buffer[PAGE_SIZE][MAX_SIZE + 1], const char* tapeFile, long long int & diskAccesses) {
	if (lastRecord != NULL)
		strcpy(lastRecord, buffer[PAGE_SIZE - 1]);
	saveToFile(buffer, tapeFile, PAGE_SIZE, diskAccesses);
	memset(buffer, '\0', PAGE_SIZE*(MAX_SIZE + 1));
	return 0;
}

// Dystrybuuje rekordy z pliku wej�ciowego na zmian� seriami na dwie ta�my.
void distribute(const char* tapeFile1, const char* tapeFile2, const char* inputFile, long long int* diskAccesses, int display, bool* tapes) {
	char buffer1[PAGE_SIZE][MAX_SIZE + 1] = {}, buffer2[PAGE_SIZE][MAX_SIZE + 1] = {};
	char inputBuffer[PAGE_SIZE][MAX_SIZE + 1] = {};
	char lastRecord1[MAX_SIZE + 1] = {}, lastRecord2[MAX_SIZE + 1] = {};

	int inputIndex = 0, indexTape1 = 0, indexTape2 = 0, largerRecord = 0, previousBuffer = 1;

	FILE *pFile = fopen(inputFile, "rb");			// otwiera istniej�cy plik w trybie do odczytu
	remove(tapeFile1);	remove(tapeFile2);

	// Wczytanie rekord�w z pliku do bufora o wielko�ci strony i odczytanie ile rekord�w zosta�o wczytanych (fread zwraca liczb� bajt�w)
	int inputRecordsNumber = (fread(inputBuffer, MAX_SIZE + 1, PAGE_SIZE, pFile));
	diskAccesses[0]++;

	if (inputRecordsNumber == 0 || pFile == NULL) { // brak rekord�w w pliku wej�ciowym
		tapes[0] = false;
		tapes[1] = false;
		return; 
	}
	else tapes[0] = true;

	strcpy(buffer1[indexTape1], inputBuffer[inputIndex]);		// Pierwszy rekord zapisywany jest do pierwszego bufora
	inputIndex++; indexTape1++;

	// Polecenia wykonuj� si� do ko�ca pliku, dop�ki jest co wczytywa� i dystrybuowa�
	while (true) {

		// Je�li wczytana liczba rekord�w jest mniejsza od strony i aktualny wska�nik jest jej r�wny
		if (inputRecordsNumber < PAGE_SIZE && inputIndex >= inputRecordsNumber)
			break;

		// Por�wnanie dw�ch rekord�w:
		switch (previousBuffer) {
		case 1:
			// Je�li bufor zosta� wrzucony do pliku i wyczyszczony, sprawd� czy trwa seria:
			if (indexTape1 == 0)
				largerRecord = compareRecords(lastRecord1, inputBuffer[inputIndex]);
			else
				largerRecord = compareRecords(buffer1[indexTape1 - 1], inputBuffer[inputIndex]);
			break;

		case 2:
			if (indexTape2 == 0)
				largerRecord = compareRecords(lastRecord2, inputBuffer[inputIndex]);
			else
				largerRecord = compareRecords(buffer2[indexTape2 - 1], inputBuffer[inputIndex]);
			break;
		}

		/* Je�li rekordy by�y zapisywane na pierwszym buforze i trwa seria (rekordy niemalej�ce) LUB
		by�y zapisywane na drugim i sko�czy�a si� seria	TO: dopisz rekord do pierwszego bufora */
		if ((largerRecord != 1 && previousBuffer == 1) || (largerRecord == 1 && previousBuffer == 2))
			previousBuffer = writeToTapeBuffer(buffer1, inputBuffer, indexTape1, inputIndex, 1);

		/* Je�li rekordy by�y zapisywane na pierwszym buforze i sko�czy�a si� seria LUB
		by�y zapisywane na drugim i trwa seria TO: dopisz rekord do drugiego bufora */
		else if ((largerRecord != 1 && previousBuffer == 2) || (largerRecord == 1 && previousBuffer == 1)) {
			previousBuffer = writeToTapeBuffer(buffer2, inputBuffer, indexTape2, inputIndex, 2);
			tapes[1] = true;
		}

		if (indexTape1 == PAGE_SIZE) 		// Po zape�nieniu strony zapisuje rekordy na pierwsz� ta�m�
			indexTape1 = saveToTapeFile(lastRecord1, buffer1, tapeFile1, diskAccesses[1]);

		if (indexTape2 == PAGE_SIZE) {		// Po zape�nieniu strony zapisuje rekordy na drug� ta�m�
			indexTape2 = saveToTapeFile(lastRecord2, buffer2, tapeFile2, diskAccesses[1]);
		}

		if (inputIndex == PAGE_SIZE) {		// Po zape�nieniu pomocniczej tablicy pobiera kolejn� stron� rekord�w z dysku
			memset(inputBuffer, '\0', PAGE_SIZE*(MAX_SIZE + 1));
			inputIndex = 0;
			inputRecordsNumber = fread(inputBuffer, MAX_SIZE + 1, PAGE_SIZE, pFile);
			diskAccesses[0]++;
			if (inputRecordsNumber == 0)
				break;
		}

	}

	// Je�li po ko�cu pliku w buforach zosta�y jakie� rekordy - trzeba je dopisa� na koniec pliku i go zamkn��.
	if (indexTape1 != 0)
		saveToFile(buffer1, tapeFile1, indexTape1, diskAccesses[1]);
	if (indexTape2 != 0 && buffer2[0][0] != '\0') {
		saveToFile(buffer2, tapeFile2, indexTape2, diskAccesses[1]);
	}

	fclose(pFile);

	if (display == 1) {
		cout << "Pierwsza tasma:\n\n";
		displayFile(tapeFile1);
		cout << "Druga tasma:\n\n";
		if (tapes[1] == true)
			displayFile(tapeFile2);
		else cout << endl << endl;
	}
}

// Znajduje indeks ostatniego rekordu w serii
int findRunEnd(char buffer[PAGE_SIZE][MAX_SIZE + 1], int indexTape) {
	int larger = 0;

	if (buffer[indexTape][0] < 32) // Je�li nie ma wi�cej serii
		return -1;

	while (larger != 1 && indexTape < PAGE_SIZE) {	// Dop�ki rekordy ustawione s� niemalej�co
		if (buffer[indexTape + 1][0] < 32) {
			indexTape++;
			break;
		}
		larger = compareRecords(buffer[indexTape], buffer[indexTape + 1]);
		indexTape++;
	}

	return indexTape - 1;
}

// Wczytuje rekordy z pliku do bufora, zapisuje ostatni rekord z poprzedniej tablicy i liczb� wczytanych rekord�w.
inline int readToBuffer(char* last, char buffer[PAGE_SIZE][MAX_SIZE + 1], int indexTape, FILE* pFile, int & recordsLeft, long long int & diskAccesses) {
	strcpy(last, buffer[indexTape - 1]);
	memset(buffer, '\0', PAGE_SIZE*(MAX_SIZE + 1));
	recordsLeft = fread(buffer, MAX_SIZE + 1, PAGE_SIZE, pFile);
	diskAccesses++;
	return 0;
}

// Zapisuje rekord z bufora-ta�my do bufora wyj�ciowego, a po zape�nieniu bufora wyj zapisuje do pliku wyj.
inline void writeToBuffer(char buffer[PAGE_SIZE][MAX_SIZE + 1], char outputBuffer[PAGE_SIZE][MAX_SIZE + 1], int & indexTape,
	int & outputIndex, const char* outputFile, long long int & diskAccesses) {
	if (outputIndex == PAGE_SIZE)
		outputIndex = saveToTapeFile(NULL, outputBuffer, outputFile, diskAccesses);
	strcpy(outputBuffer[outputIndex], buffer[indexTape]);
	outputIndex++;
	indexTape++;
}

// Scala dwie ta�my rekord�w w pliku wyj�ciowym.
bool merge(const char* tapeFile1, const char* tapeFile2, const char* outputFile, long long  int* diskAccesses, int display, bool* tapes) {
	char buffer1[PAGE_SIZE][MAX_SIZE + 1] = {}, buffer2[PAGE_SIZE][MAX_SIZE + 1] = {};
	char outputBuffer[PAGE_SIZE][MAX_SIZE + 1] = {}; // Tablica pomocnicza (do bufor�w zostan� wpisane strony z plik�w)
	char last1[MAX_SIZE + 1] = {}, last2[MAX_SIZE + 1] = {}; // ostatni rekord na stronie

	int indexTape1 = 0, indexTape2 = 0, outputIndex = 0, recordsLeft1 = -1, recordsLeft2 = -1, larger = 0;
	int end1 = 0, end2 = 0; // indeks ostatniego rekordu w serii
	bool sorted = true, run1End = false, run2End = false;

	FILE *pFile1 = fopen(tapeFile1, "rb"); // Otwiera istniej�cy plik w trybie do odczytu
	FILE *pFile2 = fopen(tapeFile2, "rb");
	remove(outputFile);

	// Je�li na drugiej ta�mie nie ma rekord�w (plik jest pusty), wpisz ca�� ta�m� pierwsz� do wyj�ciowej
	if (tapes[1] == false || pFile2 == NULL) {
		while (true) {
			writeToBuffer(buffer1, outputBuffer, indexTape1, outputIndex, outputFile, diskAccesses[1]);
			if (indexTape1 == PAGE_SIZE) 
				indexTape1 = readToBuffer(last1, buffer1, indexTape1, pFile1, recordsLeft1, diskAccesses[0]);

			// koniec rekord�w na ta�mach ko�czy scalanie
			if (recordsLeft1 == 0 || end1 == -1 || (indexTape1 > recordsLeft1 - 1)) {
				if (outputIndex != 0)
					saveToFile(outputBuffer, outputFile, outputIndex, diskAccesses[1]);
				fclose(pFile1);
				if (pFile2 != NULL) fclose(pFile2);
				return true;
			}
		}
	}

	// Odczytaj rekordy z obu ta�m do bufor�w (po 20 z ka�dej ta�my)
	recordsLeft1 = fread(buffer1, MAX_SIZE + 1, PAGE_SIZE, pFile1);
	diskAccesses[0]++;

	recordsLeft2 = fread(buffer2, MAX_SIZE + 1, PAGE_SIZE, pFile2);
	diskAccesses[0]++;
	
	// Znajd� indeks ostatniego rekordu w pierwszych seriach na obu ta�mach
	end1 = findRunEnd(buffer1, indexTape1);
	end2 = findRunEnd(buffer2, indexTape2);

	// Je�li seria jest kr�tsza ni� liczba wczytanych rekord�w to znaczy, �e plik nie jest posortowany
	// end = -1, gdy nie ma ju� wi�cej serii - koniec pliku
	if (end1 < (recordsLeft1 - 1) && end1 != -1) sorted = false;
	if (end2 < (recordsLeft2 - 1) && end2 != -1) sorted = false;

	while (true) {
		// Dop�ki trwaj� obie serie
		while (run1End == false || run2End == false) {
			// Ustawienie flag m�wi�cych o ko�cu serii
			run1End = indexTape1 > end1;	// Sprawd� czy aktualny indeks jest wi�kszy ni� indeks ko�ca serii
			run2End = indexTape2 > end2;

			// Je�li ca�a strona z pierwszej ta�my zosta�a scalona na ta�mie wyj�ciowej - pobierz kolejn� stron�
			if (indexTape1 == PAGE_SIZE) { 
				indexTape1 = readToBuffer(last1, buffer1, indexTape1, pFile1, recordsLeft1, diskAccesses[0]);
				if (compareRecords(last1, buffer1[0]) != 1)	{	// Je�eli trwa seria, to znajd� jej koniec
					end1 = findRunEnd(buffer1, indexTape1);
					run1End = false;
				}
				else {	// koniec serii na pierwszej ta�mie
					end1 = 0;
					run1End = true;
					sorted = false;
				}
			}
			if (indexTape2 == PAGE_SIZE) {
				indexTape2 = readToBuffer(last2, buffer2, indexTape2, pFile2, recordsLeft2, diskAccesses[0]);
				if (compareRecords(last2, buffer2[0]) != 1)	{	// Je�eli trwa seria, to znajd� jej koniec
					end2 = findRunEnd(buffer2, indexTape2);
					run2End = false;
				}
				else {
					end2 = 0;
					run2End = true;
					sorted = false;
				}
			}

			// Je�li seria jest kr�tsza ni� liczba wczytanych rekord�w to znaczy, �e plik nie jest posortowany
			if (end1 < (recordsLeft1 - 1) && end1 != -1) sorted = false;
			if (end2 < (recordsLeft2 - 1) && end2 != -1) sorted = false;

			if (end1 == -1 && end2 == -1) break; // koniec rekord�w na obu ta�mach

			if (run1End == true) { // Je�li seria na pierwszej ta�mie si� sko�czy�a
				while (indexTape2 <= end2)  // Wczytaj do ko�ca serii na ta�mie drugiej
					writeToBuffer(buffer2, outputBuffer, indexTape2, outputIndex, outputFile, diskAccesses[1]);
				run2End = true;
				break;
			}
			if (run2End == true) { // Je�li seria na ta�mie2 si� sko�czy�a
				while (indexTape1 <= end1)
					writeToBuffer(buffer1, outputBuffer, indexTape1, outputIndex, outputFile, diskAccesses[1]);
				run1End = true;
				break;
			}

			larger = compareRecords(buffer1[indexTape1], buffer2[indexTape2]); // Znajd� ta�m� z wi�kszym rekordem

			if (larger != 1)  // Je�li rekord w 1 buforze <= rekord w 2 - kopiuj rekord z 1. bufora
				writeToBuffer(buffer1, outputBuffer, indexTape1, outputIndex, outputFile, diskAccesses[1]);
			else
				writeToBuffer(buffer2, outputBuffer, indexTape2, outputIndex, outputFile, diskAccesses[1]);
		}

		if (recordsLeft1 == 0 && recordsLeft2 == 0) break; // koniec rekord�w na ta�mach ko�czy scalanie
		if (end1 == -1 && end2 == -1) break;

		if (indexTape1 == PAGE_SIZE) // Je�li do bufora wyj�ciowego wczytano ca�� stron� z ta�my1 - pobierz kolejn� z pliku
			indexTape1 = readToBuffer(last1, buffer1, indexTape1, pFile1, recordsLeft1, diskAccesses[0]);
		if (indexTape2 == PAGE_SIZE)
			indexTape2 = readToBuffer(last2, buffer2, indexTape2, pFile2, recordsLeft2, diskAccesses[0]);

		// Je�eli kt�ra� z serii si� sko�czy�a - znajd� nowy koniec serii
		if (run1End == true) {
			end1 = findRunEnd(buffer1, indexTape1);
			if (end1 != -1) run1End = false;
		}
		if (run2End == true) {
			end2 = findRunEnd(buffer2, indexTape2);
			if (end2 != -1) run2End = false;
		}

		// Je�eli seria jest kr�tsza ni� liczba wczytanych rekord�w to znaczy, �e plik nie jest posortowany
		if (end1 != (recordsLeft1 - 1) && end1 != -1) sorted = false;
		if (end2 != (recordsLeft2 - 1) && end2 != -1) sorted = false;
	}

	// Wrzu� do pliku niepe�n� stron�
	if (outputIndex != 0)
		saveToFile(outputBuffer, outputFile, outputIndex, diskAccesses[1]);

	fclose(pFile1);
	fclose(pFile2);
	

	if (display == 1) {
		cout << "Tasma scaleniowa:\n\n";
		displayFile(outputFile);
	}

	return sorted;
}