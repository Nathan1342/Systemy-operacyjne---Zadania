CC = gcc #kompilator
CFLAGS = -Wall -pedantic -std=c99 -D_XOPEN_SOURCE=700 #flagi kompilatora

#Wall - WarningALL - wypisuje wszystkie warningi
#pedantic - dodatkowe ostrzeżenia zwiazane z standardem c
#std=c99 - standard z 1999 roku
#D-XOPEN_SOURCE=700 - używanie funkcji zgodnych z POSIX.1-2008 (najnowszym standardem)
# ze względu na wcześniejszy warning o niejawnej deklaracji funkcji getpgid()

# domyślny cel - co ma zostać zbudowane po wywołaniu make bez innych arg
all: procesy

# cel procesy.c - jak zbudować procesy.c
procesy: procesy.c
	$(CC) $(CFLAGS) -o procesy procesy.c
# kompilacja tworzy plik wykonalny procesy

# usunięcie pliku wykonywalnego procesy, bez wyświetlania komunikatu o błędach
clean:
	rm -f procesy
