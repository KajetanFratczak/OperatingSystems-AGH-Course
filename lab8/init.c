// init.c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <semaphore.h>
#include <unistd.h>

#define QUEUE_SIZE 10
#define TEXT_LENGTH 10

// PrintTask to jedno zadanie wydruku (10-literowy tekst + ID użytkownika).
// PrintQueue to cykliczna kolejka FIFO na 10 zadań.
// Pola head, tail i count służą do śledzenia, gdzie są elementy w kolejce.

typedef struct {
    char text[TEXT_LENGTH + 1];
    int user_id;
} PrintTask;

typedef struct {
    PrintTask queue[QUEUE_SIZE];
    int head;
    int tail;
    int count;
} PrintQueue;

int main() {
    // Tworzenie pamięci współdzielonej
    int shm_fd = shm_open("/print_queue", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(PrintQueue));
    PrintQueue *queue = mmap(NULL, sizeof(PrintQueue), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // Inicjalizacja kolejki
    queue->head = 0;
    queue->tail = 0;
    queue->count = 0;

    // Inicjalizacja semaforów
    sem_open("/print_queue_mutex", O_CREAT | O_EXCL, 0666, 1); // kontrola dostępu do kolejki
    sem_open("/print_sem", O_CREAT | O_EXCL, 0666, 0); // ilość zadań oczekujących na wydruk
    sem_open("/empty_slots", O_CREAT | O_EXCL, 0666, QUEUE_SIZE); // ilość wolnych miejsc w kolejce

    printf("System wydruku zainicjalizowany.\n");
    return 0;
}
