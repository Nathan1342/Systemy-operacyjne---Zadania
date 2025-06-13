#include <stdio.h> // fprintf
#include <stdlib.h> // exit
#include <sys/types.h> // open
#include <sys/stat.h> // open
#include <fcntl.h> // open
#include <unistd.h> // read, write

#define WIELKOSC_KONSUMENT 3

// argv[1] - plik_wyj, argv[2] - tryb obsługi, argv[3] - potok
int main(int argc, char *argv[]) {
        // Program dla funkcji konsumenta - odbiera z potoku, przepisuje do pliku

        int pipe_des, file_des;

        if (argc != 4) {
                fprintf(stderr, "Niepoprawna ilosc argumentow\n");
                exit(EXIT_FAILURE);
        }

        // Otworzenie pliku
        if ((file_des = open(argv[1], O_WRONLY | O_TRUNC | O_CREAT, 0644)) == -1) {
                perror("Blad funkcji open/konsument/plik");
                exit(EXIT_FAILURE);
        }

        // Otworzenie potoku
        if ((pipe_des = open((atoi(argv[2]) == 1) ? "potok" : argv[3], O_RDONLY, 0644)) == -1) {
                perror("Blad funkcji open/konsument/potok");
                exit(EXIT_FAILURE);
        }

        char bufor[WIELKOSC_KONSUMENT];
        ssize_t bajty;

        while((bajty = read(pipe_des, bufor, WIELKOSC_KONSUMENT)) > 0) {
                printf("Konsument pobiera z bufora: %s\n", bufor);
                if(write(file_des, bufor, bajty) != bajty) {
                        perror("Blad zapisu do pliku");
                        exit(EXIT_FAILURE);
                }
                sleep(rand() % 2 + 1); // 1 lub 2 sekundy
        }
        if(bajty == -1) {
                perror("Blad funkcji read/konsument");
                exit(EXIT_FAILURE);
        }

        close(pipe_des);
        close(file_des);
        exit(EXIT_SUCCESS);

}
