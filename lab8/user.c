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
#include <string.h>
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

char random_char() {
    return 'a' + rand() % 26;
}

void generate_text(char *buf) {
    for (int i = 0; i < TEXT_LENGTH; i++) {
        buf[i] = random_char();
    }
    buf[TEXT_LENGTH] = '\0';
}

int main(int argc, char *argv[]) {
    // Pobieram user id z argumentów
    int user_id = atoi(argv[1]);

    srand(time(NULL) + user_id);

    int shm_fd = shm_open("/print_queue", O_RDWR, 0666);
    PrintQueue *queue = mmap(NULL, sizeof(PrintQueue), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // Inicjalizuję tylko raz - za pomocą init
    sem_t *mutex = sem_open("/print_queue_mutex", 0);
    sem_t *full = sem_open("/print_sem", 0);
    sem_t *empty = sem_open("/empty_slots", 0);

    while (1) {
        // Generuje losowy tekst
        char buf[TEXT_LENGTH + 1];
        generate_text(buf);

        printf("Użytkownik %d: wygenerował tekst: %s\n", user_id, buf);

        // Czeka na wolne miejsce w kolejce
        sem_wait(empty);
        // Blokuje dostęp do kolejki
        sem_wait(mutex);

        // Dodaje nowe zadanie do kolejki
        strcpy(queue->queue[queue->tail].text, buf);
        queue->queue[queue->tail].user_id = user_id;
        queue->tail = (queue->tail + 1) % QUEUE_SIZE;
        queue->count++;

        // Odblokowuje kolejkę
        sem_post(mutex);
        // Zwiększa liczbę zadań do wydruku
        sem_post(full);

        sleep(rand() % 5 + 1);
    }

    return 0;
}
