#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

void flip(int arr[], int i)
{
    int temp, start = 0;
    while (start < i) {
        temp = arr[start];
        arr[start] = arr[i];
        arr[i] = temp;
        start++;
        i--;
    }
}

int findMax(int arr[], int n, int *isSorted)
{
    int mi = 0;
    *isSorted = 1;
    for (int i = 1; i < n; i++) {
        if (arr[i] > arr[mi]) {
            mi = i;
        }
        if (arr[i] < arr[i - 1]) {
            *isSorted = 0;
        }
    }
    return mi;
}

void pancakeSort(int* arr, int n)
{
    for (int curr_size = n; curr_size > 1; --curr_size)
    {
        int isSorted;
        int mi = findMax(arr, curr_size, &isSorted);

        if (isSorted) break;

        if (mi != curr_size - 1) {
            if (mi != 0) {
                flip(arr, mi);
            }
            flip(arr, curr_size - 1);
        }
    }
}

void printArray(int arr[], int n)
{
    for (int i = 0; i < n; ++i) {
        printf("%d\n", arr[i]);
    }
}

int main() {
    srand(time(NULL));

    FILE *file = fopen("ascending_10_4.txt", "r");
    if (!file) {
        perror("File error");
        return 1;
    }

    int temp, n = 0;
    while (fscanf(file, "%d", &temp) == 1) n++;

    if (n == 0) {
        printf("File is empty.\n");
        fclose(file);
        return 0;
    }

    int *arr = (int *)malloc(n * sizeof(int));
    if (!arr) {
        printf("Memory allocation failed.\n");
        fclose(file);
        return 1;
    }

    rewind(file);
    for (int i = 0; i < n; i++) {
        fscanf(file, "%d", &arr[i]);
    }
    fclose(file);

    LARGE_INTEGER frequency, start_time, end_time;
    QueryPerformanceFrequency(&frequency);

    printf("Sorting %d elements using Pankake...\n", n);

    QueryPerformanceCounter(&start_time);

    // Call the Merge Sort function
    pancakeSort(arr, n);

    QueryPerformanceCounter(&end_time);

    double time_taken = (double)(end_time.QuadPart - start_time.QuadPart) / frequency.QuadPart;

    // Memory Overhead: Merge Sort uses O(n) auxiliary space for temp arrays
    size_t heap_memory_overhead = n * sizeof(int);

    printf("\n--- Results ---\n");
    printf("Status: Success\n");
    printf("Time taken: %.10f seconds\n", time_taken);
    printf("Time in microseconds: %.3f ms\n", time_taken * 1000000.0);
    printf("Heap memory overhead (Auxiliary): %zu bytes (%.2f KB)\n",
            heap_memory_overhead, (float)heap_memory_overhead / 1024.0);
    printf("Original array storage: %.2f KB\n", (float)(n * sizeof(int)) / 1024.0);

    free(arr);
    return 0;
}
