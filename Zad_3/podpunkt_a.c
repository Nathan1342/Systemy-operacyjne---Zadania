#define _GNU_SOURCE // dodaje wszelkie dostępne rozszerzenia GNU
#define _XOPEN_SOURCE 500 // włącza dodatkowe funkcje do nagłówków zgodnie ze standardem XOPEN
#include <stdio.h> // funkcja perror()
#include <stdlib.h> // funkcja atoi() - zmiana string na int, exit()
#include <signal.h> // funkcja signal(), makra SIG_DFL i SIG_IGN
#include <unistd.h> // funkcja pause()
#include <ctype.h> // funkcja isdigit()
#include <stdbool.h> // dla typu bool
#include <string.h> // funkcja strsignal

// Prototyp funkcji my_strsignal
const char* my_strsignal(int numer);

// Funkcja wykonująca wybraną akcję przy przechwyceniu sygnału
// Tutaj to wypisanie numeru sygnału oraz jego nazwy
void handle(int numer) {
	printf("Przechwycono sygnał: %d) (%s)\n", numer, my_strsignal(numer));
	sleep(3);
}

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
	return signals[numer];
}

int main(int argc, char *argv[]) {

	// Obsługa błędów dla innej niż 3 liczby argumentów
	if (argc != 3) {
		fprintf(stderr, "Uzycie %s <numer_sygnalu> <tryb>\n", argv[0]);
		fprintf(stderr, "Tryb 1 - domyślna obsługa sygnalu\n"
				"Tryb 2 - ignorowanie sygnalu\n"
				"Tryb 3 - przechwycenie sygnalu\n");
		return EXIT_FAILURE;
	}

	// Obsługa błędów dla złych danych wejściowych argumentów
	if (!is_number(argv[1]) || !is_number(argv[2])) {
		fprintf(stderr, "Niepoprawne dane wejsciowe\n"
				"Argument 1 oraz 2 to liczby calkowite\n");
		return EXIT_FAILURE;
	}

	// Obsługa błędów dla niepoprawnego kodu trybu
	if (atoi(argv[2]) != 1 && atoi(argv[2]) != 2 && atoi(argv[2]) != 3) {
		fprintf(stderr, "Niepoprawny <tryb> obslugi programu\n");
		fprintf(stderr, "Mozliwe opcje:\n1 - domyslna obsługa\n"
				"2 - ignorowanie\n"
				"3 - przechwycenie\n");
		return EXIT_FAILURE;
	}

	// Obsługa błędów dla niepoprawnego kodu sygnału
	if (atoi(argv[1]) < 1 || atoi(argv[1]) > 31) {
		fprintf(stderr, "Niepoprawny <numer_sygnalu>\n"
				"Mozliwy zakres - 1-31\n");
		return EXIT_FAILURE;
	}

	// Przekonwerterowanie argumentów wektora ciągu znaków na liczby całkowite
	int numer = atoi(argv[1]);
	int tryb = atoi(argv[2]);

	// Wypisanie identyfikatora procesu dla późniejszego wywołania funkcji kill
	printf("Identyfikator procesu to %d\n", getpid());

	// Funkcja switch obsługująca tryb obsługi sygnału
	switch(tryb) {
		// Domyślna obsługa sygnału
		case 1:
			printf("Domyslna obsluga sygnalu.\n");
			if (signal(numer, SIG_DFL) == SIG_ERR) {
				perror("Blad funkcji signal");
				exit(EXIT_FAILURE);
			}
			break;
		// Ignorowanie sygnału
		case 2:
			printf("Ignorowanie sygnalu.\n");
			if (signal(numer, SIG_IGN) == SIG_ERR) {
				perror("Blad funkcji signal");
				exit(EXIT_FAILURE);
			}
			break;
		// Przechwycenie sygnału
		case 3:
			printf("Przechwycenie sygnalu.\n");
			if (signal(numer, handle) == SIG_ERR) {
				perror("Blad funkcji signal");
				exit(EXIT_FAILURE);
			}
			break;
	}

	// Oczekiwanie na sygnał
	pause();

	return EXIT_SUCCESS;
}

