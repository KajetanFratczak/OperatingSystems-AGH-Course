// Program odbierający sygnały SIGUSR1.
// Program catcher jest uruchamiany jako pierwszy, wypisuje swój numer PID i czeka na sygnały SIGUSR1.
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

static int flag = 0;
static int mode_changes = 0; //licznik zmian trybu
static int current_mode = 0; //aktualny tryb

void ctrl_c_handler(int signum) {
    printf("Wciśnięto CTRL+C\n");
}

void handler(int signum, siginfo_t *info, void *ptr) {
    int mode = info->si_value.sival_int;
    pid_t sender_pid = info->si_pid;

    // Potwierdzenie odbioru sygnału
    printf("Catcher: odebrano sygnał od PID: %d\n", (int)sender_pid);
    if (mode > 0){
        printf("Tryb pracy: %d\n", mode);
    }
    else{
      printf("\n");
    }

    // Wysłanie potwierdzenia odbioru do nadawcy.
    union sigval value;
    value.sival_int = mode;
    sigqueue(sender_pid, SIGUSR1, value);

    // Zliczanie zmian trybu pracy
    if (mode>0){
        if(mode != current_mode){
            mode_changes++;
            current_mode = mode;
        }

        switch(mode){
            case 1:
                printf("Liczba otrzymanych żądań zmiany trybu pracy: %d\n", mode_changes);
                break;
            case 2:
                printf("Wypisywanie liczb co sekundę: \n");
                for (int i = 1; ; i++) {
                    printf("Liczba: %d\n", i);
                    sleep(1);

                    // działa dopóki nie dostaniemy innego sygnału  SIGUSR1
                    sigset_t set;
                    sigemptyset(&set);
                    if (sigpending(&set) == 0 && sigismember(&set, SIGUSR1)) {
                        break;
                    }
                }
                break;
            case 3:
                printf("Ustawiam ignorowanie Ctrl+C\n");
                signal(SIGINT, SIG_IGN);
                break;
            case 4:
                printf("Ustawiam reakcję 'wypisanie tekstu' na Ctrl+C\n");
                signal(SIGINT, ctrl_c_handler);
                break;
            case 5:
                printf("Otrzymano polecenie zakończenia programu\n");
                flag = 5;
                break;
        }
    }
}

int main(int argc, char *argv[]){

    printf("PID programu catcher: %d\n", (int)getpid());

    struct sigaction action;
    action.sa_sigaction = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &action, NULL);

    // Główna pętla programu - oczekiwanie na sygnały do momentu otrzymania polecenia zakończenia (tryb 5).
    while(flag!=5){
        pause();
    }

    printf("Tryb 5 - zakończenie działania programu catcher.\n");

    return 0;
}