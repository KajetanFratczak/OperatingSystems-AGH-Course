// Napisz program demonstrujący różne reakcje na przykładowy sygnał SIGUSR1 w zależności od ustawionych dyspozycji.
// Reakcja na sygnał SIGUSR1 programu powinna zależeć od wartości argumentu z linii poleceń.
// Argument ten może przyjmować wartości: none, ignore, handler, mask.
// Program w zależności od parametru odpowiednio: nie zmienia reakcji na sygnał, ustawia ignorowanie,
// instaluje handler obsługujący sygnał (działający w ten sposób, że wypisuje komunikat o jego otrzymaniu),
// maskuje ten sygnał oraz sprawdza czy wiszący/oczekujący sygnał jest widoczny.
// Następnie przy pomocy funkcji raise wysyła sygnał do samego siebie oraz wykonuje odpowiednie dla danej opcji,
// opisane wyżej działania.

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

void handler(int signum){
    printf("Otrzymano sygnał: %d\n", signum);
}

int main(int argc, char *argv[]){
    // Na wejściu otrzymujemy sygnał none, ignore, handler lub mask

    char *a = argv[1];

    // Sprawdzenie czy został podany jeden argument i czy jest to none, ignore, handler lub mask
    if (argc != 2 || (strcmp(a, "none") != 0 && strcmp(a, "ignore") != 0 && strcmp(a, "handler") != 0 && strcmp(a, "mask") != 0)) {
        printf("Niepoprawna liczba argumentów i/lub argument nie jest poprawny.");
        return 1;
    }

    // domyślna akcja dla sygnału SIGUSR1
    if (strcmp(a, "none") == 0){
        signal(SIGUSR1, SIG_DFL);
    }

    // dla opcji "ignore" ustawienie ignorowania sygnału SIGUSR1
    else if (strcmp(a, "ignore") == 0){
        signal(SIGUSR1, SIG_IGN);
    }

    // "handler" - własna funkcja obsługująca sygnał
    else if (strcmp(a, "handler") == 0){
        signal(SIGUSR1, handler);
    }

    // "mask" - tworzenie maski sygnałów dla procesu
    else if (strcmp(a, "mask") == 0){
        sigset_t newmask, oldmask;
        sigemptyset(&newmask);
        sigaddset(&newmask, SIGUSR1);
        if (sigprocmask(SIG_SETMASK, &newmask, &oldmask) < 0)
            perror("Nie udało się zablokować sygnału\n");
    }

    // Wysyła sygnał do samego siebie
    raise(SIGUSR1);

    // Sprawdzenie czy sygnał SIGUSR1 jest oczekujący
    if (strcmp(a, "mask") == 0){
        sigset_t pending;
        sigpending(&pending);
        if (sigismember(&pending, SIGUSR1)){
            printf("Sygnał SIGUSR1 jest oczekujący. \n");
        }
        else{
            printf("Sygnał SIGUSR1 nie jest oczekujący. \n");
        }
    }

    return 0;
}