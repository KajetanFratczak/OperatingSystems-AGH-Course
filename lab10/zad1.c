// Program należy zaimplementować korzystając z wątków (pthread) i mechanizmów synchronizacji biblioteki
// POSIX Threads (mutex, condition variable). Po uruchomieniu programu wątek główny tworzy wątki dla Lekarza,
// Pacjentów oraz Farmaceutów. Ilość pacjentów i farmaceutów powinna być możliwa do przekazania jako parametr
// uruchomieniowy programu (pierwszy parametr to ilość pacjentów, drugi parametr to ilość farmaceutów).
// Praca lekarza się kończy, gdy nie ma więcej pacjentów do wyleczenia. Do spania Lekarza powinny być
// wykorzystane Warunki Sprawdzające (Condition Variables).
// Użycie odpowiednich mechanizmów ma zagwarantować niedopouszczenie, np. do zdarzeń: Lekarz śpi chociaż
// czeka na niego 3 pacjentów lub Farmaceuta nie jest w stanie uzupełnić leków, więcej niż jeden farmaceuta
// uzupełnia apteczke itp.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define MAX_LICZBA_CZEKAJACYCH_PACJENTOW 3
#define POJEMNOSC_APTECZKI 6
#define LEKI_NA_KONSULTACJE 3

typedef struct {
    int id;
    int kolejnosc;
} Pacjent;

typedef struct {
    int id;
} Farmaceuta;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// zmienne warunkowe do budzenia odpowiednich wątków
pthread_cond_t cond_doctor = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_patient = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_pharmacist = PTHREAD_COND_INITIALIZER;

int czekajacy_pacjenci = 0;
int czekajacy_farmaceuci = 0;
int liczba_pacjentow, liczba_farmaceutow;
int liczba_lekow_w_apteczce = POJEMNOSC_APTECZKI;
int pozostali_pacjenci;
int program_finished = 0;
int kolejka_pacjentow[100];
int poczatek_kolejki = 0;
int koniec_kolejki = 0;
int nastepny_numer_kolejki = 1;
int obsluzeni_pacjenci = 0;

// funkcja do wypisywania dokładnego czasu w systemie
void print_timestamp() {
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
    printf("[%s] ", buffer);
}

// funkcja dodawania pacjenta do kolejki
void dodaj_do_kolejki(int id) {
    kolejka_pacjentow[koniec_kolejki] = id;
    koniec_kolejki = (koniec_kolejki + 1) % 100;
}

// funkcja pobierania pacjenta z kolejki
int pobierz_z_kolejki() {
    if (poczatek_kolejki == koniec_kolejki) return -1;
    int id = kolejka_pacjentow[poczatek_kolejki];
    poczatek_kolejki = (poczatek_kolejki + 1) % 100;
    return id;
}

void *patient_thread(void *arg) {
    Pacjent *pac = (Pacjent *)arg;
    int id = pac->id;
    int moja_kolejnosc = -1;

    while (!program_finished) {
        // pacjent idzie do szpitala (2-5 sekund)
        int walk_time = rand() % 4 + 2;
        print_timestamp();
        printf("Pacjent(%d): Idę do szpitala, będę za %d s\n", id, walk_time);
        sleep(walk_time);

        pthread_mutex_lock(&mutex);

        if (program_finished) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        // jeśli jest już 3 pacjentów w poczekalni, to odchodzi i wraca później
        if (czekajacy_pacjenci >= MAX_LICZBA_CZEKAJACYCH_PACJENTOW) {
            int retry_time = rand() % 3 + 1;
            print_timestamp();
            printf("Pacjent(%d): Za dużo pacjentów, wracam później za %d s\n", id, retry_time);
            pthread_mutex_unlock(&mutex);
            sleep(retry_time);
            continue;
        }

        // otrzymuje numer i zostaje dodany do kolejki
        czekajacy_pacjenci++;
        moja_kolejnosc = nastepny_numer_kolejki++;
        dodaj_do_kolejki(id);
        print_timestamp();
        printf("Pacjent(%d): czeka %d pacjentów na lekarza (kolejność: %d)\n", id, czekajacy_pacjenci, moja_kolejnosc);

        // jeśli to trzeci pacjent i jest odpowiednia ilość leków, to budzi lekarza
        if (czekajacy_pacjenci == MAX_LICZBA_CZEKAJACYCH_PACJENTOW &&
            liczba_lekow_w_apteczce >= LEKI_NA_KONSULTACJE) {
            print_timestamp();
            printf("Pacjent(%d): budzę lekarza\n", id);
            pthread_cond_signal(&cond_doctor);
        }

        // czeka na swoją kolej
        while (moja_kolejnosc > obsluzeni_pacjenci && !program_finished) {
            pthread_cond_wait(&cond_patient, &mutex);
        }

        if (program_finished) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        print_timestamp();
        printf("Pacjent(%d): kończę wizytę\n", id);
        pthread_mutex_unlock(&mutex);
        break;
    }

    return NULL;
}

