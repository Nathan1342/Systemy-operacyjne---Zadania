#define _POSIX_C_SOURCE 200112L
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h> // dla funkcji isdigit
#include <stdbool.h> // dla typu bool

// Funkcja sprawdzająca czy ciąg znaków zawiera coś innego niż same cyfry
bool is_number(const char *znaki) {
        while (*znaki) {
                if(!isdigit(*znaki)) return 0;
                znaki++;
        }
        return 1;
}

int main(int argc, char *argv[]) {
	// Argv[1] to ścieżka do pliku wykonywalnego programu podpunkt_c_2.c
	// Argv[2] to nazwa pliku wykonywalnego programu podpunkt_c_2.c
	// Argv[3] to ścieżka do pliku wykonywalnego programu podpunkt_a.c
	// Argv[4] to nazwa pliku wykonywalnego programu podpunkt_a.c
	// Argv[5] to numer sygnał dla jakiego proces nr 2 wykona ignorowanie oraz jego procesy potomne dany tryb ustawienia
	// Argv[6] to tryb ustawienia sygnału dla procesów potomnych procesu nr 2

	// Obsługa błędu złej liczby argumentów
        if (argc != 7) {
                fprintf(stderr, "Uzycie ./<plik_wyk> <sciezka_popunkt_a> <nazwa_podpunkt_a"
                                "<numer_sygnalu> <tryb_obsługi sygnału>\n");
                return EXIT_FAILURE;
        }

        // Obsługa błędów dla złych danych wejściowych argumentów
        if (!is_number(argv[5]) || !is_number(argv[6])) {
                fprintf(stderr, "Niepoprawne dane wejsciowe\n"
                                "Argument 5 oraz 6 to liczby calkowite\n");
                return EXIT_FAILURE;
        }

        // Obsługa błędów dla niepoprawnego kodu trybu
        if (atoi(argv[6]) != 1 && atoi(argv[6]) != 2 && atoi(argv[6]) != 3) {
                fprintf(stderr, "Niepoprawny <tryb> obslugi programu\n");
                fprintf(stderr, "Mozliwe opcje:\n1 - domyslna obsługa\n"
                                "2 - ignorowanie\n"
                                "3 - przechwycenie\n");
                return EXIT_FAILURE;
        }

        // Obsługa błędów dla niepoprawnego numeru sygnału
        if (atoi(argv[5]) < 1 || atoi(argv[5]) > 31) {
                fprintf(stderr, "Niepoprawny <numer_sygnalu>\n"
                                "Mozliwy zakres - 1-31\n");
                return EXIT_FAILURE;
        }

	int status;
	pid_t pid_pp;

	// Proces nr 1 wypisuje swój pid
	printf("Proces nr 1 - pid: %d\n", getpid());

	pid_pp = fork();

	switch(pid_pp) {
		case -1:
			perror("Blad funkcji fork");
			exit(EXIT_FAILURE);
		case 0:
			execlp(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], (char *)NULL);
			perror("Blad funkcji execlp");
			exit(EXIT_FAILURE);
		default:
			break;
	}

	//Proces nr 1 czeka na stworzenie trzech procesów potomnych procesu nr 2
	// oraz na wejście w funkcję wait()
	sleep(4);

	printf("Proces nr 1: Wysylanie sygnalu\n");
	if(kill(-pid_pp, atoi(argv[5])) == -1) {
		perror("Blad funkcji kill");
		exit(EXIT_FAILURE);
	}

	// Proces nr 1 czeka na zakończenie się procesu nr 2 i jego procesów potomnych
	wait(&status);

	// Proces nr 2 wypisuje pid i sposób zakończenia
	printf("Procesu nr 1: pid procesu nr 2 - %d\n", pid_pp);
	if(WIFEXITED(status)) printf("Proces zakonczyl sie normalnie, kod wyjscia: %d\n", WEXITSTATUS(status));

	return EXIT_SUCCESS;
}
