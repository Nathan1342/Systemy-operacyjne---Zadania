#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include "common.h"

mqd_t client_queue;
mqd_t server_queue;
char client_queue_name[64];

void cleanup() {
	mq_close(client_queue);
	mq_unlink(client_queue_name);
	mq_close(server_queue);
	printf("Klient: zamknięto i usunięto kolejki\n");
}

void sigint_handler(int signum) {
	exit(0);
}

int main() {
	pid_t pid = getpid();
	snprintf(client_queue_name, sizeof(client_queue_name), "/%d", pid);

	struct mq_attr attr;
	attr.mq_flags = 0;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = MAX_MSG_SIZE;
	attr.mq_curmsgs = 0;

	atexit(cleanup);
	signal(SIGINT, sigint_handler);

	client_queue = mq_open(client_queue_name, O_CREAT | O_RDONLY, QUEUE_PERMISSIONS, &attr);
	if (client_queue == -1) {
		perror("mq_open (client)");
		exit(1);
	}

	printf("Klient: utworzono kolejkę %s\n", client_queue_name);
	printf("  deskryptor: %d\n", client_queue);
	printf("  maksymalna wielkość wiadomości: %ld\n", attr.mq_msgsize);

	server_queue = mq_open(SERVER_QUEUE_NAME, O_WRONLY);
	if (server_queue == -1) {
		perror("mq_open (server)");
		exit(1);
	}

	char input[64], msg[MAX_MSG_SIZE], result[MAX_MSG_SIZE];

	printf("Wpisz działanie (np. 2+3), zakończ [Ctrl+D]:\n");
	while (fgets(input, sizeof(input), stdin)) {
		input[strcspn(input, "\n")] = '\0'; // usuń \n
		snprintf(msg, sizeof(msg), "%s %s", client_queue_name, input);

		if (mq_send(server_queue, msg, strlen(msg) + 1, 0) == -1) {
			perror("mq_send (server)");
			continue;
		}

		if (mq_receive(client_queue, result, MAX_MSG_SIZE, NULL) == -1) {
			perror("mq_receive (client)");
			continue;
		}

		sleep(rand() % 3);  // symulacja opóźnienia
		printf("Odpowiedź serwera: %s\n", result);
		printf("\nWpisz kolejne działanie lub zakończ [Ctrl+D]:\n");
	}

	return 0;
}
