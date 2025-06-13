#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <signal.h>
#include <stdbool.h>

sem_t *sem;

bool is_number(const char *znaki) {
        while (*znaki) {
                if(!isdigit(*znaki)) return 0;
                znaki++;
        }
        return 1;
}

void exit_handler() {
	if(sem != NULL) {
		if(sem_close(sem) == -1) perror("Blad funkcji sem_close");
		if(sem_unlink("/semafor") == -1) perror("Blad funkcji sem_unlink");
	}
	_exit(EXIT_FAILURE);
}

void signal_handler(int numer) {
	printf("Zakonczenie programu przez sygnal SIGINT\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {

	atexit(exit_handler);


	int z = 0;

	FILE *file;
	file = fopen(argv[4], "w");
	fprintf(file, "%d", z);
	fclose(file);

	if(signal(SIGINT, signal_handler) == SIG_ERR) {
		perror("Blad funkcji signal");
		_exit(EXIT_FAILURE);
	}

	if(argc != 5) {
		fprintf(stderr, "Uzycie ./nazwa <ilosc_pp> <ilosc_sekcji> <sciezka_pliku> <plik_txt_numer>\n");
		_exit(EXIT_FAILURE);
	}

	// Zabezpiecznie na odpowiedni input dla argv[1], argv[2]
	if(!is_number(argv[1]) || !is_number(argv[2])) {
		fprintf(stderr, "Niepoprawny input. <ilosc_pp> oraz <ilosc_sekcji>"
				" to liczby calkowite dodatnie\n");
		_exit(EXIT_FAILURE);
	}

	// Sprawdzenie czy plik wykonalny istnieje
	if(access(argv[3], F_OK) != 0) {
		fprintf(stderr, "Plik dla programu procesow potomnych nie istnieje\n");
		_exit(EXIT_FAILURE);
	}

	// Sprawdzenie czy plik tekstowy istnieje
	if(access(argv[4], F_OK) != 0) {
		fprintf(stderr, "Plik tekstowy 'numer.txt' nie istnieje\n");
		_exit(EXIT_FAILURE);
	}

	// Sprawdzenie czy argv[1] i argv[2] są większe od 0
	if(atoi(argv[1]) < 1 || atoi(argv[2]) < 1) {
		fprintf(stderr, "<ilosc_pp> oraz <ilosc_sekcji> nie moga byc mniejsze niz 1\n");
		_exit(EXIT_FAILURE);
	}

	// Stworzenie semafora
	if((sem = sem_open("/semafor", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED) {
		perror("Blad funkcji sem_open");
		_exit(EXIT_FAILURE);
	}

	int x;
	sem_getvalue(sem, &x);
	printf("Adres semafora: %p. Wartosc poczatkowa: %d.\n", (void*)&sem, x);

	for (int i = 0; i<(atoi(argv[1])); i++) {
		switch(fork()) {
			case -1:
				perror("Blad funkcji fork");
				exit(EXIT_FAILURE);
			case 0:
				execlp(argv[3], argv[3], argv[2], argv[4], (char *)NULL);
				perror("Blad funkcji execlp");
				exit(EXIT_FAILURE);
			default:
				break;
		}
	}

	for (int i = 0; i<(atoi(argv[1])); i++) {
		wait(NULL);
	}

	int y;
	file = fopen(argv[4], "r");
        if(fscanf(file, "%d", &y) != 1) {
        	fprintf(stderr, "Blad odczytu z pliku\n");
                exit(EXIT_FAILURE);
        }
	fclose(file);
	printf("Numer wykonanych sekcji krytycznych z pliku numer.txt: %d\n", y);
	printf("Nalezyta liczba sekcji krytycznych: %d\n", atoi(argv[1])*atoi(argv[2]));

	sem_close(sem);
	sem_unlink("/semafor");
	_exit(EXIT_SUCCESS);
}