void *pharmacist_thread(void *arg) {
    Farmaceuta *far = (Farmaceuta *)arg;
    int id = far->id;

    while (!program_finished) {
        // idzie do szpitala (5-15 sekund)
        int walk_time = rand() % 11 + 5;
        print_timestamp();
        printf("Farmaceuta(%d): idę do szpitala, będę za %d s\n", id, walk_time);
        sleep(walk_time);

        pthread_mutex_lock(&mutex);

        if (program_finished) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        // sprawdza liczbę leków w apteczce
        if (liczba_lekow_w_apteczce >= POJEMNOSC_APTECZKI) {
            print_timestamp();
            printf("Farmaceuta(%d): czekam na opróżnienie apteczki\n", id);
            while (liczba_lekow_w_apteczce >= POJEMNOSC_APTECZKI && !program_finished) {
                pthread_cond_wait(&cond_pharmacist, &mutex);
            }
            if (program_finished) {
                pthread_mutex_unlock(&mutex);
                break;
            }
        }

        czekajacy_farmaceuci++;
        print_timestamp();
        printf("Farmaceuta(%d): budzę lekarza\n", id);
        pthread_cond_signal(&cond_doctor);

        while (czekajacy_farmaceuci > 0 && !program_finished) {
            pthread_cond_wait(&cond_pharmacist, &mutex);
        }

        print_timestamp();
        printf("Farmaceuta(%d): zakończyłem dostawę\n", id);
        pthread_mutex_unlock(&mutex);
        break;
    }

    return NULL;
}

