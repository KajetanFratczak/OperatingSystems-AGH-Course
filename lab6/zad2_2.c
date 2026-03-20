// Odbiera przedział [a, b] z potoku nazwanego,
// oblicza wartość całki metodą prostokątów z zadaną szerokością prostokąta (np. dx = 1e9) i odsyła wynik.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

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

int main (){
    double a, b, result;
    const char *domain_fifo = "/tmp/domain";
    const char *result_fifo = "/tmp/result";

    mkfifo(domain_fifo, 0666);
    mkfifo(result_fifo, 0666);

    // tu można dać while(1) jak chcemy sami decydować kiedy się zamknie
    int domain_fd = open(domain_fifo, O_RDONLY);
    read(domain_fd, &a, sizeof(double));
    read(domain_fd, &b, sizeof(double));
    close(domain_fd);

    result = rect_int(f, a, b, 1e-6);

    int result_fd = open(result_fifo, O_WRONLY);
    write(result_fd, &result, sizeof(double));
    close(result_fd);

    return 0;
}