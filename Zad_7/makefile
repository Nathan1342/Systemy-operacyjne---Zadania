CC = gcc
CFLAGS = -Wall -pedantic -std=c99 -Ilib

# Nazwa biblioteki i katalog\ nLIB_DIR = lib
LIB_NAME = lib

PROGRAMS = main producent konsument

# --------------------------------------------------
# 1) Kompilacja biblioteki statycznej
# --------------------------------------------------
lib/lib.o: lib/lib.c lib/lib.h
	$(CC) $(CFLAGS) -c lib/lib.c -o lib/lib.o

lib/lib.a: lib/lib.o
	ar rcs lib/lib.a lib/lib.o

static: lib/lib.a
	$(CC) $(CFLAGS) -o main main.c lib/lib.a
	$(CC) $(CFLAGS) -o producent producent.c lib/lib.a
	$(CC) $(CFLAGS) -o konsument konsument.c lib/lib.a

run_static: static
	./main

# --------------------------------------------------
# 2) Kompilacja biblioteki dzielonej
# --------------------------------------------------
# -fPIC Position-independent Code
lib/lib.so: lib/lib.c lib/lib.h
	$(CC) $(CFLAGS) -fPIC -shared lib/lib.c -o lib/lib.so

# --------------------------------------------------
# 2.1) Linkowanie z RPATH (dynamiczna z wbudowaną ścieżką)
# --------------------------------------------------
# -Wl - przekazanie opcji bezpośrednio do linkera
dynamic_1: lib/lib.so
	$(CC) $(CFLAGS) -o main main.c -Llib -l:lib.so -Wl,-rpath=$(CURDIR)/lib
	$(CC) $(CFLAGS) -o producent producent.c -Llib -l:lib.so -Wl,-rpath=$(CURDIR)/lib
	$(CC) $(CFLAGS) -o konsument konsument.c -Llib -l:lib.so -Wl,-rpath=$(CURDIR)/lib

run_dynamic_1: dynamic_1
	./main

# --------------------------------------------------
# 2.2) Linkowanie bez RPATH + uruchamianie z LD_LIBRARY_PATH
# --------------------------------------------------
dynamic_2: lib/lib.so
	$(CC) $(CFLAGS) -o main main.c -Llib -l:lib.so
	$(CC) $(CFLAGS) -o producent producent.c -Llib -l:lib.so
	$(CC) $(CFLAGS) -o konsument konsument.c -Llib -l:lib.so

run_dynamic_2: dynamic_2
	LD_LIBRARY_PATH=$(CURDIR)/lib:$$LD_LIBRARY_PATH ./main

# --------------------------------------------------
# Sprzątanie
# --------------------------------------------------
clean:
	rm -f $(PROGRAMS) lib/lib.o lib/lib.a lib/lib.so
