#include <stdio.h>
#include <sys/types.h> // do identyfiaktorów, fork
#include <unistd.h> // do identyfikatorów, fork, sleep
#include <stdlib.h> // exit_failure, exit_success
#include <sys/wait.h> // dla funkcji wait

void wypisz_iden(const char *msg) {
	// Funkcja wypisuje identyfikatory: UID, GID, PID, PPID, PGID.
	// UID - iden użytkownika
	// GID - iden grupy użytk
	// PID - iden procesu
	// PPID - iden proc mac
	// PGID - iden grupy proc proc bieżacego, dla pid == 0 zwraca
	// pgid dla biez proc
	printf("%s UID: %d, GID: %d, PID: %d, PPID: %d, PGID: %d\n",
		msg, getuid(), getgid(), getpid(), getppid(), getpgid(0));
}

int main() {
	char podpunkt;
	int status; // do funkcji wait, aby wiedzieć jak zakończył się proc pot

	// Pobranie znaku do określenia podpunktu
	printf("Podaj jeden znak (a, b, c, d, e) odpowiadajacy podpunktowi\n");
	scanf(" %c", &podpunkt);


	switch(podpunkt) {
		case 'a':
			// Podpunkt a: wypisanie identyfikatorów
			wypisz_iden("Biezacy proces");
			break;
		case 'b':
			// Podpunkt b: wywołanie fork trzykrotnie,
			// wypisanie iden proc potomnych
			for(int i = 0; i<3; i++) {
				switch(fork()) {
					case -1:
						perror("fork error");
						exit(EXIT_FAILURE);
					case 0:
						// Akcja dla procesu potomn - wypisanie iden
						wypisz_iden("Proces potomny");
						exit(EXIT_SUCCESS);
					default:
						// Dalsze iteracje
						break;
				}
			}
			// Wypisanie iden proc macierz
			wypisz_iden("Proces macierzysty");

			// Czekanie na zakończenie proc potomnych
			for(int i=0; i<3; i++) {
				// Funkcja przymuje wsk do int i zapisuje status zakończenia procesu
				wait(&status);
			}
			break;
		case 'c':
			// Podpunkt c: to samo co w b) tylko sleep zamiast wait
			// Proces macierzysty zakańcza się przed proc pot
			// Są adoptowane przez init
			for(int i = 0; i<3; i++) {
                                switch(fork()) {
                                        case -1:
                                                perror("fork error");
                                                exit(EXIT_FAILURE);
                                        case 0:
                                                // Akcja dla procesu potomn - wypisanie iden
						sleep(2);
                                                wypisz_iden("Proces potomny");
                                                exit(EXIT_SUCCESS);
                                        default:
                                                // Dalsze iteracje
                                                break;
                                }
                        }
			// Proc mac śpi krócej niż potmn
			sleep(1);
                        // Wypisanie iden proc macierz
                        wypisz_iden("Proces macierzysty");
			break;
		case 'd':
			// Podpunkt d: Wypisanie procesu macierzystego przed potomnymi
			// Proc mac wypisuje swoje iden jako pierwszy
			wypisz_iden("Proces macierzysty (przodek)");
			for(int i = 0; i<3; i++) {
                                switch(fork()) {
                                        case -1:
                                                perror("fork error");
                                                exit(EXIT_FAILURE);
                                        case 0:
                                                // Akcja dla procesu potomn - wypisanie iden
                                                wypisz_iden("Proces potomny");
                                                exit(EXIT_SUCCESS);
                                        default:
                                                // Dalsze iteracje
                                                break;
                                }
                        }
			// Proc mac czeka aby na pewno zakończyły się procesy potmn
			sleep(2);
			break;
		case 'e':
			// Podpunkt e: Każdy proces potomny staje się liderem swojej
			// własnej grupy
			for(int i = 0; i<3; i++) {
                                switch(fork()) {
                                        case -1:
                                                perror("fork error");
                                                exit(EXIT_FAILURE);
                                        case 0:
                                                // Akcja dla procesu potomn - wypisanie iden
						// oraz ustawienie pgid na swój własny
						// Pierwszy arg to iden procesu dla którego funkcja ma działać jeśli pid == 0 to funkcja działa dla proc bież
						// Drugi arg odpowiada iden grupy na ktory ma zostać zmieniony poprzedni pgid == 0 to funkcja ustawia to na pid
						setpgid(0, 0);
                                                wypisz_iden("Proces potomny");
                                                exit(EXIT_SUCCESS);
                                        default:
                                                // Dalsze iteracje
                                                break;
                                }
                        }

			break;
	}
	return 0;
}
