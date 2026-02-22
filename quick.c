#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h> // For high-resolution timing

// Swap helper function
void swap(int* a, int* b) {
    int t = *a;
    *a = *b;
    *b = t;
}

int pivot_(int array[], int start, int end) {
    // RANDOMIZED PIVOT: Prevents Stack Overflow on sorted/descending data
    int randomIndex = start + rand() % (end - start + 1);
    swap(&array[randomIndex], &array[end]);

    int pivot = array[end];
    int i = (start - 1);

    for (int j = start; j < end; j++) {
        if (array[j] < pivot) {
            i++;
            swap(&array[i], &array[j]);
        }
    }
    swap(&array[i + 1], &array[end]);
    return (i + 1);
}

void quick(int array[], int start, int end) {
    if (start < end) {
        int pi = pivot_(array, start, end);
        quick(array, start, pi - 1);
        quick(array, pi + 1, end);
    }
}

int main() {
    // Seed the random number generator
    srand(time(NULL));

    FILE *file = fopen("random_numbers_10_6.txt", "r");
    if (!file) {
        perror("File error");
        return 1;
    }

    // Count elements
    int temp, n = 0;
    while (fscanf(file, "%d", &temp) == 1) n++;

    if (n == 0) {
        printf("File is empty.\n");
        fclose(file);
        return 0;
    }

    // Allocate memory
    int *arr = (int *)malloc(n * sizeof(int));
    if (!arr) {
        printf("Memory allocation failed.\n");
        fclose(file);
        return 1;
    }

    // Load data
    rewind(file);
    for (int i = 0; i < n; i++) {
        fscanf(file, "%d", &arr[i]);
    }
    fclose(file);

    // High-resolution timing setup
    LARGE_INTEGER frequency, start_time, end_time;
    QueryPerformanceFrequency(&frequency);

    printf("Sorting %d elements...\n", n);

    // --- START TRACKING ---
    QueryPerformanceCounter(&start_time);

    quick(arr, 0, n - 1);

    QueryPerformanceCounter(&end_time);
    // --- END TRACKING ---

    // Calculate metrics
    double time_taken = (double)(end_time.QuadPart - start_time.QuadPart) / frequency.QuadPart;

    // QuickSort is "In-Place", meaning it uses no extra heap memory for the sort itself
    size_t heap_memory_overhead = 0;

    printf("\n--- Results ---\n");
    printf("Status: Success (No Crash)\n");
    printf("Time taken: %.10f seconds\n", time_taken);
    printf("Time in milliseconds: %.3f ms\n", time_taken * 1000000.0);
    printf("Heap memory overhead: %zu bytes\n", heap_memory_overhead);
    printf("Original array storage: %.2f KB\n", (float)(n * sizeof(int)) / 1024);

    // Cleanup
    free(arr);
    return 0;
}
