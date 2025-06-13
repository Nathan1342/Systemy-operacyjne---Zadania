// Funkcja do tworzenia semafora nazwanego
sem_t *stworz_sem_naz(const char *nazwa, unsigned int wartosc) {
	// Sprawdzenie czy nazwa jest poprawna
	CheckError_1(name && name[0] == '/' && name[1] != '\0');
	// Sprawdzenie poprawności wartości
	CheckError_1(wartosc > 0);
	// Wywołanie funkcji
	sem_t = *adres;
	CheckError_1((adres = sem_open(nazwa, O_CREAT | O_EXCL, 0644, wartosc)) != -1);
	return adres;
}

// Funkcja do tworzenia semafora nienazwanego
int stworz_sem_nienaz(sem_t *adres, int dzielony, unsigned int wartosc) {
	// Wywołanie funkcji
	CheckError(sem_init(adres, dzielony, wartosc) != -1);
	return 1;
}

// Funkcja do otwierania semafora
sem_t *otworz_sem(const char *nazwa) {
	// Wywołanie funkcji
	sem_t *adres;
	CheckError_1((adres = sem_open(nazwa, 0)) != SEM_FAILED);
	return adres;
}

// Funkcja do zamykania semafora nazwanego
int zamknij_sem_naz(sem_t *sem) {
	CheckError(sem_close(sem) != -1);
	return 1;
}

// Funkcja do zamykania semafora nienazwanego
int usun_sem_nienaz(sem_t *sem) {
	CheckError(sem_destroy(sem) != -1);
	return 1;
}

// Funkcja do uzyskiwania wartości semafora
int wartosc_sem(sem_t *adres, int *wartosc) {
	CheckError(sem_getvalue(adres, wartosc) != -1);
	return 1;
}

// Funkcja do usuwania semafora
int usun_sem_naz(const char *nazwa) {
	CheckError(sem_unlink(nazwa) != -1);
	return 1;
}

// Funkcja do operowania semafora
int zmien_wart_sem(sem_t *sem, int wartosc) {
	CheckError(wartosc != 0);
	if (wartosc > 0) {
		for (int i = 0; i<wartosc; i++) {
			CheckError(sem_post(sem) != -1);
		}
	} else {
		for (int i = 0; i<wartosc; i++) {
                        CheckError(sem_wait(sem) != -1);
                }
	}
	return 1;
}
