#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
	printf("Proces potomny: UID: %d, GID: %d, PID: %d, PPID: %d, PGID: %d\n",
                getuid(), getgid(), getpid(), getppid(), getpgid(0));
}
