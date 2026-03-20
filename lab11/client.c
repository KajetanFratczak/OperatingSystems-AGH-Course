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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define NAME_SIZE 32

int sockfd; // numer deskryptora gniazda
char name[NAME_SIZE]; // nazwa klienta

// obsługa ctrl + C
// wysyła komendę STOP do serwera, zamyka gniazdo i kończy program
void handle_exit(int sig) {
    char msg[] = "STOP\n";
    send(sockfd, msg, strlen(msg), 0);
    close(sockfd);
    printf("\nDisconnected from server.\n");
    exit(0);
}

// wątek odbioru wiadomości
void *receive_handler(void *arg) {
    char buffer[BUFFER_SIZE];
    while (1) {
        memset(buffer, 0, BUFFER_SIZE); // czyszczenie bufora przed odbiorem nowej wiadomości
        int received = recv(sockfd, buffer, BUFFER_SIZE, 0); // odbieranie danych z gniazda do bufora
        if (received <= 0) {
            printf("Server disconnected.\n");
            close(sockfd);
            exit(0);
        }
        buffer[received] = '\0';
        printf("%s", buffer);
    }
    return NULL;
}

// Klient przyjmuje jako swoje argumenty:
// - swoją nazwę/identyfikator (string o z góry ograniczonej długości)
// - adres serwera (adres IPv4 i numer portu)
int main(int argc, char *argv[]) {

    // nazwa_programu, nazwa_klienta, adres_ip, port
    if (argc != 4) {
        printf("Niepoprawna liczba argumentów.\n");
        return 1;
    }

    strncpy(name, argv[1], NAME_SIZE - 1); // kopiowanie nazwy do zmiennej name
    name[NAME_SIZE - 1] = '\0';

    char *server_ip = argv[2];
    int port = atoi(argv[3]);

    signal(SIGINT, handle_exit); // funkcja handle_exit jako obsługa sygnału SIGINT

    sockfd = socket(AF_INET, SOCK_STREAM, 0); // tworzę gniazdo TCP
    if (sockfd < 0) {
        perror("Socket failed");
        return 1;
    }

    // konfiguruję strukturę sockaddr_in
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    // nawiązuję połączenie z serwerem za pomocą connect
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }

    // wysyłamy nazwę klienta do serwera
    send(sockfd, name, strlen(name), 0);

    printf("Connected to server as %s\n", name);

    // tworzymy wątek do odbioru wiadomości
    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_handler, NULL);

    char input[BUFFER_SIZE];
    while (1) {
        memset(input, 0, BUFFER_SIZE);
        fgets(input, BUFFER_SIZE, stdin); // odczytuje dane od użytkownika

        if (strncmp(input, "STOP", 4) == 0) {
            handle_exit(SIGINT);
        } else if (strncmp(input, "LIST", 4) == 0 ||
                   strncmp(input, "ALIVE", 5) == 0 ||
                   strncmp(input, "2ALL", 4) == 0 ||
                   strncmp(input, "2ONE", 4) == 0) {
            send(sockfd, input, strlen(input), 0);
        } else {
            printf("Unknown command. Use: LIST, 2ALL <msg>, 2ONE <id> <msg>, STOP\n");
        }
    }

    close(sockfd);
    return 0;
}
