// Zadanie 2
// Napisz program, który przyjmuje jeden argument: argv[1] — ścieżkę katalogu.
// Program powinien wypisać na standardowym wyjściu swoją nazwę, korzystając z funkcji printf().
// Zadeklaruj zmienną globalną global, a następnie zmienną lokalną local.
// W zależności od zwróconej wartości przez fork() dokonaj obsługi błędu, wykonaj proces rodzica / proces potomny.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int int_global = 2025;

int main(int argc, char *argv[]) {

    // Sprawdzenie, czy została podana odpowiednia liczba argumentów.
    if (argc != 2) {
        printf("Niepoprawna liczba argumentów.");
        return 1;
    }

    int int_local = 2025;

    printf("Nazwa programu: %s\n", argv[0]);

    pid_t child_pid;
    child_pid = fork();

    if (child_pid < 0) {
        printf("Błąd podczas tworzenia procesu potomnego.\n");
        return 1;
    }

    if(child_pid!=0) {
        int child_status;
        waitpid(child_pid, &child_status, 0);

        printf("parent process\n");
        printf("parent pid = %d, child pid = %d\n", (int)getpid(), child_pid);
        printf("Child exit code: %d\n", WEXITSTATUS(child_status));
        printf("Parent's local = %d, parent's global = %d\n", int_local, int_global);
    }
    else {
        printf("child process\n");
        int_local += 1;
        int_global += 1;
        printf("child pid = %d, parent pid = %d\n", (int)getpid(), (int)getppid());
        printf("child's local = %d, child's global = %d\n", int_local, int_global);
        execl("/bin/ls", "ls", argv[1], NULL);

        printf("Błąd podczas wykonywania execl.\n");
        exit(1);
    }

    return 0;
}