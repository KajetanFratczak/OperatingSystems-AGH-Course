// Napisz prosty chat typu klient-serwer w którym komunikacja pomiędzy uczestnikami czatu / klientami / klientami i
// serwerem realizowana jest za pośrednictwem socketów z użyciem protokołu strumieniowego.
// Adres / port serwera podawany jest jako argument jego uruchomienia
// Klient przyjmuje jako swoje argumenty:
// - swoją nazwę/identyfikator (string o z góry ograniczonej długości)
// - adres serwera (adres IPv4 i numer portu)
// Protokół komunikacyjny powinien obsługiwać następujące operacje:
// LIST:
// Pobranie z serwera i wylistowanie wszystkich aktywnych klientów
// 2ALL string:
// Wysłania wiadomości do wszystkich pozostałych klientów. Klient wysyła ciąg znaków do serwera, a serwer rozsyła ten ciąg wraz z identyfikatorem nadawcy oraz aktualną datą do wszystkich pozostałych klientów
// 2ONE id_klienta string:
// Wysłanie wiadomości do konkretnego klienta. Klient wysyła do serwera ciąg znaków podając jako adresata konkretnego klienta o identyfikatorze z listy aktywnych klientów. Serwer wysyła ten ciąg wraz z identyfikatorem klienta-nadawcy oraz aktualną datą do wskazanego klienta.
// STOP: Zgłoszenie zakończenia pracy klienta.  Powinno skutkować usunięciem klienta z listy klientów przechowywanej na serwerze
// ALIVE - serwer powinien cyklicznie "pingować" zarejestrowanych klientów, aby zweryfikować że wciąż odpowiadają na żądania, a jeśli nie - usuwać ich z listy klientów.
// Klient przy wyłączeniu Ctrl+C powinien wyrejestrować się z serwera
// Dla uproszczenia można przyjąć, że serwer przechowuje informacje o klientach w statycznej tablicy (rozmiar tablicy ogranicza liczbę klientów, którzy mogą jednocześnie byc uczestnikami czatu).

// Kiedy klient zostanie usunięty (obsługa ALIVE):
// Zostanie usunięty z listy klientów, jeśli serwer nie będzie w stanie wysłać do niego komunikatu ALIVE.
// Może to się zdarzyć w następujących sytuacjach:
// Połączenie zostanie przerwane.
// Program kliencki zostanie zamknięty bez wysłania komunikatu STOP (np. awaria aplikacji lub systemu).
// Klient nie odpowiada na komunikat ALIVE (np. z powodu zablokowania gniazda lub problemów z siecią).
// Uznałem, że lepiej zrobić coś takiego niż np. wywalanie klienta po 30 sekundach bezczynności, poniewać może on np. dłuugo myśleć.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>

#define MAX_CLIENTS 10 // max liczba klientów
#define BUFFER_SIZE 1024 // max rozmiar bufora
#define NAME_SIZE 32 // max długośc nazwy klienta
#define PING_INTERVAL 20 // odstęp czasu pomiędzy "pingami" klientów w celu sprawdzenia czy żyją

// struktura klienta
typedef struct {
    int socket_fd; // numer deskryptora gniazda klienta
    char name[NAME_SIZE]; // nazwa klienta
    int active; // czy klient jest aktywny
} Client;

Client clients[MAX_CLIENTS]; // statyczna tablica klientów
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER; // mutex chroniący dostęp do tej tablicy

