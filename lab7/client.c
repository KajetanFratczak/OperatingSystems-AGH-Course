#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_MSG_SIZE 256

int main(int argc, char *argv[]){
    char client_queue_name[64];
    char msg[MAX_MSG_SIZE];
    // Tworzenie unikalnej nazwy kolejki klienta na podstawie pid procesu
    sprintf(client_queue_name, "/client%d", getpid());

    // Atrybuty kolejki (max 10 wiadomosci do 256 bajtów)
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    // Tworzenie kolejki klienta do odbierania odpowiedzi od serwera
    mqd_t mq_client_fd = mq_open(client_queue_name, O_RDWR | O_CREAT, 0666, &attr);
    if (mq_client_fd == -1) {
        printf("Nie udało się otworzyć kolejki komunikatów klienta");
        return 1;
    }

    // Otwarcie kolejki do wysyłania wiadomości do serwera
    mqd_t mq_server_fd = mq_open("/msg_queue", O_WRONLY);
    if (mq_server_fd == -1) {
        printf("Nie udało się otworzyć kolejki komunikatów serwera");
        mq_close(mq_client_fd);
        mq_unlink(client_queue_name);
        return 1;
    }

    // Wysłanie wiadomości INIT
    sprintf(msg, "INIT|%s", client_queue_name);
    if (mq_send(mq_server_fd, msg, strlen(msg) + 1, 0) == -1) {
        printf("Nie udało się wysłać komunikatu INIT do serwera");
        mq_close(mq_server_fd);
        mq_close(mq_client_fd);
        mq_unlink(client_queue_name);
        return 1;
    }

    // Odbiór ID klienta
    if (mq_receive(mq_client_fd, msg, MAX_MSG_SIZE, NULL) == -1) {
        printf("Nie udało się odczytać identyfikatora od serwera");
        mq_close(mq_server_fd);
        mq_close(mq_client_fd);
        mq_unlink(client_queue_name);
        return 1;
    }

    int client_id = atoi(msg);
    printf("Otrzymano identyfikator: %d\n", client_id);

    // Tworzenie procesu odbierającego wiadomości
    pid_t pid = fork();
    if (pid == 0) {
        while (1) {
            if (mq_receive(mq_client_fd, msg, MAX_MSG_SIZE, NULL) > 0) {
                printf("Odebrano wiadomość: %s\n", msg);
            }
        }
    } else if (pid > 0) { // Proces wysyłający wiadomości do serwera
        while (1) {
            printf("Wpisz wiadomość do wysłania: ");
            if (fgets(msg, MAX_MSG_SIZE, stdin) != NULL) {
                char send_msg[MAX_MSG_SIZE];
                msg[strcspn(msg, "\n")] = '\0'; // Usuwanie znaku nowej linii
                sprintf(send_msg, "%d|%s", client_id, msg);
                if (mq_send(mq_server_fd, send_msg, strlen(send_msg) + 1, 0) == -1) {
                    printf("Nie udało się wysłać komunikatu do serwera");
                }
            }
        }
    } else {
        printf("Nie udało się utworzyć procesu potomnego");
        mq_close(mq_server_fd);
        mq_close(mq_client_fd);
        mq_unlink(client_queue_name);
        return 1;
    }

    mq_close(mq_client_fd);
    mq_unlink(client_queue_name);
    mq_close(mq_server_fd);
    return 0;
}
