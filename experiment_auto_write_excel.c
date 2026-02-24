#include <windows.h>
#include <psapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// Array of files to process

void bitBubbleSort(unsigned int a[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            // Branchless swap: (a > b) results in -1 (all bits 1), else 0
            int mask = -(a[j] > a[j+1]);
            unsigned int t = mask & (a[j] ^ a[j+1]);
            a[j] ^= t;
            a[j+1] ^= t;
        }
    }
}

void bitwiseCompare(unsigned int a[], int i, int j, int dir) {
    // Branchless bitwise comparison and swap
    if (dir == (a[i] > a[j])) {
        a[i] ^= a[j];
        a[j] ^= a[i];
        a[i] ^= a[j];
    }
}

void bitonicMerge(unsigned int a[], int low, int cnt, int dir) {
    if (cnt > 1) {
        int k = cnt / 2;
        for (int i = low; i < low + k; i++)
            bitwiseCompare(a, i, i + k, dir);
        bitonicMerge(a, low, k, dir);
        bitonicMerge(a, low + k, k, dir);
    }
}

void binaryQuickSort(unsigned int a[], int left, int right, int bit) {
    if (left >= right || bit < 0) return;

    int i = left, j = right;
    while (i <= j) {
        // Find first element with bit set to 1
        while (i <= j && !((a[i] >> bit) & 1)) i++;
        // Find last element with bit set to 0
        while (i <= j && ((a[j] >> bit) & 1)) j--;

        if (i < j) {
            // Bitwise XOR Swap
            a[i] ^= a[j];
            a[j] ^= a[i];
            a[i] ^= a[j];
        }
    }
    binaryQuickSort(a, left, j, bit - 1);
    binaryQuickSort(a, i, right, bit - 1);
}

void fastRadixSort(int arr[], int n) {
    if (n <= 0) return;

    unsigned int *u_arr = (unsigned int *)arr;
    unsigned int *output = (unsigned int *)malloc(n * sizeof(unsigned int));
    if (!output) return;

    // STEP 1: Flip sign bit to handle signed integers in one go
    for (int i = 0; i < n; i++) {
        u_arr[i] ^= 0x80000000;
    }

    // STEP 2: 4 Passes (8 bits each)
    for (int shift = 0; shift < 32; shift += 8) {
        int count[256] = {0};

        // Counting occurrences of the current byte
        for (int i = 0; i < n; i++) {
            count[(u_arr[i] >> shift) & 0xFF]++;
        }

        // Prefix sum (positions)
        for (int i = 1; i < 256; i++) {
            count[i] += count[i - 1];
        }

        // Build output array (backwards for stability)
        for (int i = n - 1; i >= 0; i--) {
            output[--count[(u_arr[i] >> shift) & 0xFF]] = u_arr[i];
        }

        // Copy back to main array
        memcpy(u_arr, output, n * sizeof(unsigned int));
    }

    // STEP 3: Flip sign bit back
    for (int i = 0; i < n; i++) {
        u_arr[i] ^= 0x80000000;
    }

    free(output);
}

void countSortUnsigned(unsigned int arr[], int n, unsigned int exp, unsigned int *output) {
    int count[10] = {0};

    for (int i = 0; i < n; i++)
        count[(arr[i] / exp) % 10]++;

    for (int i = 1; i < 10; i++)
        count[i] += count[i - 1];

    for (int i = n - 1; i >= 0; i--) {
        output[count[(arr[i] / exp) % 10] - 1] = arr[i];
        count[(arr[i] / exp) % 10]--;
    }

    for (int i = 0; i < n; i++)
        arr[i] = output[i];
}

void radixSortAll(int arr[], int n) {
    if (n <= 0) return;

    // Use an unsigned pointer to treat the bits correctly
    unsigned int *u_arr = (unsigned int *)arr;

    // STEP 1: Flip the sign bit (0x80000000)
    // This maps the most negative number to 0 and the most positive to UINT_MAX
    for (int i = 0; i < n; i++) {
        u_arr[i] ^= 0x80000000;
    }

    // STEP 2: Find the new maximum for Radix passes
    unsigned int maxVal = u_arr[0];
    for (int i = 1; i < n; i++) {
        if (u_arr[i] > maxVal) maxVal = u_arr[i];
    }

    // STEP 3: Perform standard Radix Sort passes
    unsigned int *output = (unsigned int *)malloc(n * sizeof(unsigned int));
    if (output == NULL) return;

    // Using long long for 'exp' to prevent overflow when multiplying by 10
    for (unsigned long long exp = 1; maxVal / exp > 0; exp *= 10) {
        countSortUnsigned(u_arr, n, (unsigned int)exp, output);
    }

    // STEP 4: Flip the sign bit back to restore original signed values
    for (int i = 0; i < n; i++) {
        u_arr[i] ^= 0x80000000;
    }

    free(output);
}


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