// Wysyłanie wiadomości do wszystkich klientów poza nadawcą (omija dzięki exclude_sock)
void send_to_all(const char *msg, int exclude_sock) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].socket_fd != exclude_sock) {
            send(clients[i].socket_fd, msg, strlen(msg), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

// Pobieranie listy aktywnych klientów
void get_clients_list(int sock) {
    char buffer[BUFFER_SIZE] = "Active clients:\n";
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) {
            strcat(buffer, clients[i].name);
            strcat(buffer, "\n");
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    send(sock, buffer, strlen(buffer), 0);
}

// Wysyłanie wiadomości do jednego klienta
void send_to_one(char *msg, char *target) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && strcmp(clients[i].name, target) == 0) {
            send(clients[i].socket_fd, msg, strlen(msg), 0);
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

// Usuwanie klienta z listy
void remove_client(int sock) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket_fd == sock) {
            clients[i].active = 0;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

// Obsługa pojedynczego klienta
void *handle_client(void *arg) {
    int sock = *(int *)arg;
    char name[NAME_SIZE];
    char buffer[BUFFER_SIZE];

    // Odczytuję nazwę klienta
    recv(sock, name, NAME_SIZE, 0);

    // Dodaję klienta do listy
    pthread_mutex_lock(&clients_mutex);
    int idx = -1;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i].active) {
            clients[i].socket_fd = sock;
            strncpy(clients[i].name, name, NAME_SIZE);
            clients[i].active = 1;
            idx = i;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    if (idx == -1){
        char* msg = "Server full.\n";
        send(sock, msg, strlen(msg), 0);
        close(sock);
        return NULL;
    }

    printf("Client %s has joined.\n", name);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes = recv(sock, buffer, BUFFER_SIZE, 0);
        if (bytes <= 0) {
            printf("%s disconnected.\n", name);
            break;
        }

        buffer[strcspn(buffer, "\n")] = 0; // usuń znak nowej linii
        printf("Received from %s: %s\n", name, buffer);

        if (strncmp(buffer, "LIST", 4) == 0) {
            get_clients_list(sock);
        } else if (strncmp(buffer, "2ALL ", 5) == 0) {
            time_t now = time(NULL);
            char msg[BUFFER_SIZE];
            snprintf(msg, sizeof(msg), "[%s][%s]: %s\n", name, ctime(&now), buffer + 5);
            send_to_all(msg, sock);
        } else if (strncmp(buffer, "2ONE ", 5) == 0) {
            char *target = strtok(buffer + 5, " ");
            char *msg_body = strtok(NULL, "");
            if (target && msg_body) {
                time_t now = time(NULL);
                char msg[BUFFER_SIZE];
                snprintf(msg, sizeof(msg), "[%s][%s]: %s\n", name, ctime(&now), msg_body);
                send_to_one(msg, target);
            }
        } else if (strncmp(buffer, "STOP", 4) == 0) {
            printf("Client %s disconnected via STOP.\n", name);
            break;
        }
    }

    // Usuń klienta
    close(sock);
    remove_client(sock);
    pthread_exit(NULL);
}

// Wątek do pingowania klientów
void *ping_clients(void *arg) {
    while (1) {
        sleep(PING_INTERVAL);

        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].active) {
                if (send(clients[i].socket_fd, "ALIVE\n", 6, 0) <= 0) {
                    printf("Client %s is inactive.\n", clients[i].name);
                    close(clients[i].socket_fd);
                    clients[i].active = 0;
                }
            }
        }
        pthread_mutex_unlock(&clients_mutex);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("Niepoprawna liczba argumentów.\n");
        return 1;
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);

    // tworzę gniazdo nasłuchujące i konfiguruje adres serwera
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    signal(SIGPIPE, SIG_IGN);

    // ustawiam opcję SO_REUSEADDR, aby umożliwić ponowne użycie adresu/portu po zamknięciu serwera
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // powiązuje gniazdo z adresem i portem
    if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // ustawiam gniazdo w tryb nasłuchiwania z kolejką dla MAX_CLIENTS połączeń
    listen(listen_fd, MAX_CLIENTS);
    printf("Server started on %s:%d\n", ip, port);

    // uruchamiam wątek dla pingowania klientów
    pthread_t ping_thread;
    pthread_create(&ping_thread, NULL, &ping_clients, NULL);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);
        int client_sock = accept(listen_fd, (struct sockaddr *)&client_addr, &len);

        if (client_sock < 0) {
            perror("accept failed");
            continue;
        }

        pthread_t tid;
        pthread_create(&tid, NULL, &handle_client, (void *)&client_sock);
        pthread_detach(tid);
    }

    close(listen_fd);
    return 0;
}