void *doctor_thread(void *arg) {
    while (!program_finished) {
        pthread_mutex_lock(&mutex);

        while (!((czekajacy_pacjenci >= MAX_LICZBA_CZEKAJACYCH_PACJENTOW &&
                  liczba_lekow_w_apteczce >= LEKI_NA_KONSULTACJE) ||
                 (czekajacy_farmaceuci > 0 && liczba_lekow_w_apteczce < POJEMNOSC_APTECZKI)) &&
               !program_finished) {
            // nie ma już pacjentów - kończymy
            if (pozostali_pacjenci <= 0 && czekajacy_pacjenci == 0) {
                program_finished = 1;
                pthread_cond_broadcast(&cond_doctor);
                pthread_cond_broadcast(&cond_patient);
                pthread_cond_broadcast(&cond_pharmacist);
                pthread_mutex_unlock(&mutex);
                print_timestamp();
                printf("Lekarz: wszyscy pacjenci obsłużeni, kończę pracę\n");
                return NULL;
            }

            print_timestamp();
            printf("Lekarz: zasypiam\n");
            pthread_cond_wait(&cond_doctor, &mutex);
        }

        print_timestamp();
        printf("Lekarz: budzę się\n");

        // jeśli 3 pacjentów czeka i mamy odpowiednią liczbę leków
        if (czekajacy_pacjenci >= MAX_LICZBA_CZEKAJACYCH_PACJENTOW &&
            liczba_lekow_w_apteczce >= LEKI_NA_KONSULTACJE) {
            int pacjenci[3];
            for (int i = 0; i < MAX_LICZBA_CZEKAJACYCH_PACJENTOW; i++) {
                pacjenci[i] = pobierz_z_kolejki();
            }
            print_timestamp();
            printf("Lekarz: konsultuję pacjentów %d, %d, %d\n", pacjenci[0], pacjenci[1], pacjenci[2]);

            liczba_lekow_w_apteczce -= LEKI_NA_KONSULTACJE;
            pozostali_pacjenci -= MAX_LICZBA_CZEKAJACYCH_PACJENTOW;
            czekajacy_pacjenci -= MAX_LICZBA_CZEKAJACYCH_PACJENTOW;

            for (int i = 0; i < MAX_LICZBA_CZEKAJACYCH_PACJENTOW; i++) {
                obsluzeni_pacjenci++;
                pthread_cond_signal(&cond_patient);
            }

            pthread_mutex_unlock(&mutex);

            sleep(rand() % 3 + 2);

            pthread_mutex_lock(&mutex);
            print_timestamp();
            printf("Lekarz: zakończono konsultację\n");
            pthread_cond_broadcast(&cond_patient);
        } else if (czekajacy_farmaceuci > 0 && liczba_lekow_w_apteczce < POJEMNOSC_APTECZKI) {
            print_timestamp();
            printf("Lekarz: przyjmuję dostawę leków\n");

            liczba_lekow_w_apteczce = POJEMNOSC_APTECZKI;
            czekajacy_farmaceuci--;

            pthread_mutex_unlock(&mutex);

            sleep(rand() % 3 + 1);

            pthread_mutex_lock(&mutex);
            print_timestamp();
            printf("Lekarz: przyjąłem dostawę\n");
            pthread_cond_broadcast(&cond_pharmacist);
        }

        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Niepoprawna liczba argumentów\n");
        return 1;
    }

    liczba_pacjentow = atoi(argv[1]);
    liczba_farmaceutow = atoi(argv[2]);
    pozostali_pacjenci = liczba_pacjentow;

    if (liczba_pacjentow < 1 || liczba_farmaceutow < 1) {
        printf("Nieprawidłowa liczba pacjentów i/lub farmaceutów\n");
        return 1;
    }

    srand(time(NULL));

    pthread_t doctor;
    pthread_t patients[liczba_pacjentow];
    pthread_t pharmacists[liczba_farmaceutow];
    Pacjent dane_pacjentow[liczba_pacjentow];
    Farmaceuta dane_farmaceutow[liczba_farmaceutow];

    pthread_create(&doctor, NULL, doctor_thread, NULL);

    for (int i = 0; i < liczba_pacjentow; i++) {
        dane_pacjentow[i].id = i + 1;
        dane_pacjentow[i].kolejnosc = 0;
        pthread_create(&patients[i], NULL, patient_thread, &dane_pacjentow[i]);
    }

    for (int i = 0; i < liczba_farmaceutow; i++) {
        dane_farmaceutow[i].id = i + 1;
        pthread_create(&pharmacists[i], NULL, pharmacist_thread, &dane_farmaceutow[i]);
    }

    for (int i = 0; i < liczba_pacjentow; i++) {
        pthread_join(patients[i], NULL);
    }

    pthread_mutex_lock(&mutex);
    program_finished = 1;
    pthread_cond_broadcast(&cond_doctor);
    pthread_cond_broadcast(&cond_patient);
    pthread_cond_broadcast(&cond_pharmacist);
    pthread_mutex_unlock(&mutex);

    for (int i = 0; i < liczba_farmaceutow; i++) {
        pthread_join(pharmacists[i], NULL);
    }

    pthread_join(doctor, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_doctor);
    pthread_cond_destroy(&cond_patient);
    pthread_cond_destroy(&cond_pharmacist);

    print_timestamp();
    printf("Koniec programu\n");

    return 0;
}