#include <stdio.h> // podstawowe funkcje jak printf, perror
#include <unistd.h> // funkcja fork, execlp
#include <stdlib.h> // funkcja exit
#include <sys/types.h> // funckja fork, wait
#include <sys/wait.h> // funkcja wait


int main(int argc, char *argv[]) {
	// argc - argument count - liczba argumentów, pierszy argument to zawsze nazwa prog
	// argv - argument vector - tablica wskaźników do ciągu znaków, którymi są argumenty

	// Obsługa argumentów funkcji main
	// Domyślnie argumentów musi być co najmniej 2 (plik wykonawczy oraz plik z zad 1b)
	if (argc < 2) {
		// printf to skrót dla fprintf(stdout, ...)
		// w fprintf można określić wyjście, gdzie stdout to zwykłe wyjście
		// najczęściej terminal, a stderr to wyjście błędów
		// fprintf używa się aby błędy nie mieszały się z normalnym
		// wyjściem programu
		fprintf(stderr, "Użycie: %s <program_do_uruchomienia\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int status; // Zmienna dla funkcji wait o statusie zakończenia proc potmn

	// Trzykrotne wywołanie fork dla stworzenia 3 procesów potomnych
	// gdzie każdy uruchomi przez execlp program z wywołaniem ich iden
	for(int i = 0; i<3; i++) {
		switch(fork()) {
			case -1:
				// Błąd fork - perror wypisuje komunikat błędu
				// Perror dodaje tekst do tego co jest w niego
				// wpisane na podstawie errno
				perror("fork error");
				exit(EXIT_FAILURE);
			case 0:
				// Działanie procesu potomnego
				// Execlp uruchamia proces potomn dla nowego podanego prog
				// Pierwsz arg to nazwa programu
				// Kolejne to arg jakie ten program przyjmuje
				// Jako drugi też podaje się nazwę programu i jest on
				// traktowany jako arg[0]
				// Lista arg musi się kończyć na NULL
				execlp(argv[1], argv[2],(char *)NULL);
				// Null musi być traktowane jak wskaźnik na char, czyli
				// jak inne arguementy funkcji execlp
				perror("execlp error");
				exit(EXIT_FAILURE);
			default:
				// Działanie procesu macierzystego
				break;
		}
	}
	for(int i = 0; i<3; i++) {
		// Proces macierzysty czeka aż wszystkie 3 proc potmn zakończą
		// się po wykonaniu swoich programów
		wait(&status);
	}
	// pgid - iden grupy procesów - dla pgid(0) - pgid proc bież
	printf("Proces macierzysty: UID: %d, GID: %d, PID: %d, PPID: %d, PGID: %d\n",
		getuid(), getgid(), getpid(), getppid(), getpgid(0));
}
