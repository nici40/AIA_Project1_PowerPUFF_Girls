#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h> // Required for QueryPerformanceCounter

#include <windows.h>
#include <stdio.h>
#include <psapi.h>
#include <math.h>

void printPeakMemory() {
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        printf("Peak Working Set Size: %zu bytes (%.2f MB)\n",
                pmc.PeakWorkingSetSize,
                (double)pmc.PeakWorkingSetSize / pow(1024,2));
    }
}

// Function to find the maximum value in the array
int getMax(int arr[], int n) {
    int mx = arr[0];
    for (int i = 1; i < n; i++)
        if (arr[i] > mx) mx = arr[i];
    return mx;
}

// Counting sort helper function
void countSort(int arr[], int n, int exp, int *output) {
    int count[10] = {0};

    // Store count of occurrences
    for (int i = 0; i < n; i++)
        count[(arr[i] / exp) % 10]++;

    // Change count[i] so it contains actual position of digit in output[]
    for (int i = 1; i < 10; i++)
        count[i] += count[i - 1];

    // Build the output array
    for (int i = n - 1; i >= 0; i--) {
        output[count[(arr[i] / exp) % 10] - 1] = arr[i];
        count[(arr[i] / exp) % 10]--;
    }

    // Copy output array back to arr[]
    for (int i = 0; i < n; i++)
        arr[i] = output[i];
}

// Main Radix Sort function
void radixSort(int arr[], int n) {
    if (n <= 0) return;

    int m = getMax(arr, n);

    // Allocate the auxiliary buffer once to be reused in each pass
    int *output = (int *)malloc(n * sizeof(int));
    if (output == NULL) return;

    for (int exp = 1; m / exp > 0; exp *= 10)
        countSort(arr, n, exp, output);

    free(output);
}

void merge_sort(int a[], int length);
void merge_sort_recursion(int a[], int l, int r);
void merge_sorted_arrays(int a[], int l, int m, int r);
void merge_sort(int a[], int length) {
    merge_sort_recursion(a, 0, length - 1);
}

void merge_sort_recursion(int a[], int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        merge_sort_recursion(a, l, m);
        merge_sort_recursion(a, m + 1, r);
        merge_sorted_arrays(a, l, m, r);
    }
}

void merge_sorted_arrays(int a[], int l, int m, int r) {
    int left_length = m - l + 1;
    int right_length = r - m;

    // HEAP ALLOCATION to prevent Stack Overflow (0xC00000FD)
    int *temp_left = (int *)malloc(left_length * sizeof(int));
    int *temp_right = (int *)malloc(right_length * sizeof(int));

    if (temp_left == NULL || temp_right == NULL) {
        perror("Merge malloc failed");
        exit(1);
    }

    int i, j, k;
    for (i = 0; i < left_length; i++) temp_left[i] = a[l + i];
    for (i = 0; i < right_length; i++) temp_right[i] = a[m + 1 + i];

    for (i = 0, j = 0, k = l; k <= r; k++) {
        if ((i < left_length) && (j >= right_length || temp_left[i] <= temp_right[j])) {
            a[k] = temp_left[i];
            i++;
        } else {
            a[k] = temp_right[j];
            j++;
        }
    }

    free(temp_left);
    free(temp_right);
}

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

int main() {
    srand(time(NULL));

    // CHANGE FILENAME HERE FOR 10^2, 10^4, 10^6
    const char* filename = "descending_10_6.txt";
    FILE *file = fopen(filename, "r");

    if (!file) {
        perror("File error");
        return 1;
    }

    // Count elements
    int temp, n = 0;
    while (fscanf(file, "%d", &temp) == 1) n++;

    if (n == 0) {
        printf("File is empty or not found.\n");
        fclose(file);
        return 0;
    }

    // Allocate main array
    int *arr = (int *)malloc(n * sizeof(int));
    if (!arr) {
        printf("Memory allocation failed.\n");
        fclose(file);
        return 1;
    }

    // Read data
    rewind(file);
    for (int i = 0; i < n; i++) {
        fscanf(file, "%d", &arr[i]);
    }
    fclose(file);

    LARGE_INTEGER frequency, start_time, end_time;
    QueryPerformanceFrequency(&frequency);

    printf("Sorting %d elements from %s...\n", n, filename);

    QueryPerformanceCounter(&start_time);

    // CHOOSE YOUR ALGORITHM HERE:
    // radixSort(arr, n);
    pancakeSort(arr,n);

    QueryPerformanceCounter(&end_time);

    double time_taken = (double)(end_time.QuadPart - start_time.QuadPart) / frequency.QuadPart;

    printf("\n--- Results ---\n");
    printf("Status: Success\n");
    printf("Time taken: %.10f seconds\n", time_taken);
    printf("Time in microseconds: %.3f ms\n", time_taken * 1000000.0);

    printPeakMemory();

    free(arr);
    return 0;
}



