// Wykorzystując semafory i pamięć wspólną z IPC Systemu V lub standardu POSIX napisz program symulujący
// działanie systemu wydruku:
// System składa się z N użytkowników oraz M drukarek. Każdy z użytkowników może wysłać do systemu
// zadanie wydruku tekstu składającego się z 10 znaków. Drukarka, która nie jest aktualnie zajęta,
// podejmuje się zadania "wydruku" tekstu danego zadania. Wydruk w zadaniu polega na wypisaniu na
// standardowym wyjściu znaków wysłanych wcześniej do wydruku w ten sposób, że każdy następny znak
// wpisywany jest co jedną sekundę. Jeżeli wszystkie drukarki są zajęte to zlecenia wydruku są kolejkowane
// w opisywanym systemie wydruku. Jeżeli kolejka jest pełna to użytkownik chcący zlecić zadanie wydruku
// czeka do momentu gdy będzie można zlecenie wpisać do kolejki.
// Każdy z N użytkowników powinien przesyłać do systemu wydruku zadanie wydruku 10 losowych znaków
// (od 'a' do 'z') a następnie odczekać losową liczbe sekund. Zadania zlecenia wydruku i odczekania
// powinny być wykonane w nieskończonej pętli.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>


#define QUEUE_SIZE 10
#define TEXT_LENGTH 10

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
    int shm_fd = shm_open("/print_queue", O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(PrintQueue));
    PrintQueue *queue = mmap(NULL, sizeof(PrintQueue), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // Inicjalizuję tylko raz - za pomocą init
    sem_t *mutex = sem_open("/print_queue_mutex", 0);
    sem_t *full = sem_open("/print_sem", 0);
    sem_t *empty = sem_open("/empty_slots", 0);

    printf("Drukarka %d uruchomiona\n", getpid());

    while (1) {
        // Czeka aż pojawi się zadanie w kolejce
        sem_wait(full);
        // Blokuje dostęp do kolejki
        sem_wait(mutex);

        // Pobiera zadanie z kolejki
        PrintTask task = queue->queue[queue->head];
        queue->head = (queue->head + 1) % QUEUE_SIZE;
        queue->count--;

        // Odblokowuje kolejke
        sem_post(mutex);
        // Nowe wolne miejsce
        sem_post(empty);

        printf("Drukarka %d: drukuje tekst użytkownika %d: ", getpid(), task.user_id);
        fflush(stdout);
        for (int i = 0; i < TEXT_LENGTH; i++) {
            printf("%c", task.text[i]);
            fflush(stdout);
            sleep(1);
        }
        printf("\n");
    }

    return 0;
}
