#ifndef LIB_H
#define LIB_H

#define _POSIX_C_SOURCE 200112L
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>

// --------------------- CHECKERROR MAKRA ---------------------

#define CheckError(Arg) \
	do { \
		if (!(Arg)) { \
			fprintf(stderr, "[ERROR] %s:%d (%s) -> CheckError argument: %s\n", \
					__FILE__, __LINE__, __func__, #Arg); \
			exit(EXIT_FAILURE); \
		} \
	} while (0)

#define CheckError_1(Arg) \
	do { \
		if (!(Arg)) { \
			fprintf(stderr, "[ERROR] %s:%d (%s) -> CheckError_1 argument: %s\n", \
					__FILE__, __LINE__, __func__, #Arg); \
		return NULL; \
		} \
	} while (0)

// --------------------- SEMAFORY ---------------------

sem_t *stworz_sem_naz(const char *nazwa, unsigned int wartosc);
int stworz_sem_nienaz(sem_t *adres, int dzielony, unsigned int wartosc);
sem_t *otworz_sem(const char *nazwa);
int zamknij_sem_naz(sem_t *sem);
int usun_sem_nienaz(sem_t *sem);
int usun_sem_naz(const char *nazwa);
int wartosc_sem(sem_t *adres, int *wartosc);
int zmien_wart_sem(sem_t *sem, int wartosc);

// --------------------- PAMIEC DZIELONA ---------------------

int utworz_pamiec(const char *nazwa, size_t rozmiar);
int otworz_pamiec(const char *nazwa);
int usun_pamiec(const char *nazwa);
int zamknij_pamiec(int shm_fd);
void *mapuj_pamiec(int fd, size_t rozmiar);
int odmapuj_pamiec(void *adres, size_t rozmiar);

#endif // LIB_H
