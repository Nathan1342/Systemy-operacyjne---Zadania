CC = gcc
FLAGS = -Wall -pedantic -std=c99

MAIN_A = w
SRC_A = wykluczenie.c

MAIN_B = pow
SRC_B = powielacz.c

SUB_B = pot
SRC_SBB = potomne.c

all: $(MAIN_A) $(MAIN_B) $(SUB_B)

$(MAIN_A): $(SRC_A)
	$(CC) $(FLAGS) $(SRC_A) -o $(MAIN_A)

$(MAIN_B): $(SRC_B)
	$(CC) $(FLAGS) $(SRC_B) -o $(MAIN_B)

$(SUB_B): $(SRC_SBB)
	$(CC) $(FLAGS) $(SRC_SBB) -o $(SUB_B)

run_a:
	$(CC) $(FLAGS) $(SRC_A) -o $(MAIN_A)
	./$(MAIN_A)

run_b:
	$(CC) $(FLAGS) $(SRC_B) -o $(MAIN_B)
	$(CC) $(FLAGS) $(SRC_SBB) -o $(SUB_B)
	./$(MAIN_B) 3 2 ./$(SUB_B) numer.txt

clean:
	rm -f $(MAIN_A) $(MAIN_B) $(SUB_B)
