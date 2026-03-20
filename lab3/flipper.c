// Napisz program o nazwie "flipper", który na wejściu przyjmie ścieżkę do katalogu źródłowego oraz katalogu wynikowego.
// Program będzie przeszukiwał katalog źródłowy w poszukiwaniu plików tekstowych,
// otwierał je i odwracał kolejność znaków w każdej linii.
// Następnie zmodyfikowane pliki zostaną zapisane w katalogu wynikowym.

#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

void flip_file(char *in_path, char *out_path) {
    char blok[1024];

    FILE *in_file = fopen(in_path, "r");
    if (in_file == NULL) {
        printf("Nie udało się otworzyć pliku.");
        return;
    }
    FILE *out_file = fopen(out_path, "w");
    if (out_file == NULL) {
        printf("Nie udało się otworzyć pliku.");
        fclose(in_file);
        return;
    }

    // fgets - wczytuje jeden wiersz tekstu do bufora ze wskazanego strumienia
    while (fgets(blok, sizeof(blok), in_file) != NULL) {
        int dl_bloku = strlen(blok);

        // mała poprawka - fgets zapisuje znak nowej linii na końcu, co powodowało błędy przy odwracaniu
        if (blok[dl_bloku - 1] == '\n') {
            blok[dl_bloku - 1] = '\0';
            dl_bloku--;
        }

        for (int i = 0; i < dl_bloku / 2; i++) {
            char temp = blok[i];
            blok[i] = blok[dl_bloku - i - 1];
            blok[dl_bloku - i - 1] = temp;
        }
        // fprintf - zapisuje tekst sformatowany do wskazanego strumienia
        fprintf(out_file, "%s\n", blok);
    }

    fclose(in_file);
    fclose(out_file);
}

int main(int argc, char *argv[]) {
    // Sprawdzam, czy użytkownik wpisał dwa argumenty:
    // ścieżkę do katalogu źródłowego oraz ścieżkę do katalogu wyjściowego
    if (argc != 3) {
        printf("Niepoprawna liczba argumentów.");
        return 1;
    }

    // argv[0] - nazwa wykonywanego programu (w moim przypadku ./flipper)
    // argv[1] - ścieżka do katalogu źródłowego
    // argv[2] - ścieżka do katalogu wyjściowego

    // Otwieram katalog wejściowy + obsługa errora gdyby nie istniał (na przyszłość -> error handling można robić np. za pomocą perror)
    DIR *in_path = opendir(argv[1]);
    if (in_path == NULL) {
        printf("Nie udało się otworzyć katalogu.");
        return 1;
    }

    // Otwieram katalog wyjściowy, aby sprawdzić czy istnieje (a jeśli go nie ma, to go tworzę)
    DIR *out_path = opendir(argv[2]);
    if (out_path == NULL) {
        mkdir(argv[2], 0777);
    }

    struct dirent *entry; // wskaźnik na pojedynczy wpis w katalogu

    // Przechodzę po wszystkich plikach w katalogu źródłowym i sprawdzam czy to plik ".txt"
    // Muszę sprawdzać ostatnie 4 znaki, bo może być przypadek plik.txt.jpg
    while ((entry = readdir(in_path)) != NULL) {
        // strcmp zwraca 0 jeśli stringi są takie same
        if (strcmp(entry->d_name + strlen(entry->d_name) - 4, ".txt") == 0) {
            char in_file_path[1024];
            char out_file_path[1024];
            // Tworzę pełne ścieżki do plików, coś w stylu: "./lab3/result/nazwa_pliku.txt" i za pomocą snprintf zapisuje je w bufforach
            snprintf(in_file_path, 1024, "%s/%s", argv[1], entry->d_name);
            snprintf(out_file_path, 1024, "%s/%s", argv[2], entry->d_name);
            //printf("%s\n", in_file_path);
            //printf("%s\n", out_file_path);
            flip_file(in_file_path, out_file_path);
        }
        //printf("%s\n", entry->d_name);
    }

    closedir(in_path);
    closedir(out_path);
    return 0;
}
