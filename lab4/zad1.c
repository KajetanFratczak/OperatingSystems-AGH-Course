// Napisz program, który przyjmuje jeden argument: argv[1]. Program ma utworzyć argv[1] procesów potomnych.
// Każdy proces potomny ma wypisać na standardowym wyjściu w jednym wierszu dwa identyfikatory:
// identyfikator procesu macierzystego i swój własny. Na końcu standardowego wyjścia proces macierzysty ma wypisać argv[1].
// Wskazówka: aby program na pewno wypisywał argv[1] jako ostatni wiersz standardowego wyjścia, należy użyć funkcji systemowej wait().

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int is_number(char *str) {
    int dl_napisu = strlen(str);
    for (int i = 0; i < dl_napisu; i++) {
        if (str[i] < '0' || str[i] > '9') {
            return 0;
        }
    }
    return 1;
}

// argc wskazuje liczbę argumentów wiersza poleceń, a argv jest tablicą wskaźników do tych argumentów
int main(int argc, char *argv[]) {

    // Sprawdzenie, czy została podana odpowiednia liczba argumentów
    // oraz czy argument jest liczbą.
    if (argc != 2 || is_number(argv[1]) == 0) {
        printf("Niepoprawna liczba argumentów i/lub argument nie jest liczbą.");
        return 1;
    }

    // Wyjaśnienie funkcji:

    // pid_t getpid(void) - zwraca PID procesu wywołującego funkcję
    // pid_t getppid(void) - zwraca PID procesu macierzystego

    // atoi - konwertuje wartość zapisaną w łańcuchu znaków do postaci liczby typu całkowitego (int)

    // fork - proces potomny otrzymuje wartość 0, a proces macierzysty PID nowego procesu
    // 0 jest bezpieczną wartością, ponieważ jest zarezerwowana dla procesu demona wymiany i nie ma możliwości utworzenia nowego procesu o takim PID
    // Po wywołaniu forka oba procesy (macierzysty i potomny) kontynuują swoje działanie
    // nie można przewidzieć, który z procesów będzie wykonywać swoje instrukcje jako pierwszy

    // Jak za pomocą fork() utworzyć "n" dzieci z tego samego procesu macierzystego?
    // Jak we wskazówce do zadania - muszę użyć funkcji wait() i poczekać na koniec każdego potomnego procesu.

    int liczba_procesow_potomnych = atoi(argv[1]);

    // Tworzę "n" procesów potomnych:
    // w każdej iteracji tworzę nowy proces potomny, wykonuję go i kończę
    // następnie powracamy do macierzystego i robimy tak w kółko
    // na koniec czekam na wszystkie procesy potomne, zanim wypisze argv[1]

    for (int i = 0; i < liczba_procesow_potomnych; i++) {
        pid_t child_pid;
        child_pid = fork();
        if (child_pid == 0) {
            printf("PID procesu macierzystego: %d, PID procesu potomnego: %d\n", (int)getppid(), (int)getpid());
            exit(0); // potomek kończy działanie
            // zapobiega to tworzeniu kolejnych dzieci przez procesy potomne
        }
        else if(child_pid < 0) {
            printf("Błąd podczas tworzenia procesu potomnego.\n");
            exit(1);
        }
    }

    // bez tego mógłby być błąd związany z procesami zombi
    for (int i = 0; i < liczba_procesow_potomnych; i++) {
        wait(NULL);
    }

    printf("Wartość argv[1]: %s\n", argv[1]);

    return 0;

    // Napotkane problemy:
    // 1) tworzyłem procesy potomne i w nich kolejne procesy potomne, zamiast tworzyć je z jednego "rodzica"
    // 2) nie korzystałem z funkcji wait()
}