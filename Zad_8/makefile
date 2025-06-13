CC = gcc
CFLAGS = -Wall -std=c99 -pedantic
LDFLAGS = -lrt

all: serwer klient

serwer: serwer.c
	$(CC) $(CFLAGS) serwer.c -o serwer.x $(LDFLAGS)

klient: klient.c
	$(CC) $(CFLAGS) klient.c -o klient.x $(LDFLAGS)

run_serwer:
	xterm -hold -title SERWER -bg red -fg black -e ./serwer.x &

run_clients:
	xterm -hold -title KLIENT1 -bg green -fg black -e ./klient.x &
	xterm -hold -title KLIENT2 -bg green -fg black -e ./klient.x &

clean:
	rm -f serwer.x klient.x
