// Program wysyłający sygnały SIGUSR1
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

static int got_response = 0;

// Potwierdzenie odbioru
void confirm_handler(int signum, siginfo_t *info, void *ptr) {
    if (signum == SIGUSR1) {
        int mode = info->si_value.sival_int;
        printf("Sender: otrzymano potwierdzenie, tryb: %d\n", mode);
        got_response = 1;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Nieprawidłowa liczba argumentów\n");
        return 1;
    }

    pid_t catcher_pid = atoi(argv[1]);
    int mode = atoi(argv[2]);

    if (mode < 1 || mode > 5) {
        printf("Nieprawidłowy tryb pracy (1-5)\n");
        return 1;
    }

    // Konfiguracja i wysłanie sygnału
    struct sigaction action;
    action.sa_sigaction = confirm_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &action, NULL);

    union sigval value;
    value.sival_int = mode;

    printf("Sender: wysyłam sygnał do catchera (PID: %d) z trybem: %d\n", (int)catcher_pid, mode);

    sigqueue(catcher_pid, SIGUSR1, value);

    while (!got_response) {
        pause();
    }

    printf("Sender: zakończono działanie\n");

    return 0;
}
