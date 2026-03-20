#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_MSG_SIZE 256
#define MAX_CLIENTS 10

// id i nazwy kolejek klientów
int client_ids[MAX_CLIENTS];
char client_queue_names[MAX_CLIENTS][50];

// obsługa ctrl + C
void handle_sigint(int sig) {
    mq_unlink("/msg_queue");
    printf("\nSerwer został zamknięty.\n");
    exit(0);
}

int main(int argc, char *argv[]) {

    mqd_t server_queue_fd;
    int client_count = 0;
    char msg[MAX_MSG_SIZE];

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    signal(SIGINT, handle_sigint);

    // Tworzenie kolejki serwera
    server_queue_fd = mq_open("/msg_queue", O_RDWR | O_CREAT, 0666, &attr);
    if (server_queue_fd == -1) {
        printf("Nie udało się otworzyć kolejki komunikatów serwera");
        return 1;
    }

    printf("Serwer uruchomiony. Oczekiwanie na wiadomości...\n");

    // Odbieranie wiadomości
    while (1) {
        if (mq_receive(server_queue_fd, msg, MAX_MSG_SIZE, NULL) > 0) {
            // strtok zastępuje znak "|" znakiem terminalnym
            char *command = strtok(msg, "|");
			// Obsługa komunikatu INIT
            if (strcmp(command, "INIT") == 0) {
                char *client_queue_name = strtok(NULL, "|");

                if (client_count >= MAX_CLIENTS) {
                    printf("Za dużo klientów. Odrzucono: %s\n", client_queue_name);
                    continue;
                }

                client_ids[client_count] = client_count;
                strncpy(client_queue_names[client_count], client_queue_name, 50);

                // Otwarcie kolejki klienta
                mqd_t client_fd = mq_open(client_queue_name, O_WRONLY);
                if (client_fd == -1) {
                    printf("Nie udało się otworzyć kolejki klienta");
                    continue;
                }

                // Wysłanie ID do klienta
                snprintf(msg, MAX_MSG_SIZE, "%d", client_count);
                if (mq_send(client_fd, msg, strlen(msg) + 1, 0) == -1) {
                    printf("Nie udało się wysłać identyfikatora do klienta");
                    mq_close(client_fd);
                    continue;
                }

                printf("Dodano klienta o ID: %d, kolejka: %s\n", client_count, client_queue_name);
                mq_close(client_fd);
                client_count++;
            } else {
                // Rozesłanie wiadomości do wszystkich klientów oprócz nadawcy
                int sender_id = atoi(command);
                char *message = strtok(NULL, "|");

                printf("Otrzymano wiadomość od klienta %d: %s\n", sender_id, message);

                for (int i = 0; i < client_count; i++) {
                    if (client_ids[i] != sender_id) {
                        mqd_t client_fd = mq_open(client_queue_names[i], O_WRONLY);
                        if (client_fd == -1) {
                            printf("Nie udało się otworzyć kolejki klienta do przekazania wiadomości");
                            continue;
                        }

                        if (mq_send(client_fd, message, strlen(message) + 1, 0) == -1) {
                            printf("Nie udało się wysłać wiadomości do klienta");
                        }

                        mq_close(client_fd);
                    }
                }
            }
        }
    }

    mq_close(server_queue_fd);
    mq_unlink("/msg_queue");
    return 0;
}
