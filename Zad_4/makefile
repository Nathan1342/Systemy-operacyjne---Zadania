CC = gcc
FLAGS = -Wall -pedantic -std=c99

MAIN = 4
SRC = main_4.c

all: $(MAIN)

$(MAIN): $(SRC)
	$(CC) $(FLAGS) $(SRC) -o $(MAIN)

clean:
	rm -f $(MAIN)

check:
	diff -s plik_wej.txt plik_wyj.txt

run:
	./$(MAIN) plik_wej.txt plik_wyj.txt
