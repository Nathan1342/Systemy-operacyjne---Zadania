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

mqd_t server_queue;

void cleanup() {
	mq_close(server_queue);
	mq_unlink(SERVER_QUEUE_NAME);
	printf("Serwer: kolejka zamknięta i usunięta\n");
}

void sigint_handler(int signum) {
	exit(0);
}

int evaluate_expression(const char *expr, int *result) {
	int a, b;
	char op;
	if (sscanf(expr, "%d%c%d", &a, &op, &b) != 3) {
		return -1;
	}

	switch (op) {
		case '+': *result = a + b; break;
		case '-': *result = a - b; break;
		case '*': *result = a * b; break;
		case '/':
			if (b == 0) return -2;
				*result = a / b; break;
		default: return -1;
	}

	return 0;
}

int main() {
	struct mq_attr attr;
	attr.mq_flags = 0;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = MAX_MSG_SIZE;
	attr.mq_curmsgs = 0;

	atexit(cleanup);
	signal(SIGINT, sigint_handler);

	server_queue = mq_open(SERVER_QUEUE_NAME, O_CREAT | O_RDONLY, QUEUE_PERMISSIONS, &attr);
	if (server_queue == -1) {
		perror("mq_open (server)");
		exit(1);
	}

	printf("Serwer: utworzono kolejkę %s\n", SERVER_QUEUE_NAME);
	printf("  deskryptor: %d\n", server_queue);
	printf("  maksymalna wielkość wiadomości: %ld\n", attr.mq_msgsize);
	printf("  maksymalna liczba wiadomości: %ld\n", attr.mq_maxmsg);

	char buffer[MAX_MSG_SIZE];

	while (1) {
		ssize_t bytes_read = mq_receive(server_queue, buffer, MAX_MSG_SIZE, NULL);
		if (bytes_read < 0) {
			perror("mq_receive");
			continue;
		}

		char client_queue_name[64];
		char expression[MAX_EXPRESSION_LENGTH];

		sscanf(buffer, "%s %s", client_queue_name, expression);
		printf("Serwer: otrzymano od [%s] wyrażenie: %s\n", client_queue_name, expression);

		int result;
		char result_msg[64];

		int eval_status = evaluate_expression(expression, &result);
		if (eval_status == 0) {
			snprintf(result_msg, sizeof(result_msg), "Wynik: %d", result);
		} else if (eval_status == -2) {
			snprintf(result_msg, sizeof(result_msg), "Błąd: dzielenie przez zero");
		} else {
			snprintf(result_msg, sizeof(result_msg), "Błąd: nieprawidłowe wyrażenie");
		}

		mqd_t client_queue = mq_open(client_queue_name, O_WRONLY);
		if (client_queue == -1) {
			perror("mq_open (client)");
			continue;
		}

		sleep(rand() % 3);  // symulacja opóźnienia
		if (mq_send(client_queue, result_msg, strlen(result_msg) + 1, 0) == -1) {
			perror("mq_send (result)");
		}

		mq_close(client_queue);
		printf("Serwer: wysłano do [%s] wynik: %s\n", client_queue_name, result_msg);
	}

	return 0;
}
