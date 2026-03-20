int collatz_conjecture(int input) {
    if (input%2 == 0) {
        return input/2;
    }
    return 3*input + 1;
}

int test_collatz_convergence(int input, int max_iter, int *steps) {
    // dla 1 liczyło mi iteracje, wiec ten przypadek jest oddzielnie (wiem ze mozna zacząć od 0 i nie ma tego problemu)
    if (input == 1) return 0;

    steps[0] = input;
    int result = input;
    int iter = 1;

    while (iter < max_iter && result != 1) {
        result = collatz_conjecture(result);
        steps[iter] = result;
        iter++;
    }

    if (result == 1) {
        return iter-1;
    }
    return 0;
}

// int test_collatz_convergence(int input, int max_iter, int *steps) {
//
//     int result = input;
//     int iter = 0;
//
//     while (iter < max_iter && result != 1) {
//         steps[iter] = result;
//         result = collatz_conjecture(result);
//         iter++;
//     }
//
//     if (result == 1 && iter < max_iter) {
//         steps[iter] = result;
//         return iter;
//     }
//     return 0;
// }

// int main() {
//     int input = 15;
//     int max_iter = 10;
//     int steps[max_iter];
//
//     int steps_count = test_collatz_convergence(input, max_iter, steps);
//
//     printf("%d\n", steps_count);
//     for (int i = 0; i < steps_count + 1; i++) {
//         printf("%d ", steps[i]);
//     }
//
//     return 0;
// }