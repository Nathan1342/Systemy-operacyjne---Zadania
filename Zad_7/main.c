#include "lib.h"

#define SHM_NAME "/pam_dziel"
#define SEM_EMPTY "/sem_empty"
#define SEM_FULL "/sem_full"
#define SEM_MUTEX "/sem_mutex"

#define NELE 20 // Liczba bajtów w jednym elemencie bufora
#define NBUF 5 // Liczba elementów w buforze

typedef struct {
	char element[NELE];
} Towar;

typedef struct {
	Towar bufor[NBUF];
	int wstaw, wyjmij;
} SegmentPD;

// Inicjalizacja wskaźników na semafory, deskryptor na pamdziel oraz identyfikatory proc
// Dla bezpieczenstwa w cleanup() tak żeby funkcja mogła sprawdzić czy oiekty istnieją
// Jest to zabezpiecznie w przypadku uruchamiania funkcji cleanup() oraz signalhandler
int shm_fd = -1;
SegmentPD *wpd = NULL;
sem_t *sem_empty = NULL;
sem_t *sem_full = NULL;
sem_t *sem_mutex = NULL;
pid_t pid_prod = -1, pid_cons = -1;

// Funkcja czyszcząca wszelkie zasoby
// Najpierw sprawdza co istnieje a co nie, i jeśli istnieje to zamyka a potem usuwa
void cleanup() {
	printf("[MAIN] Czyszczenie zasobów...\n");

	if (wpd != MAP_FAILED && wpd != NULL)
		odmapuj_pamiec(wpd, sizeof(SegmentPD));
	if (shm_fd != -1)
		zamknij_pamiec(shm_fd);
	usun_pamiec(SHM_NAME);

	if (sem_empty != SEM_FAILED) zamknij_sem_naz(sem_empty);
	if (sem_full != SEM_FAILED) zamknij_sem_naz(sem_full);
	if (sem_mutex != SEM_FAILED) zamknij_sem_naz(sem_mutex);

	usun_sem_naz(SEM_EMPTY);
	usun_sem_naz(SEM_FULL);
	usun_sem_naz(SEM_MUTEX);
}

// Sprawdzenie czy procesy isntieją, jeśli tak to wysyła sygnał zamknięcia
void sigint_handler(int signum) {
	printf("[MAIN] Otrzymano SIGINT. Zamykanie...\n");

	// Zamykanie  procesów producenta i konsumenta
	if (pid_prod > 0) kill(pid_prod, SIGINT);
	if (pid_cons > 0) kill(pid_cons, SIGINT);

	// Zaczekanie na ich zakończenie
	wait(NULL);
	wait(NULL);
	// Uruchomienie funkcji cleanup() z atexit i zamknięcie procesu
	exit(0);
}

int main() {
	// Ustawienie funkcji czyszczącej obiekty
	atexit(cleanup);
	// Ustawienie funkcji obsługującej sygnał sigint
	signal(SIGINT, sigint_handler);

	// Tworzenie pamięci dzielonej
	shm_fd = utworz_pamiec(SHM_NAME, sizeof(SegmentPD));
	printf("[MAIN] Utworzono pamiec dzielona: fd = %d, rozmiar = %ld\n", shm_fd, sizeof(SegmentPD));
	wpd = mapuj_pamiec(shm_fd, sizeof(SegmentPD));

	// Inicjalizacja struktur
	memset(wpd, 0, sizeof(SegmentPD));

	// Semafory
	// Sem_empty jest ustawiany na ilość segmentów bufora cyklicznego, i wtedy producent
	// zapisując opuszcza go, a konsument odczytując podwyższa. Służy on dla producenta
	// aby ten nie zapisywał jeśli wszystko jest pełne
	sem_t *sem_empty = stworz_sem_naz(SEM_EMPTY, NBUF);
	// Sem_full jest odwrotnością czyli liczbą zajętych miejsc. wtedy producent podwyższa
	// go. a konsument opuszcza jeśli liczba zajętych miejsc to 0 a konsument próbuje
	// go opuścić to wtedy będzie musiał poczekaż aż liczba zajętych miejsc będzie równa
	// co najmniej 1. więc ten sema to blokada dla konsumenta
	sem_t *sem_full  = stworz_sem_naz(SEM_FULL, 0);
	// Ostatni semafor, który jest semaforem binarnym, po to aby tylko jeden proces miał
	// dostęp do bufora w jednej chwili
	sem_t *sem_mutex = stworz_sem_naz(SEM_MUTEX, 1);

	if (sem_empty == SEM_FAILED || sem_mutex == SEM_FAILED) {
		perror("sem_open");
		exit(1);
	}

	// Wypisanie adresów semaforów oraz deskryptora i rozmiaru pam dziel
	printf("[MAIN] Adres sem_empty: %p\n", (void*)sem_empty);
	printf("[MAIN] Adres sem_full:  %p\n", (void*)sem_full);
	printf("[MAIN] Adres sem_mutex: %p\n", (void*)sem_mutex);
	printf("[MAIN] Deskryptor pamięci dzielonej: %d\n", shm_fd);
	printf("[MAIN] Rozmiar pamięci dzielonej: %zu bajtów\n", sizeof(SegmentPD));

	// Tworzenie procesów potomnych
	pid_prod = fork();
	if (pid_prod == 0) {
		execlp("./producent", "./producent", SHM_NAME, SEM_EMPTY, SEM_FULL, SEM_MUTEX, "plik_wej.txt", NULL);
		perror("execlp producent");
		exit(1);
	} else if (pid_prod == -1) {
		perror("fork producent");
		exit(1);
	}

	pid_cons = fork();
	if (pid_cons == 0) {
		execlp("./konsument", "./konsument", SHM_NAME, SEM_EMPTY, SEM_FULL, SEM_MUTEX, "plik_wyj.txt", NULL);
		perror("execlp konsument");
		exit(1);
	} else if (pid_cons == -1) {
                perror("fork konsument");
                exit(1);
        }

	// Czekanie na zakończenie
	wait(NULL);
	wait(NULL);

	printf("[MAIN] Proces macierzysty kończy działanie.\n");
	return 0;
}
