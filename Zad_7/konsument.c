#include "lib.h"

#define SHM_NAME "/pam_dziel"
#define SEM_EMPTY_NAME "/sem_empty"
#define SEM_FULL_NAME "/sem_full"
#define SEM_MUTEX_NAME "/sem_mutex"

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
sem_t *sem_empty = NULL;
sem_t *sem_full = NULL;
sem_t *sem_mutex = NULL;

void cleanup() {
	if (wpd != MAP_FAILED && wpd != NULL)
		odmapuj_pamiec(wpd, sizeof(SegmentPD));
	if (sem_empty != SEM_FAILED) zamknij_sem_naz(sem_empty);
	if (sem_full != SEM_FAILED) zamknij_sem_naz(sem_full);
	if (sem_mutex != SEM_FAILED) zamknij_sem_naz(sem_mutex);
}

void sigint_handler(int signum) {
	printf("[KONSUMENT] Otrzymano SIGINT. Zamykanie...\n");
	exit(0);
}

int main(int argc, char *argv[]) {
	atexit(cleanup);
	signal(SIGINT, sigint_handler);

	int shm_fd = otworz_pamiec(argv[1]);

	wpd = mapuj_pamiec(shm_fd, sizeof(SegmentPD));

	sem_empty = otworz_sem(argv[2]);
	sem_full  = otworz_sem(argv[3]);
	sem_mutex = otworz_sem(argv[4]);

	// Zmienne sem_empty, sem_full to wskaźniki typu sem_t*
	// Więc musimy zrobić rzutowanie z sem_t* na ogólny wskaźnik typu void
	// Aby kompilator nie miał problemu z %p
	printf("[KONSUMENT] Adres sem_empty: %p\n", (void*)sem_empty);
	printf("[KONSUMENT] Adres sem_full:  %p\n", (void*)sem_full);
	printf("[KONSUMENT] Adres sem_mutex: %p\n", (void*)sem_mutex);
	printf("[KONSUMENT] Deskryptor pamięci dzielonej: %d\n", shm_fd);

	int fd = open(argv[5], O_WRONLY | O_CREAT | O_TRUNC, 0666);
	char buf[NELE];

	while (1) {
		zmien_wart_sem(sem_full, -1);
		zmien_wart_sem(sem_mutex, -1);

		// Obliczanie aktualnych wartości semaforów
		int val_empty, val_full, val_mutex;
		wartosc_sem(sem_empty, &val_empty);
		wartosc_sem(sem_full, &val_full);
		wartosc_sem(sem_mutex, &val_mutex);

		// Obliczenie ile bajtów zostało odczytanych
		size_t real_size = strlen(buf);

		memcpy(buf, wpd->bufor[wpd->wyjmij].element, NELE);
		printf("[KONSUMENT] Indeks: %d, Sem_empty: %d, Sem_full: %d, Sem_mutex: %d, Bajty: %zu, Tekst: %s\n", wpd->wyjmij, val_empty, val_full, val_mutex, real_size, buf);
		wpd->wyjmij = (wpd->wyjmij + 1) % NBUF;

		zmien_wart_sem(sem_mutex, 1);
		zmien_wart_sem(sem_empty, 1);

		sleep(2);

		if (buf[0] == '\0') break; // Sygnał końca danych

		write(fd, buf, NELE);
	}

	return 0;
}
