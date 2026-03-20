// Odczytuje przedział [a, b] z wejścia standardowego,
// przesyła go przez potok nazwany do drugiego programu, a następnie odbiera wynik i go wyświetla.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

int main (){
    double a, b, result;
    const char *domain_fifo = "/tmp/domain";
    const char *result_fifo = "/tmp/result";

    printf("Podaj przedział całkowania: ");
    scanf("%lf %lf", &a, &b);

    int domain_fd = open(domain_fifo, O_WRONLY);
    write(domain_fd, &a, sizeof(double));
    write(domain_fd, &b, sizeof(double));
    close(domain_fd);

    int result_fd = open(result_fifo, O_RDONLY);
    read(result_fd, &result, sizeof(double));
    close(result_fd);

    printf("Wynik: %lf\n", result);

    return 0;
}