// Napisz program, który liczy numerycznie wartość całki oznaczonej z funkcji 4/(x2+1) w
// przedziale od 0 do 1 metodą prostokątów (z definicji całki oznaczonej Riemanna).
// Program będzie podobny do zadania 1 w zestawie 6, jednak do przyspieszenia obliczeń zamiast
// procesów użyjemy wątki. Pierwszy parametr programu to szerokość każdego prostokąta, określająca
// dokładność obliczeń. Obliczenia należy rozdzielić na k wątków, tak by każdy z nich liczył inny
// fragment ustalonego wyżej przedziału. Każdy z wątków powinien wynik swojej części obliczeń umieścić
// w odpowiednim miejscu przeznaczonej do tego tablicy wyników. Wątek główny powinien oczekiwać na
// wyniki uzyskane od wszystkich wątków. Ponieważ dopiero na następnym laboratorium zapoznamy się z
// metodami synchronizacji wątków, można do tego celu z pominięciem tych mechanizmów użyć tablicy,
// w której każdy z wątków liczących po umieszczeniu wyników swoich obliczeń w tablicy wyników
// umieszcza np wartość 1. Nazwijmy tę tablicę tablicą gotowości. Wątek główny sprawdza zatem czy
// wszystkie wartości w tablicy gotowości wynoszą 1 i jeśli tak to dodaje wyniki z tablicy wyników
// i wyświetla wynik na standardowym wyjściu. Zamiast powyższego można również przed dodaniem wyników
// zwróconych przez wątki czekać na zakończenie wszystkich wątków.
#define _POSIX_C_SOURCE 199309L // aktywuje widoczność clock_gettime()
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>

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

// Struktura do przekazania informacji do każdego wątku
typedef struct {
    int id;
    int num_threads;
    double rectangle_width;
    double *results;
    //int *ready; - tablica gotowości
} ThreadData;

void* thread_func(void *arg){
    ThreadData *data = (ThreadData*)arg;

    // Wątek liczy swój przedział do całkowania
    double a = data->id/data->num_threads;
    double b = (data->id+1)/data->num_threads;

    // Wykonuje całkowanie na swoim przedziale
    double result = rect_int(f, a, b, data->rectangle_width);
    data->results[data->id] = result;

    //data->ready[data->id] = 1;

    return NULL;
}


int main(int argc, char *argv[]) {
    int num_rect = atoi(argv[1]); // liczba prostokątów
    int num_threads = atoi(argv[2]); // liczba wątków

    if (argc != 3){
        printf("Nieprawidłowa liczba argumentów\n");
        return 1;
    }

    if (num_rect < 1 || num_threads < 1){
        printf("Nieprawidłowa liczba prostokątów i/lub wątków\n");
        return 1;
    }

    double rectangle_width = 1.0/num_rect;
    struct timespec requestStart, requestEnd;
    clock_gettime(CLOCK_REALTIME, &requestStart);

    // bez malloc-ów (od wersji C99 można już bez)
    double results[num_threads];
    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];
//    double* results = malloc(num_threads * sizeof(double));
//    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
//    ThreadData *thread_data = malloc(num_threads * sizeof(ThreadData));
//    int* ready = malloc(num_threads * sizeof(int));

    for (int j = 1; j <= num_threads; j++){
//        for (int i = 0; i < j; i++){
//            ready[i] = 0;
//        }

        // Tworzenie wątków
        for (int i = 0; i < j; i++){
            thread_data[i].id = i;
            thread_data[i].num_threads = j;
            thread_data[i].rectangle_width = rectangle_width;
            thread_data[i].results = results;
            //thread_data[i].ready = ready;

            pthread_create(&threads[i], NULL, thread_func, (void*)&thread_data[i]);
        }

        // Czekamy na zakończenie wszystkich wątków
        for (int i = 0; i < j; i++){
            pthread_join(threads[i], NULL);
        }

        // Czekamy przy użyciu "tablicy gotowości" - zamiast powyższej pętli można takie coś zrobić
//        int all_ready = 0;
//        while (!all_ready){
//            all_ready = 1;
//            for (int i = 0; i < j; i++){
//                if (ready[i] == 0){
//                    all_ready = 0;
//                    break;
//                }
//            }
//        }

        // Sumowanie wyników
        double total = 0.0;
        for (int i = 0; i < j; i++){
            total += results[i];
        }

        clock_gettime(CLOCK_REALTIME, &requestEnd);
        double time = (requestEnd.tv_sec - requestStart.tv_sec) + (requestEnd.tv_nsec - requestStart.tv_nsec)/1e9;

        printf("Wynik: %.15lf\n", total);
        printf("Liczba prostokątów: %d\n", num_rect);
        printf("Szerokość prostokąta: %.2e\n", rectangle_width);
        printf("Liczba wątków: %d\n", j);
        printf("Czas: %.6f\n", time);
        printf("---------------------\n");
    }

    // Dokładność zależy tylko od rectangle_width, czyli od s
    // zerokości prostokątów na jakie dzielimy przedział całkowania.
    // Liczba wątków powoduje tylko to, że jak np. mamy 4 wątki to dzielą
    // sobie przedział na 4 podprzedziały i tam całkują zgodnie z ustaloną szerokością prostokątów.

    return 0;
}