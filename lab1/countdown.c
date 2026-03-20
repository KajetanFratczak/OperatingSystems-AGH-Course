#include <stdio.h>

int main(void) {
    for(int i = 10; i >= 0; i--) {
        printf("%d\n", i);
    }
    return 0;
}

// GDB - test (robaczek)
// print i
// continue
// print i
// delete 1
// continue


// albo z konsoli
// break 5
// run
// itd tak samo