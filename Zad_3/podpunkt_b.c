#define _POSIX_C_SOURCE 200112L
#define _GNU_SOURCE
#include <stdlib.h> // atoi(), EXIT_FAILURE, EXIT_SUCCESS, exit()
#include <stdio.h> // printf, fprintf
#include <sys/types.h> // funkcja fork, wait, waitpid, kill
#include <unistd.h> // funkcja fork, sleep, execlp
#include <signal.h> // funkcja kill
#include <sys/wait.h> // funkcja wait, waitpid
#include <ctype.h> // funkcja isdigit
#include <stdbool.h> // zmienna typu bool

// Funkcja sprawdzająca czy ciąg znaków zawiera coś innego niż same cyfry
bool is_number(const char *znaki) {
        while (*znaki) {
                if(!isdigit(*znaki)) return 0;
                znaki++;
        }
        return 1;
}

// Funkcja działająca tak samo jak strsignal
// consta char* - wskaźnik na niemodyfikalny ciąg znaków
const char* my_strsignal(int numer) {
        // static - statyczna lokalizacja w pamięci
        static const char* signals[] = {
        [SIGHUP] = "SIGHUP",
        [SIGINT] = "SIGINT",
        [SIGQUIT] = "SIGQUIT",
        [SIGILL] = "SIGILL",
        [SIGTRAP] = "SIGTRAP",
        [SIGABRT] = "SIGABRT",
        [SIGBUS] = "SIGBUS",
        [SIGFPE] = "SIGFPE",
        [SIGKILL] = "SIGKILL",
        [SIGUSR1] = "SIGUSR1",
        [SIGSEGV] = "SIGSEGV",
        [SIGUSR2] = "SIGUSR2",
        [SIGPIPE] = "SIGPIPE",
        [SIGALRM] = "SIGALRM",
        [SIGTERM] = "SIGTERM",
        [SIGSTKFLT] = "SIGSTKFLT",
        [SIGCHLD] = "SIGCHLD",
        [SIGCONT] = "SIGCONT",
        [SIGSTOP] = "SIGSTOP",
        [SIGTSTP] = "SIGTSTP",
        [SIGTTIN] = "SIGTTIN",
        [SIGTTOU] = "SIGTTOU",
        [SIGURG] = "SIGURG",
        [SIGXCPU] = "SIGXCPU",
        [SIGXFSZ] = "SIGXFSZ",
        [SIGVTALRM] = "SIGVTALRM",
        [SIGPROF] = "SIGPROF",
        [SIGWINCH] = "SIGWINCH",
        [SIGIO] = "SIGIO",
        [SIGPWR] = "SIGPWR",
        [SIGSYS] = "SIGSYS"
        };
        return (numer >= 1 && numer <= 32 && signals[numer]) ? signals[numer] : "Unknown signal";
}

int main (int argc, char *argv[]) {

	// Obsługa błędu złej liczby argumentów
	if (argc != 5) {
		fprintf(stderr, "Uzycie ./<plik_wyk> <sciezka_popunkt_a> <nazwa_podpunkt_a"
				"<numer_sygnalu> <tryb_obsługi sygnału>\n");
		return EXIT_FAILURE;
	}

	// Obsługa błędów dla złych danych wejściowych argumentów
        if (!is_number(argv[3]) || !is_number(argv[4])) {
                fprintf(stderr, "Niepoprawne dane wejsciowe\n"
                                "Argument 3 oraz 4 to liczby calkowite\n");
                return EXIT_FAILURE;
        }

        // Obsługa błędów dla niepoprawnego kodu trybu
        if (atoi(argv[4]) != 1 && atoi(argv[4]) != 2 && atoi(argv[4]) != 3) {
                fprintf(stderr, "Niepoprawny <tryb> obslugi programu\n");
                fprintf(stderr, "Mozliwe opcje:\n1 - domyslna obsługa\n"
                                "2 - ignorowanie\n"
                                "3 - przechwycenie\n");
                return EXIT_FAILURE;
        }

        // Obsługa błędów dla niepoprawnego kodu sygnału
        if (atoi(argv[3]) < 1 || atoi(argv[3]) > 31) {
                fprintf(stderr, "Niepoprawny <numer_sygnalu>\n"
                                "Mozliwy zakres - 1-31\n");
                return EXIT_FAILURE;
	}

	pid_t pid_pp;
	int status_1, status_2;
	pid_pp = fork();

	switch(pid_pp) {
		case -1:
			perror("Blad funkcji fork");
			exit(EXIT_FAILURE);
		case 0:
			execlp(argv[1], argv[2], argv[3], argv[4], (char *)NULL);
			perror("Blad funkcji execlp");
			exit(EXIT_FAILURE);
		default:
			// po co jest default????
			break;
	}

	// Proces macierzysty czeka aż proces potomny wykona program podpunkt_a uruchomi pause
	sleep(2);
	// Proces macierzysty sprawdza czy proces potomny istnieje
	int result = waitpid(pid_pp, &status_1, WNOHANG);
	if ((result == 0)) {
		printf("Proces potomny nadal dziala\n");
	} else if (result == -1) {
		perror("Blad funkcji waitpid");
		exit(EXIT_FAILURE);
	} else {
		fprintf(stderr, "Proces potomny zakonczyl sie przedwczesnie\n");
		exit(EXIT_FAILURE);
	}
	// Proces macierzysty wysyła sygnał do procesu potomnego
	if(kill(pid_pp, atoi(argv[3])) == -1) {
		perror("Blad funkcji kill");
		exit(EXIT_FAILURE);
	};
	// Proces macierzysty czeka na zakończenie procesu potomnego
	wait(&status_2);

	// Wypisanie pid potomka
	printf("PID procesu potomnego: %d\n", pid_pp);
	// Jeśli true, oznacza to zakończenie procesu w sposób normalny
	if(WIFEXITED(status_2)) printf("Proces zakonczony normalnie, kod wyjscia %d\n", WEXITSTATUS(status_2));
	// Jeśli true, oznacza to zakończenie procesu przez sygnał
	if(WIFSIGNALED(status_2)) printf("Proces zakonczony sygnalem %d) (%s)\n", WTERMSIG(status_2), my_strsignal(WTERMSIG(status_2)));
	return EXIT_SUCCESS;
}
