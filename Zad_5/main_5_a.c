#define _GNU_SOURCE
#include <stdio.h> // fprintf, perror
#include <stdlib.h> // exit, EXIT_FAILURE, EXIT_SUCCESS, rand
#include <unistd.h> // fork, exec, unlink
#include <sys/types.h> // fork, mkfifo, wait
#include <sys/stat.h> //mkfifo, struct stat, stat()
#include <signal.h> // signal, makra WIFEXITED
#include <sys/wait.h> // wait

void usun_potok() {
	if (unlink("potok") == -1) {
		perror("Blad funkcji unlink, usuniecia potoku");
	}
}

void handler(int numer) {
	// Specjalna struktura dla funkcji stat()
	struct stat info;


	if(access("potok", F_OK) == 0) {
		//Funkcja stat() służy do pobieranie informacji o pliku lub katalogu
		// Przyjmuje ścieżkę do pliku oraz wskaźnik na specjalną strukturę która jest w bibliotece sys/stat
		// Zwraca 0 jeśli pomyślnie zapisano info o pliku w strukturze lub standardowo -1 dla porażki
		if (stat("potok", &info) == 0) {
			if(S_ISFIFO(info.st_mode)) {
				// Plik o nazwie "potok" to jest potok
				if (unlink("potok") == -1) {
					perror("Blad funkcji unlink, usuniecia potoku");
				}
			} else {
				// Plik o nazwie "potok" to nie jest potok
				fprintf(stderr, "Podany plik to nie potok fifo\n");
			}
		} else {
			perror("Blad funkcji stat");
		}
	} else {
		printf("Potok zostal wczesniej usuniety\n");
	}

	// Proces macierzysty czeka na oba procesy potomne aby się zakonczyły
	for (int i = 0; i<2; i++) {
		wait(NULL);
	}
	printf("Program zakonczyl sie przez sygnal SIGINT\n");
	_exit(EXIT_SUCCESS);
}


// Argumenty: argv[1] - konsument, argv[2] - producent, argv[3] - plik wej, argv[4] - plik wyj
int main(int argc, char *argv[]) {

	// Funkcja do czyszczenia potoku
	atexit(usun_potok);

	// Obsługa sygnału SIGINT
	if (signal(SIGINT, handler) == SIG_ERR) {
		perror("Blad funkcji signal");
		_exit(EXIT_FAILURE);
	}

	if(argc != 5) {
		fprintf(stderr, "Uzycie: ./<nzw_plik_wyk> <sciezka_prg_1>"
				" <sciezka_prg_2> <plik_wej> <plik_wyj>\n");
		_exit(EXIT_FAILURE);
	}

	// F_OK - czy plik istnieje
	// R_OK - czy plik jest czytelny
	// W_OK - czy plik jest zapisywalny
	// X_OK - czy plik jest wykonalny
	if (access(argv[1], F_OK) != 0) {
		fprintf(stderr, "Plik %s nie istenieje\n", argv[1]);
		_exit(EXIT_FAILURE);
	}

	if (access(argv[2], F_OK) != 0) {
		fprintf(stderr, "Plik %s nie istenieje\n", argv[2]);
		_exit(EXIT_FAILURE);
	}

	// Stworzenie potoku
	if (mkfifo("potok", 0644) == -1) {
		perror("Blad funkcji mkfifo");
		_exit(EXIT_FAILURE);
	}

	for (int i = 0; i<2; i++) {
		switch(fork()) {
			case -1:
				perror("Blad funkcji fork");
				exit(EXIT_FAILURE);
			case 0:
				switch(i) {
					case 0:
						// Uruchomienie konsumenta
						execlp(argv[1],argv[1], argv[4], "1", "nic", (char *)NULL);
						perror("Blad funkcji execlp dla konsumenta");
						exit(EXIT_FAILURE);
					case 1:
						// Uruchomienie producenta
						execlp(argv[2], argv[2], argv[3], "1", "nic", (char *)NULL);
						perror("Blad funkcji execlp dla producenta");
						exit(EXIT_FAILURE);
				}
			default:
				break;
		}
	}

	int tab_status[2];
	for (int i = 0; i<2; i++) {
		wait(&tab_status[i]);
		if(WIFEXITED(tab_status[i])) printf("Proces potmn nr %d zakonczyl sie normalnie, kod wyjscia %d\n", i+1, WEXITSTATUS(tab_status[i]));
		if(WIFSIGNALED(tab_status[i])) printf("Proces potmn nr %d zakonczyl sie przez sygnal, kod sygnalu: %d\n", i+1, WTERMSIG(tab_status[i]));
	}

	exit(EXIT_SUCCESS);
}
