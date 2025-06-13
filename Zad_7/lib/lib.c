#include "lib.h"
// ======================= SEMAFORY =======================

sem_t *stworz_sem_naz(const char *nazwa, unsigned int wartosc) {
	CheckError_1(nazwa && nazwa[0] == '/' && nazwa[1] != '\0');
	CheckError_1(wartosc >= 0);
	sem_t *adres;
	CheckError_1((adres = sem_open(nazwa, O_CREAT | O_EXCL, 0644, wartosc)) != SEM_FAILED);
	return adres;
}

int stworz_sem_nienaz(sem_t *adres, int dzielony, unsigned int wartosc) {
	CheckError(sem_init(adres, dzielony, wartosc) != -1);
	return 1;
}

sem_t *otworz_sem(const char *nazwa) {
	sem_t *adres;
	CheckError_1((adres = sem_open(nazwa, 0)) != SEM_FAILED);
	return adres;
}

int zamknij_sem_naz(sem_t *sem) {
	CheckError(sem_close(sem) != -1);
	return 1;
}

int usun_sem_naz(const char *nazwa) {
	CheckError(sem_unlink(nazwa) != -1);
	return 1;
}

int usun_sem_nienaz(sem_t *sem) {
	CheckError(sem_destroy(sem) != -1);
	return 1;
}

int wartosc_sem(sem_t *adres, int *wartosc) {
	CheckError(sem_getvalue(adres, wartosc) != -1);
	return 1;
}

int zmien_wart_sem(sem_t *sem, int wartosc) {
	CheckError(wartosc != 0);
	if (wartosc > 0) {
		for (int i = 0; i < wartosc; i++) {
			CheckError(sem_post(sem) != -1);
		}
	} else {
		for (int i = 0; i < -wartosc; i++) {
			CheckError(sem_wait(sem) != -1);
		}
	}
	return 1;
}

// ======================= PAMIEC DZIELONA =======================

int utworz_pamiec(const char *nazwa, size_t rozmiar) {
	int fd = shm_open(nazwa, O_CREAT | O_RDWR, 0666);
	CheckError(fd != -1);
	CheckError(ftruncate(fd, rozmiar) != -1);
	return fd;
}

int otworz_pamiec(const char *nazwa) {
	int fd = shm_open(nazwa, O_RDWR, 0666);
	CheckError(fd != -1);
	return fd;
}

void *mapuj_pamiec(int fd, size_t rozmiar) {
	void *ptr = mmap(NULL, rozmiar, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	CheckError_1(ptr != MAP_FAILED);
	return ptr;
}

int odmapuj_pamiec(void *adres, size_t rozmiar) {
	CheckError(munmap(adres, rozmiar) != -1);
	return 1;
}

int zamknij_pamiec(int fd) {
	CheckError(close(fd) != -1);
	return 1;
}

int usun_pamiec(const char *nazwa) {
	CheckError(shm_unlink(nazwa) != -1);
	return 1;
}
