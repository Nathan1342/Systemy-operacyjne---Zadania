#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE   700   /* dla usleep i POSIX */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>    /* dla usleep() */
#include <time.h>      /* dla time() */

typedef struct {
	int thread_num;          /* numer wątku (1..num_threads) */
	int num_sections;        /* ile sekcji krytycznych ma wykonać ten wątek */
	pthread_mutex_t *mutex;  /* wskaźnik na wspólny muteks */
	int *global_counter;     /* wskaźnik na wspólny licznik */
} thread_arg_t;

/* Funkcja wywoływana przez każdy wątek */
void *thread_func(void *arg) {
	thread_arg_t *t = (thread_arg_t *)arg;
	int thr_num = t->thread_num;
	int num_sec = t->num_sections;
	pthread_mutex_t *mx = t->mutex;
	int *glob_count = t->global_counter;

	/* Wiersz, w którym ma być wątek */
	int row = thr_num;
	/* Kolumna, w której piszemy sekcję prywatną */
	int col_private = 1;
	/* Kolumna, w której piszemy sekcję krytyczną */
	int col_critical = 31;

	sleep(3);
	printf("\033[%d;%dH", row, col_private);
	printf("sekcja prywatna: wątek nr %d  ", thr_num);
	fflush(stdout);

	for (int i = 1; i <= num_sec; ++i) {
		/* Sekcja prywatna */
        	/* Ustawiamy kursor w (row, col_private) i wypisujemy tekst */

		sleep(1);

		/*  Sekcja krytyczna   */
		int err = pthread_mutex_lock(mx);
		if (err != 0) {
			fprintf(stderr, "pthread_mutex_lock (wątek %d): %s\n", thr_num, strerror(err));
			pthread_exit(NULL);
		}

		/* Najpierw kasujemy dotychczasowy tekst „sekcja prywatna” (lewa strona) */
		printf("\033[%d;1H\033[K", row);
		fflush(stdout);
		/* Ustawiamy kursor w (row, col_critical) i wypisujemy tekst sekcji krytycznej */
		printf("\033[%d;%dH", row, col_critical);
		printf("sekcja krytyczna nr: %d wątku nr: %d", i, thr_num);
		fflush(stdout);
		sleep(2);

		/* Inkrementujemy wspólny licznik */
		(*glob_count)++;

		err = pthread_mutex_unlock(mx);
		if (err != 0) {
			fprintf(stderr, "pthread_mutex_unlock (wątek %d): %s\n", thr_num, strerror(err));
			pthread_exit(NULL);
		}
		printf("\033[%d;%dH\033[K", row, col_private);
		printf("sekcja prywatna: wątek nr %d  ", thr_num);
		fflush(stdout);
		sleep(1);
	}
	return NULL;
}

int main(int argc, char *argv[]) {

	if (argc != 3) {
		fprintf(stderr, "Użycie: %s <liczba_wątków> <liczba_sekcji_krytycznych>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int num_threads = atoi(argv[1]);
	int num_sections = atoi(argv[2]);

	if (num_threads <= 0 || num_sections <= 0) {
		fprintf(stderr, "Oba argumenty muszą być liczbami calkowitymi dodatnimi.\n");
		exit(EXIT_FAILURE);
	}

	// Tworzenie mutexu
	pthread_mutex_t mutex;
	int err = pthread_mutex_init(&mutex, NULL);
	if (err != 0) {
		fprintf(stderr, "pthread_mutex_init: %s\n", strerror(err));
		exit(EXIT_FAILURE);
	}

	int global_counter = 0;

	printf("Adres utworzonego muteksu: %p\n", (void *)&mutex);
	printf("Liczba wątków: %d, liczba sekcji na wątek: %d\n", num_threads, num_sections);
	printf("Tworzenie wątków:\n");

	pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
	if (threads == NULL) {
		perror("malloc (threads)");
		pthread_mutex_destroy(&mutex);
		exit(EXIT_FAILURE);
	}

	thread_arg_t *targs = malloc(num_threads * sizeof(thread_arg_t));
	if (targs == NULL) {
		perror("malloc (targs)");
		free(threads);
		pthread_mutex_destroy(&mutex);
		exit(EXIT_FAILURE);
	}

	// Tworzenie wątków
	for (int i = 0; i < num_threads; ++i) {
		/* Wypełniamy pola struktury dla wątku i */
		targs[i].thread_num  = i + 1;              /* numerujemy od 1 */
		targs[i].num_sections   = num_sections;    /* ile sekcji krytycznych wykona */
		targs[i].mutex          = &mutex;          /* wspólny muteks */
		targs[i].global_counter = &global_counter; /* wskaźnik na wspólny licznik */

		err = pthread_create(&threads[i], NULL, thread_func, (void *)&targs[i]);
		if (err != 0) {
			fprintf(stderr, "pthread_create (wątek %d): %s\n", i + 1, strerror(err));
			threads[i] = 0; /* oznaczamy, że tego wątku nie udało się utworzyć */
		} else {
			printf("  -> Wątek %2d utworzony, pthread_t ID = %lu\n", i + 1, (unsigned long)threads[i]);
		}
	}

	printf("\033[2J");

	// Czekanie za zakończenie wątków
	for (int i = 0; i < num_threads; ++i) {
		if (threads[i] != 0) {
			err = pthread_join(threads[i], NULL);
			if (err != 0) {
				fprintf(stderr, "pthread_join (wątek %d): %s\n", i + 1, strerror(err));
			}
		}
	}

	// Usuwanie mutexu
	err = pthread_mutex_destroy(&mutex);
	if (err != 0) {
		fprintf(stderr, "pthread_mutex_destroy: %s\n", strerror(err));
	}

	printf("\033[%d;1H==== Program zakończony ====\n", num_threads + 2);
	printf("Wartość globalnego licznika: %d\n", global_counter);
	printf("Oczekiwana wartość: %d  (num_threads * num_sections)\n", num_threads*num_sections);

	free(threads);
	free(targs);

	return 0;
}
