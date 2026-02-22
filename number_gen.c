#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TOTAL_NUMBERS 100
#define MIN -10000000
#define MAX 10000000

int main() {
    FILE *filePtr;

    // Seed the random number generator with the current time
    srand(time(NULL));

    // Open file for writing ("w")
    filePtr = fopen("random_numbers_10_2.txt", "w");

    if (filePtr == NULL) {
        printf("Error: Could not open file for writing.\n");
        return 1;
    }

    printf("Generating %d numbers... please wait.\n", TOTAL_NUMBERS);

    for (int i = 0; i < TOTAL_NUMBERS; i++) {
        // Formula for generating numbers in a specific range:
        // rand() % (high - low + 1) + low
        long randomNumber = (rand() % (MAX - MIN + 1)) + MIN;

        // Write to file, one number per line
        fprintf(filePtr, "%ld\n", randomNumber);
    }

    // Close the file stream
    fclose(filePtr);

    printf("Done! Numbers saved to 'random_numbers.txt'.\n");

    return 0;
}
