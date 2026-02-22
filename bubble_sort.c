#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h> // Essential for QueryPerformanceCounter

// Bubble Sort implementation
void bubbleSort(int a[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            if (a[j] > a[j + 1]) {
                int temp = a[j];
                a[j] = a[j + 1];
                a[j + 1] = temp;
            }
        }
    }
}

int main() {
    // Seed the random number generator
    srand(time(NULL));

    // Change filename here if needed
    FILE *file = fopen("descending_10_6.txt", "r");
    if (!file) {
        perror("File error");
        return 1;
    }

    // Count elements in the file
    int temp, n = 0;
    while (fscanf(file, "%d", &temp) == 1) n++;

    if (n == 0) {
        printf("File is empty.\n");
        fclose(file);
        return 0;
    }

    // Allocate memory dynamically
    int *arr = (int *)malloc(n * sizeof(int));
    if (!arr) {
        printf("Memory allocation failed.\n");
        fclose(file);
        return 1;
    }

    // Load data from file into array
    rewind(file);
    for (int i = 0; i < n; i++) {
        fscanf(file, "%d", &arr[i]);
    }
    fclose(file);

    // High-resolution timing setup
    LARGE_INTEGER frequency, start_time, end_time;
    QueryPerformanceFrequency(&frequency);

    printf("Sorting %d elements using Bubble Sort...\n", n);
    if (n > 50000) {
        printf("Warning: Bubble Sort is very slow for large datasets. This may take a while.\n");
    }

    // --- START TRACKING ---
    QueryPerformanceCounter(&start_time);

    bubbleSort(arr, n);

    QueryPerformanceCounter(&end_time);
    // --- END TRACKING ---

    // Calculate metrics
    double time_taken = (double)(end_time.QuadPart - start_time.QuadPart) / frequency.QuadPart;

    // Bubble Sort is "In-Place", so extra heap memory used during sorting is 0
    size_t heap_memory_overhead = 0;

    printf("\n--- Results ---\n");
    printf("Status: Success\n");
    printf("Time taken: %.10f seconds\n", time_taken);
    printf("Time in microseconds: %.3f ms\n", time_taken * 1000000.0);
    printf("Heap memory overhead (Auxiliary): %zu bytes\n", heap_memory_overhead);
    printf("Original array storage: %.2f KB\n", (float)(n * sizeof(int)) / 1024);

    // Cleanup
    free(arr);
    return 0;
}
