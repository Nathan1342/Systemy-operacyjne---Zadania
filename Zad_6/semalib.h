#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>

#define CheckError(Arg) \
        do { \
                if(!Arg) { \
                        fprintf(stderr, "[ERROR] %s:%d (%s) -> CheckError arguement: %s\n", \
                        __FILE__, __LINE__, __func__, #Arg); \
                } \
		return 0;
        } while (0)

#define CheckError_1(Arg) \
        do { \
                if(!Arg) { \
                        fprintf(stderr, "[ERROR] %s:%d (%s) -> CheckError arguement: %s\n", \
                        __FILE__, __LINE__, __func__, #Arg); \
                } \
                return NULL;
        } while (0)

int zmien_wart_sem(sem_t *sem, int wartosc);
int usun_sem_naz(const char *nazwa);
int wartosc_sem(sem_t *adres, int *wartosc);
int usun_sem_nienaz(sem_t *sem);
int zamknij_sem_naz(sem_t *sem);
sem_t *otworz_sem(const char *nazwa);
int stworz_sem_nienaz(sem_t *adres, int dzielony, unsigned int wartosc);
sem_t *stworz_sem_naz(const char *nazwa, unsigned int wartosc);
