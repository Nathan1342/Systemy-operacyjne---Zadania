#include <stdio.h>
#include <stdlib.h> // exit
#include <semaphore.h> // sem_open
#include <fcntl.h> // sem_open
#include <sys/stat.h> // sem_open
#include <unistd.h> // fork, _exit
#include <sys/types.h> // fork, wait
#include <sys/wait.h> // wait

sem_t *sem;

void exit_handler() {
	if(sem_open("/semafor", 0) == SEM_FAILED) {
		sem_close(sem);
		sem_unlink("/semafor");
	}
}

int main() {

	atexit(exit_handler);

	if((sem = sem_open("/semafor", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED) {
		perror("Blad funkcji sem_open");
		_exit(EXIT_FAILURE);
	}

	switch(fork()) {
		case -1:
			perror("Blad funkcji fork");
			exit(EXIT_FAILURE);
		case 0:
			sleep(1);

			for(int i = 0; i<3; i++) {

			printf("Proces potomny czeka na pamiec\n");
			sem_wait(sem);
			printf("Proces potomny sekcja krytyczna\n");
			sleep(2);
			printf("Proces potomny wychodzi z sekcji krytycznej\n");
			sem_post(sem);

			}
			exit(EXIT_SUCCESS);
		default:
			sleep(1);

			for (int i = 0; i<3; i++) {

			printf("Proces macierzysty czeka na pamiec\n");
			sem_wait(sem);
                        printf("Proces macierzysty sekcja krytyczna\n");
                        sleep(2);
			printf("Proces macierzysty wychodzi z sekcji krytycznej\n");
                        sem_post(sem);

			}
			break;
	}

	wait(NULL);
	sem_close(sem);
	sem_unlink("/semafor");
	exit(EXIT_SUCCESS);
}
