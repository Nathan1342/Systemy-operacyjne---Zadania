CC = gcc
FLAGS = -Wall -pedantic -std=c99

MAIN_A = 5_a
SRC_A = main_5_a.c

SUB_A_1 = konsm
SRC_A_1 = konsument.c

SUB_A_2 = prodc
SRC_A_2 = producent.c

MAIN_B = 5_b

all: $(MAIN_A) $(SUB_A_1) $(SUB_A_2)

$(MAIN_A): $(SRC_A)
	$(CC) $(FLAGS) $(SRC_A) -o $(MAIN_A)

$(SUB_A_1): $(SRC_A_1)
	$(CC) $(FLAGS) $(SRC_A_1) -o $(SUB_A_1)

$(SUB_A_2): $(SRC_A_2)
	$(CC) $(FLAGS) $(SRC_A_2) -o $(SUB_A_2)

run_a:
	@echo "Uruchamiam program 5_a..."
	$(CC) $(FLAGS) $(SRC_A) -o $(MAIN_A)
	$(CC) $(FLAGS) $(SRC_A_1) -o $(SUB_A_1)
	$(CC) $(FLAGS) $(SRC_A_2) -o $(SUB_A_2)
	./$(MAIN_A) ./$(SUB_A_1) ./$(SUB_A_2) plik_wej.txt plik_wyj.txt

# xterm - uruchamia program w nowym terminalu, hold - okno nie zamyka sie po zakonczeniu
# title <nazwa> - ustawia tytuł terminala na <nazwa>, -e <program> <arg[1]> -
# pozwala na uruchomienie polecenia lub programu w nowym oknie terminala
run_b:
	@echo "Uruchamian program 5_b..."
	mkfifo -m 0644 potok
	$(CC) $(FLAGS) $(SRC_A_1) -o $(SUB_A_1)
	$(CC) $(FLAGS) $(SRC_A_2) -o $(SUB_A_2)
	xterm -hold -title KONSUMENT -e "./konsm plik_wyj.txt 2 potok" &
	xterm -hold -title PRODUCENT -e "./prodc plik_wej.txt 2 potok" &

clean_pipe:
	unlink potok

clean:
	rm -f $(MAIN_A) $(SUB_A_1) $(SUB_A_2)

check:
	diff -s plik_wej.txt plik_wyj.txt