#include <windows.h>
#include <psapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Define the categories and scales
const char* categories[] = {"random_numbers", "ascending", "descending", "partially_sorted"};
const char* scales[] = {"10_2", "10_3", "10_4", "10_5", "10_6"};
const char* algo_names[] = {"Radix Sort", "Quick Sort","Merge Sort" , "Bubble Sort", "Pancake Sort"};

double getCurrentMemoryMB() {
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return (double)pmc.WorkingSetSize / (1024.0 * 1024.0);
    }
    return 0;
}

int main() {
    srand(time(NULL));
    FILE *excel = fopen("comprehensive_analysis_V2.csv", "w");
    if (!excel) {
        perror("Error opening CSV file");
        return 1;
    }

    int num_cats = sizeof(categories) / sizeof(categories[0]);
    int num_scales = sizeof(scales) / sizeof(scales[0]);
    int num_algos = sizeof(algo_names) / sizeof(algo_names[0]);

    for (int c = 0; c < num_cats; c++) {
        fprintf(excel, "CATEGORY: %s\n", categories[c]);
        fprintf(excel, "Algorithm");
        for (int s = 0; s < num_scales; s++) {
            fprintf(excel, ",Time %s (us),Max Mem %s (MB)", scales[s], scales[s]);
        }
        fprintf(excel, "\n");

        for (int a = 0; a < num_algos; a++) {
            fprintf(excel, "%s", algo_names[a]);

            for (int s = 0; s < num_scales; s++) {
                char filename[100];
                sprintf(filename, "%s_%s.txt", categories[c], scales[s]);

                FILE *fp = fopen(filename, "r");
                if (!fp) {
                    fprintf(excel, ",FILE_MISSING,0");
                    continue;
                }

                int n = 0, temp;
                while (fscanf(fp, "%d", &temp) == 1) n++;
                rewind(fp);

                int *arr = (int*)malloc(n * sizeof(int));
                for (int i = 0; i < n; i++) fscanf(fp, "%d", &arr[i]);
                fclose(fp);

                // --- IMPROVED MEMORY RESET ---
                EmptyWorkingSet(GetCurrentProcess());
                // Small delay to let Windows process the flush
                Sleep(10);
                double baselineMem = getCurrentMemoryMB();

                LARGE_INTEGER freq, start, end;
                QueryPerformanceFrequency(&freq);

                if ((a == 3 || a == 4) && n > 10000) {
                    fprintf(excel, ",TIMEOUT,0");
                } else {
                    QueryPerformanceCounter(&start);

                    if (a == 0)      fastRadixSort(arr, n);
                    else if (a == 1) quick(arr, 0, n - 1);
                    else if (a == 2) merge_sort(arr, n);
                    else if (a == 3) bubbleSort(arr, n);
                    else if (a == 4) pancakeSort(arr, n);

                    QueryPerformanceCounter(&end);

                    // Get memory usage after the sort
                    double afterMem = getCurrentMemoryMB();

                    // The 'Max' memory is the additional RAM consumed by the algorithm
                    // We add a tiny offset for in-place algorithms to show they used "some" stack
                    double algorithmMemory = afterMem - baselineMem;
                    if (algorithmMemory < 0.01) algorithmMemory = 0.01;

                    double time_us = ((double)(end.QuadPart - start.QuadPart) * 1000000.0) / freq.QuadPart;

                    fprintf(excel, ",%.2f,%.2f", time_us, algorithmMemory);
                }
                free(arr);
            }
            fprintf(excel, "\n");
        }
        fprintf(excel, "\n");
        printf("Finished benchmarking Category: %s\n", categories[c]);
    }

    fclose(excel);
    return 0;
}
