#include "lib.h"

#define NELE 20
#define NBUF 5

typedef struct {
	char element[NELE];
} Towar;

typedef struct {
	Towar bufor[NBUF];
	int wstaw, wyjmij;
} SegmentPD;

SegmentPD *wpd = NULL;
// Sema zapobiegający nadpisywaniu danych które nie zostały jescze przetworzone przez kons
sem_t *sem_empty = NULL;
// Sema zapobiegający próby odczytu z pustego bufora
sem_t *sem_full = NULL;
// Sema zapobiegający sytuacji w której oba procesy mają dostęp do bufora na raz
sem_t *sem_mutex = NULL;

void cleanup() {
	if (wpd != MAP_FAILED && wpd != NULL)
		odmapuj_pamiec(wpd, sizeof(SegmentPD));
	if (sem_empty != SEM_FAILED) zamknij_sem_naz(sem_empty);
	if (sem_full != SEM_FAILED) zamknij_sem_naz(sem_full);
	if (sem_mutex != SEM_FAILED) zamknij_sem_naz(sem_mutex);
}

void sigint_handler(int signum) {
	printf("[PRODUCENT] Otrzymano SIGINT. Zamykanie...\n");
	exit(0);
}

int main(int argc, char *argv[]) {

	atexit(cleanup);
	signal(SIGINT, sigint_handler);

	int shm_fd = shm_open(argv[1], O_RDWR, 0666);
	if (shm_fd == -1) {
		perror("shm_open");
		exit(1);
	}

	wpd = mapuj_pamiec(shm_fd, sizeof(SegmentPD));

	sem_empty = otworz_sem(argv[2]);
	sem_full  = otworz_sem(argv[3]);
	sem_mutex = otworz_sem(argv[4]);

	printf("[PRODUCENT] Adres sem_empty: %p\n", (void*)sem_empty);
	printf("[PRODUCENT] Adres sem_full:  %p\n", (void*)sem_full);
	printf("[PRODUCENT] Adres sem_mutex: %p\n", (void*)sem_mutex);
	printf("[PRODUCENT] Deskryptor pamięci dzielonej: %d\n", shm_fd);

	int fd = open(argv[5], O_RDONLY);
	char buf[NELE];
	ssize_t bytes_read;

	while ((bytes_read = read(fd, buf, NELE)) > 0) {
		zmien_wart_sem(sem_empty, -1);
		zmien_wart_sem(sem_mutex, -1);

		// Obliczanie aktualnych wartości semaforów
		int val_empty, val_full, val_mutex;
		wartosc_sem(sem_empty, &val_empty);
		wartosc_sem(sem_full, &val_full);
		wartosc_sem(sem_mutex, &val_mutex);

		// Kopiuje bytes_read z lokalnego buf do bufora dzielonego w pozycji wpd->wstaw
		// wpd to wskaźnik na segment pamięci dzielonej
		memcpy(wpd->bufor[wpd->wstaw].element, buf, bytes_read);
		printf("[PRODUCENT] Indeks %d, Sem_empty: %d, Sem_full: %d, Sem_mutex: %d, Bajty: %d, Tekst: %s\n", wpd->wstaw, val_empty, val_full, val_mutex, (int)bytes_read, buf);

		wpd->wstaw = (wpd->wstaw + 1) % NBUF;

		zmien_wart_sem(sem_mutex, 1);
		zmien_wart_sem(sem_full, 1);

		sleep(1);
	}

	zmien_wart_sem(sem_empty, -1);
	zmien_wart_sem(sem_mutex, -1);
	// Funkcja wstawiająca \0 na koniec bufora dzielonego aby wskazać że to koniec towaru
	// memset(dest, value, len) ustawia len bajtów w pamięci dest na wartość value
	// Więc tutaj cały jeden segment zapełnia się zerami
	memset(wpd->bufor[wpd->wstaw].element, 0, NELE); // Wstawienie znaku '\0' jako sygnału końca
	wpd->wstaw = (wpd->wstaw + 1) % NBUF;
	zmien_wart_sem(sem_mutex, 1);
	zmien_wart_sem(sem_full, 1);

	close(fd);

	return 0;
}
