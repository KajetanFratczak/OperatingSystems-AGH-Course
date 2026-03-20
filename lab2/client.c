#include <stdio.h>
#include <dlfcn.h>
#include "collatz.h"

#ifdef DYNAMIC_LOAD

void run_dynamic (int input, int max_iter) {
    // Kerrisk - 42 chapter
    // The dlopen() function opens a shared library, returning a handle used by subsequent calls.
    // The dlsym() function searches a library for a symbol (a string containing the name of a function or variable) and returns its address.
    // The dlclose() function closes a library previously opened by dlopen().
    // The dlerror() function returns an error-message string, and is used after a failure return from one of the preceding functions.

    void *handle = dlopen("./libcollatz.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Błąd ładowania biblioteki: %s\n", dlerror());
        return;
    }

    int (*test_collatz_convergence) (int, int, int*);
    *(void **)(&test_collatz_convergence) = dlsym(handle, "test_collatz_convergence");
    // 2 sposob (bardziej czytelny): collatz_func test_collatz_convergence = (collatz_func) dlsym(handle, "test_collatz_convergence");
    if (dlerror() != NULL) {
        printf("Błąd pobierania funkcji");
        return;
    }

    int steps[max_iter];
    int steps_count = test_collatz_convergence(input, max_iter, steps);

    if (input==1) printf("Do 1 doszliśmy w %d iteracjach.\n", 0);
    else {
        if (steps_count == 0) {
            printf("Nie udało się dojść do 1 w %d iteracjach.\n", max_iter);
        } else {
            printf("Ciąg Collatza dla %d:\n", input);
            for (int i = 0; i <= steps_count; i++) {
                printf("%d ", steps[i]);
            }
            printf("\nLiczba kroków: %d\n", steps_count);
        }
    }

    dlclose(handle);
}
#else

// Wersja statyczna lub współdzielona
void run_static_or_shared (int input, int max_iter) {
    int steps[max_iter];
    int steps_count = test_collatz_convergence(input, max_iter, steps);

    if (input==1) printf("Do 1 doszliśmy w %d iteracjach.\n", 0);
    else {
        if (steps_count == 0) {
            printf("Nie udało się dojść do 1 w %d iteracjach.\n", max_iter);
        } else {
            printf("Ciąg Collatza dla %d:\n", input);
            for (int i = 0; i <= steps_count; i++) {
                printf("%d ", steps[i]);
            }
            printf("\nLiczba kroków: %d\n", steps_count);
        }
    }

    // Komentarz: można było po prostu nie robic dwóch oddzielnych funkcji, tylko wywołanie
    // zrobić z użyciem dyrektywy preprocesora #ifdef
}

#endif

int main() {
    int input, max_iter;
    printf("Podaj liczbę: ");
    scanf("%d", &input);
    printf("Podaj górną granicę iteracji: ");
    scanf("%d", &max_iter);

#ifdef DYNAMIC_LOAD
    run_dynamic(input, max_iter);
#else
    run_static_or_shared(input, max_iter);
#endif

    return 0;
}



