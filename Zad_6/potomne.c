#include <stdio.h>
#include <stdlib.h> // rand
#include <unistd.h> // fork
#include <fcntl.h> // sem_open
#include <semaphore.h> // sem_open, sem_wait, sem_post
#include <sys/types.h> // fork




int main(int argc, char *argv[]) {

	sem_t *sema;
	FILE *file;
	int x, y;

	if((sema = sem_open("/semafor", 0)) == SEM_FAILED) {
		perror("Blad funkcji sem_open");
		exit(EXIT_FAILURE);
	}


	for(int i = 0; i<(atoi(argv[1])); i++) {

		sem_getvalue(sema, &y);
		printf("Wartosc semafora przed sekcja krytyczna: %d, dla PID: %d\n", y, getpid());
		// Opuszczenie semafora
		sem_wait(sema);

		// Sekcja krytyczna
		sem_getvalue(sema, &x);
		printf("Proces potomny %d wchodzi do sekcji krytycznej, wartosc semafora: %d\n", getpid(), x);
		// Odczytaj z pliku aktualny numer
		file = fopen(argv[2], "r");
		if(fscanf(file, "%d", &x) != 1) {
			fprintf(stderr, "Blad odczytu z pliku\n");
			exit(EXIT_FAILURE);
		}
		printf("Numer wykonywanej sekcji krytycznej: %d\n", x);
		x = x + 1;
		fclose(file);
		sleep(rand() % 2 + 1);
		// Zapisz do pliku nową liczbę
		file = fopen(argv[2], "w");
		fprintf(file, "%d", x);
		fclose(file);

		// Podniesienie semafora
		sem_post(sema);
		sem_getvalue(sema, &x);
		printf("Proces potomny %d wyszedl z sekcji krytycznej, wartosc semafora: %d\n", getpid(), x);
		sleep(1);
	}

}
