CC = gcc # kompilator
FLAGS = -Wall -pedantic -std=c99 -D_XOPEN_SOURCE=700 #flagi kompilatora
# Wall - WarningALL
# pedantic - ostrzeżenia restrykcyjne danego standardu c
# std=c99 - standard języka c (c99 = 1999)
# D_XOPEN_SOURCE=700 - standard systemu POSIX dla dostępności pewnych komend (jak pgid())

MAIN = execlp # nazwa pliku wykonywalnego
MAIN_SRC = execlp.c # plik źródłowy głównego programu
PP = iden # nazwa pliku  wykonywalnego podprogramu - arguemnt dla głównego programu
PP_SRC = iden.c # plik źródłowy podprogramu

# Znaczek $ oznacza zmienną. Używamy go do odwoływania się do nich.

# Określa cel po użyciu funkcji make lub make all
# Wykonuje domyślną regułę (all)
# Sprawdza czy program się zmienił i w zależności od tego wykonuje kompilacje, bądź nie.
all: $(MAIN) $(PP)

# Kompiluje program execlp.c do pliku wykonywalnego execlp
# Flaga -o pozwala określić nazwę pliku wykonywalnego
$(MAIN): $(MAIN_SRC)
	$(CC) $(FLAGS) $(MAIN_SRC) -o $(MAIN)

# Kompiluje program iden.c do pliku wykonywalnego iden
$(PP): $(PP_SRC)
	$(CC) $(FLAGS) $(PP_SRC) -o $(PP)

# Usuwa plik wykonywalny
# Flaga -f mówi, aby nie zwracać żadnych komunikatów błędów
# Jak na przykład kiedy plik, który chcemy usunąć nie istnieje
clean:
	rm -f $(MAIN) $(PP)

# Komenda make run uruchami
run:
	./$(MAIN) ./$(PP) iden # Uruchomienie programu z argumentem



