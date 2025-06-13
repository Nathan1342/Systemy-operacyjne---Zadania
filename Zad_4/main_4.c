#define _GNU_SOURCE
#include <stdio.h> // printf, fprintf
#include <stdlib.h> // exit, EXIT_FAILURE, EXIT_SUCCESS
#include <unistd.h> // fork, read, write, close
#include <sys/stat.h> // open
#include <fcntl.h> // open
#include <string.h>
#include <time.h>
#include <sys/types.h> // fork, open
#include <sys/wait.h> // wait

#define WIELKOSC_PRODUCENT 5
#define WIELKOSC_KONSUMENT 3

int main(int argc, char *argv[]) {

	if (argc != 3) {
		fprintf(stderr, "Użycie: %s <plik_wejściowy> <plik_wyjściowy>\n", argv[0]);
		return EXIT_FAILURE;
	}

	// Tablica na deskryptory:
	// - tab_des[0] - koniec do odczytu
	// - tab_des[1] - koniec do zapisu
	// Oba procesy dziedziczą oba końce więc w każdym trzeba zamknąc jeden z nich
	int tab_des[2];

	// Stworzenie potoku nienazwanego
	if (pipe(tab_des) == -1) {
		perror("Blad funkcji pipe");
		exit(EXIT_FAILURE);
	}

	switch(fork()) {
		case -1:
			perror("Blad funkcji fork");
			exit(EXIT_FAILURE);
		case 0:
			// Konsument - odczytuje z bufora, zapisuje do pliku
			// Zamykamy koniec do zapisu
			close(tab_des[1]);

			// Tworzymy zmienna typu mode_t dla funkcji open określając uprw.
                        // W języku c początek "0b" oznacza liczbę binarną, a "0" oktalną, a "0x" to szesnastkowa
                        // mode_t to typ przechowujący uprw. pliku w postaci liczby całk. w jednym z formatów
                        // Sposób zapisu: <znak_systemu_liczbowego> <uprw_użytk> <uprw_grupy> <uprw_dla_innych>
                        // Każda sekcja to trzy bity: -read, -write, -execute

                        // Otwieramy plik do odczytu
                        // Argument mode ma sens tylko kiedy istnieje flaga O_CREAT
                        // inaczej jest ignorowany
                        int write_des = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (write_des == -1) {
				perror("Blad funkcji open dla konsumenta");
				exit(EXIT_FAILURE);
			}

			char bufor_1[WIELKOSC_KONSUMENT];
			ssize_t ilosc_bajt_1;

			sleep(1);

			// Wykonujemy pętlę dopóki w buforze coś jest
			// Problem może pojawić się gdy konsument odczyta pusty bufor
			// przed kolejnym wstawieniem surowca do niego przez producenta

			// Rozwiązaniem może być nałożenie czasu pomiędzy odbieraniem
			// surowca, tak żeby producent był szybszy niż konsument
			// A sam producent też ma ograniczenie na maksymalną pojemność rynku
			while((ilosc_bajt_1 = read(tab_des[0], bufor_1, WIELKOSC_KONSUMENT)) > 0) {
				printf("Konsument odebral: %s w ilosci bajtow %ld\n", bufor_1, ilosc_bajt_1);
				if (write(write_des, bufor_1, ilosc_bajt_1) != ilosc_bajt_1) {
					perror("Blad zapisu do pliku");
					exit(EXIT_FAILURE);
				}
				sleep(rand() % 2 + 1);
			}

			// Zamknięcie pliku do zapisu
			close(write_des);
			// Zamknięcie końca potoku do odczytu
			close(tab_des[0]);
			exit(EXIT_SUCCESS);
		default:
			// Producent - czyta z pliku, zapisuje do bufora
			// Zamykamy koniec do odczytu
			close(tab_des[0]);

			int read_des = open(argv[1], O_RDONLY);
			if (read_des == -1) {
				perror("Blad funkcji open dla producenta");
				exit(EXIT_FAILURE);
			}

			char bufor[WIELKOSC_PRODUCENT];
			ssize_t ilosc_bajt;

			while((ilosc_bajt = read(read_des, bufor, WIELKOSC_PRODUCENT)) > 0) {
				if (write(tab_des[1], bufor, ilosc_bajt) != ilosc_bajt) {
					perror("Blad zapisu do bufora");
					exit(EXIT_FAILURE);
				} printf("Producent wyprodukowal: %s w ilosc bajtow %ld\n", bufor, ilosc_bajt);
				sleep(rand() % 2 + 1);
			}

			close(read_des);
			close(tab_des[1]);
			wait(NULL);
	}

	exit(EXIT_SUCCESS);
}


