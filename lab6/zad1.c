// Napisz program, który liczy numerycznie wartość całki oznaczonej z funkcji 4/(x2+1) w przedziale od 0 do 1
// metodą prostokątów (z definicji całki oznaczonej Riemanna).
// Pierwszy parametr programu to szerokość każdego prostokąta, określająca dokładność obliczeń.
// Obliczenia należy rozdzielić na k procesów potomnych, tak by każdy z procesów liczył inny fragment ustalonego
// wyżej przedziału. Każdy z procesów powinien wynik swojej części obliczeń przesłać przez potok nienazwany
// do procesu macierzystego. Każdy proces potomny do komunikacji z procesem macierzystym powinien używać osobnego potoku.
// Proces macierzysty powinien oczekiwać na wyniki uzyskane od wszystkich procesów potomnych po czym powinien dodać
// te wyniki cząstkowe i wyświetlić wynik na standardowym wyjściu wraz z czasem wykonania oraz odpowiadającą mu wartością k.

// Program powinien przeprowadzić powyższe obliczenia dla wartości k=1,2,...,n (gdzie n to drugi parametr wywołania programu).
// W ten sposób w wyniku działania programu na standardowym wyjściu wypisane zostaną wyniki obliczeń oraz czasy wykonania
// tych obliczeń przy wykorzystaniu jednego procesu, dwóch procesów, trzech procesów oraz kolejnych liczb wykorzystanych
// procesów aż do n.

// jako argument przyjmuję liczbę prostokątów k i na jej bazie liczę szerokość prostokątów
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

double f(double x){
    return 4.0/(x*x + 1.0);
}

double rect_int(double (*f)(double), double a, double b, double rectangle_width){
    double result = 0.0;
    double curr_x = a;

    while (curr_x < b){
        double next_x = curr_x + rectangle_width;
        if (next_x > b) next_x = b;

        result += f(curr_x) * (next_x - curr_x);
        curr_x = next_x;
    }

    return result;
}

int main(int argc, char *argv[]){
    int num_rect = atoi(argv[1]); // liczba prostokątów
    int num_proc = atoi(argv[2]); // liczba procesów

    if (argc != 3){
        printf("Nieprawidłowa liczba argumentów\n");
        return 1;
     }

    if (num_rect < 1 || num_proc < 1){
        printf("Nieprawidłowa liczba prostokątów i/lub procesów\n");
        return 1;
    }

    double rectangle_width = 1.0/num_rect;
    struct timespec requestStart, requestEnd;
    clock_gettime(CLOCK_REALTIME, &requestStart);

    int* child_pipes = malloc(num_proc * sizeof(int));

    for (int j = 1; j <= num_proc; j++){
        for (int i = 0; i < j; i++){
            int fd[2];
            pipe(fd);
            pid_t pid = fork();
            // dziecko
            if (pid == 0){
                close(fd[0]);
                double a = i/j;
                double b = (i+1)/j;

                double result = rect_int(f, a, b, rectangle_width);

                write(fd[1], &result, sizeof(double));
                close(fd[1]);

                exit(0);
            }
            // rodzic
            else{
                close(fd[1]);
                child_pipes[i] = fd[0];
            }
        }

        // czekamy na dzieci
        while (wait(NULL) > 0);

        // zbieramy wyniki
        double total = 0.0;
        for (int i = 0; i < j; i++){
            double partial_result = 0.0;
            read(child_pipes[i], &partial_result, sizeof(double));
            total += partial_result;
            close(child_pipes[i]);
        }

        clock_gettime(CLOCK_REALTIME, &requestEnd);

        double time = (requestEnd.tv_sec - requestStart.tv_sec) + (requestEnd.tv_nsec - requestStart.tv_nsec)/1e9;

        printf("Wynik: %.15lf\n", total);
        printf("Liczba prostokątów: %d\n", num_rect);
        printf("Szerokość prostokąta: %.2e\n", rectangle_width);
        printf("Liczba procesów: %d\n", j);
        printf("Czas: %.6f\n", time);
        printf("---------------------\n");
    }

    return 0;
}