CC = gcc
FLAGS = -Wall -pedantic -std=c99
MAIN_1 = a
SRC_1 = podpunkt_a.c
MAIN_2 = b
SRC_2 = podpunkt_b.c
MAIN_3 = c_1
SRC_3 = podpunkt_c_1.c
MAIN_3_2= c_2
SRC_3_2= podpunkt_c_2.c

all: $(MAIN_1) $(MAIN_2) $(MAIN_3) $(MAIN_3_2)

$(MAIN_1): $(SRC_1)
	$(CC) $(FLAGS) $(SRC_1) -o $(MAIN_1)

$(MAIN_2): $(SRC_2)
	$(CC) $(FLAGS) $(SRC_2) -o $(MAIN_2)

$(MAIN_3): $(SRC_3)
	$(CC) $(FLAGS) $(SRC_3) -o $(MAIN_3)

$(MAIN_3_2): $(SRC_3_2)
	$(CC) $(FLAGS) $(SRC_3_2) -o $(MAIN_3_2)

clean:
	rm -f $(MAIN_1) $(MAIN_2) $(MAIN_3) $(MAIN_3_2)
