#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TOTAL_NUMBERS 100
#define MIN -100000
#define MAX 10000000

// Comparison function for ascending order
int compareAsc(const void *a, const void *b) {
    return (*(long*)a - *(long*)b);
}

int main() {
    long *numbers = malloc(TOTAL_NUMBERS * sizeof(long));
    if (numbers == NULL) return 1;

    srand(time(NULL));

    // 1. Generate random numbers
    for (int i = 0; i < TOTAL_NUMBERS; i++) {
        numbers[i] = (rand() % (MAX - MIN + 1)) + MIN;
    }

    // 2. Sort ascending
    qsort(numbers, TOTAL_NUMBERS, sizeof(long), compareAsc);

    // 3. Write to file
    FILE *f = fopen("ascending_10_2.txt", "w");
    for (int i = 0; i < TOTAL_NUMBERS; i++) {
        fprintf(f, "%ld\n", numbers[i]);
    }
    fclose(f);
    free(numbers);

    printf("Ascending file created.\n");
    return 0;
}
