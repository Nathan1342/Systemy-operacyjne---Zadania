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

// Funkcja sprawdzająca czy ciąg znaków zawiera coś innego niż same cyfry
bool is_number(const char *znaki) {
        while (*znaki) {
                if(!isdigit(*znaki)) return 0;
                znaki++;
        }
        return 1;
}

int main(int argc, char *argv[]) {
	// Argv[1] to ścieżka do pliku wykonywalnego programu podpunkt_a.c
	// Argv[2] to nazwa pliku wykonywalnego programu podpunkt_a.c
	// Argv[3] to numer sygnału dla jakiego proces nr 2 wykonuje ignorowanie oraz jego procesy potomne program podpunkt_a.c
	// Argv[4] to tryb ustawienia sygnału dla procesów potomnych wykonujących program podpunkt_a.c

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

	// Proces nr 2 ustawia ignorowanie sygnału dla sygnału podanego jako argument
	if(signal(atoi(argv[3]), SIG_IGN) == SIG_ERR) {
		perror("Blad funkcji signal");
		exit(EXIT_FAILURE);
	}

	// Proces nr 2 ustawia siebie jako lidera grupy
	if(setpgid(0, 0) == -1) {
		perror("Blad funkcji setgpid");
		exit(EXIT_FAILURE);
	}

	pid_t pid_pp;
	pid_t tab_pid[3];

	int status;

	// Proces nr 2 wypisuje swój pid do indentyfikacji
	printf("Proces nr 2 - pid: %d\n", getpid());

	// Proces nr 2 wywołuje 3x funkcję fork, a każdy proces potomny uruchamia
	// program podpunkt_a
	for(int i = 0; i<3; i++) {
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
				tab_pid[i] = pid_pp;
				break;
		}
		sleep(0.5);
	}

	sleep(1);

	for(int i = 0; i<3; i++) {
		wait(&status);
		if(WIFEXITED(status)) printf("Procesu nr 2: proces potomny nr %d zakonczyl sie normalnie."
			" Pid potomnego: %d, kod wyjscia: %d\n", (i+1), tab_pid[i], WEXITSTATUS(status));
		if(WIFSIGNALED(status)) printf("Procesu nr 2: proces potomny nr %d zakonczyl sie przez sygnal."
			" Pid potomnego: %d, numer sygnalu: %d, nazwa sygnalu: %s\n", (i+1), tab_pid[i], WTERMSIG(status), my_strsignal(status));
		sleep(0.5);
	}

	// Proces nr 2 konczy sie
	return EXIT_SUCCESS;
}
