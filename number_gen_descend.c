#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TOTAL_NUMBERS 100
#define MIN -1000
#define MAX 10000000

// Comparison function for descending order
int compareDesc(const void *a, const void *b) {
    // If b > a, return positive to swap
    return (*(long*)b - *(long*)a);
}

int main() {
    long *numbers = malloc(TOTAL_NUMBERS * sizeof(long));
    if (numbers == NULL) return 1;

    srand(time(NULL));

    for (int i = 0; i < TOTAL_NUMBERS; i++) {
        numbers[i] = (rand() % (MAX - MIN + 1)) + MIN;
    }

    // Sort descending
    qsort(numbers, TOTAL_NUMBERS, sizeof(long), compareDesc);

    FILE *f = fopen("descending_10_2.txt", "w");
    for (int i = 0; i < TOTAL_NUMBERS; i++) {
        fprintf(f, "%ld\n", numbers[i]);
    }
    fclose(f);
    free(numbers);

    printf("Descending file created.\n");
    return 0;
}
