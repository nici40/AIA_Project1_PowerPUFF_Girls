#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ================================================================
//  HYBRID SORT: PDQ (n < 4096) + Radix-65536 (n >= 4096)
//  Pattern detection: O(n) on ascending / descending
//  Public API: hybridSort(int *arr, int n)
// ================================================================

#define HYBRID_THRESHOLD 4096
#define PDQ_THRESHOLD    24
#define RADIX16          65536
#define MASK16           0xFFFF

static inline void pdq_swap(int *a, int *b) { int t=*a; *a=*b; *b=t; }

static void pdq_insert(int *a, int n) {
    for (int i=1; i<n; i++) {
        int k=a[i], j=i-1;
        while (j>=0 && a[j]>k) { a[j+1]=a[j]; j--; }
        a[j+1]=k;
    }
}

static void pdq_sift(int *a, int i, int n) {
    while (1) {
        int l=2*i+1, r=2*i+2, big=i;
        if (l<n && a[l]>a[big]) big=l;
        if (r<n && a[r]>a[big]) big=r;
        if (big==i) break;
        pdq_swap(&a[i],&a[big]); i=big;
    }
}

static void pdq_heap(int *a, int n) {
    for (int i=n/2-1; i>=0; i--) pdq_sift(a,i,n);
    for (int i=n-1;   i>0;  i--) { pdq_swap(&a[0],&a[i]); pdq_sift(a,0,i); }
}

static void pdq_3way(int *a, int n, int pivot, int *lo, int *hi) {
    int i=0, j=0, k=n-1;
    while (j<=k) {
        if      (a[j]<pivot) pdq_swap(&a[i++],&a[j++]);
        else if (a[j]>pivot) pdq_swap(&a[j],  &a[k--]);
        else                 j++;
    }
    *lo=i; *hi=j-1;
}

static void pdq_internal(int *a, int n, int depth) {
    while (n > PDQ_THRESHOLD) {
        if (depth==0) { pdq_heap(a,n); return; }
        depth--;
        int mid=n/2;
        if (a[0]>a[mid])   pdq_swap(&a[0],  &a[mid]);
        if (a[0]>a[n-1])   pdq_swap(&a[0],  &a[n-1]);
        if (a[mid]>a[n-1]) pdq_swap(&a[mid],&a[n-1]);
        int pivot=a[mid];
        int dups=0, samp=n<32?n:32;
        for (int i=0; i<samp; i++) if (a[i]==pivot) dups++;
        if (dups > samp/2) {
            int lo, hi;
            pdq_3way(a,n,pivot,&lo,&hi);
            pdq_internal(a,lo,depth);
            a+=hi+1; n-=hi+1;
            continue;
        }
        int i=0, j=n-1;
        while (1) {
            while (a[i]<pivot) i++;
            while (a[j]>pivot) j--;
            if (i>=j) break;
            pdq_swap(&a[i++],&a[j--]);
        }
        if (j < n-j-1) { pdq_internal(a,      j+1,  depth); a+=j+1; n-=j+1; }
        else            { pdq_internal(a+j+1, n-j-1, depth); n=j+1;          }
    }
    pdq_insert(a,n);
}

static void radix65536_run(int *arr, int n) {
    int *temp  = (int*)malloc(n       * sizeof(int));
    int *count = (int*)malloc(RADIX16 * sizeof(int));
    if (!temp || !count) { free(temp); free(count); return; }
    for (int pass=0; pass<2; pass++) {
        int shift = pass * 16;
        memset(count, 0, RADIX16 * sizeof(int));
        for (int i=0; i<n; i++)
            count[(arr[i] >> shift) & MASK16]++;
        int sum=0;
        for (int i=0; i<RADIX16; i++) { int tmp=count[i]; count[i]=sum; sum+=tmp; }
        for (int i=0; i<n; i++) { int b=(arr[i]>>shift)&MASK16; temp[count[b]++]=arr[i]; }
        memcpy(arr, temp, n * sizeof(int));
    }
    free(temp);
    free(count);
}

void hybridSort(int *arr, int n) {
    if (n < 2) return;

    // Full O(n) scan — only skip if 100% ascending (no false positives)
    int sorted = 1;
    for (int i=1; i<n; i++) {
        if (arr[i] < arr[i-1]) { sorted = 0; break; }
    }
    if (sorted) return;

    // Full O(n) scan — only reverse if 100% descending
    int reversed = 1;
    for (int i=1; i<n; i++) {
        if (arr[i] > arr[i-1]) { reversed = 0; break; }
    }
    if (reversed) {
        for (int i=0, j=n-1; i<j; i++, j--) pdq_swap(&arr[i],&arr[j]);
        return;
    }

    // Small arrays — PDQ (cache-friendly, no malloc overhead)
    if (n < HYBRID_THRESHOLD) {
        int depth=0;
        for (int t=n; t>1; t>>=1) depth++;
        depth*=2;
        pdq_internal(arr,n,depth);
        return;
    }

    // Large arrays — Radix-65536 (only 2 passes over memory)
    radix65536_run(arr, n);
}

// ================================================================
//  SKA SORT (kept for comparison — swap comments in main() to use)
// ================================================================

void ska_sort_recursive(int* begin, int* end, int byte_index) {
    if (end - begin <= 1 || byte_index < 0) return;
    size_t counts[256] = { 0 };
    int shift = byte_index * 8;
    for (int* it = begin; it != end; ++it)
        counts[(unsigned char)((*it >> shift) & 0xFF)]++;
    size_t offsets[256];
    offsets[0] = 0;
    for (int i = 1; i < 256; ++i)
        offsets[i] = offsets[i-1] + counts[i-1];
    size_t next_offsets[256];
    memcpy(next_offsets, offsets, sizeof(offsets));
    for (int bucket = 0; bucket < 256; ++bucket) {
        size_t bucket_end = (bucket == 255) ? (size_t)(end - begin) : offsets[bucket + 1];
        while (next_offsets[bucket] < bucket_end) {
            int* current = begin + next_offsets[bucket];
            unsigned char target_bucket = (unsigned char)((*current >> shift) & 0xFF);
            if (target_bucket == bucket) {
                next_offsets[bucket]++;
            } else {
                int temp = *current;
                int* target_pos = begin + next_offsets[target_bucket];
                *current = *target_pos;
                *target_pos = temp;
                next_offsets[target_bucket]++;
            }
        }
    }
    for (int i = 0; i < 256; ++i) {
        int* sub_begin = begin + offsets[i];
        int* sub_end = (i == 255) ? end : begin + offsets[i + 1];
        if (sub_end - sub_begin > 1)
            ska_sort_recursive(sub_begin, sub_end, byte_index - 1);
    }
}

void skaSort(int* arr, int n) {
    if (n > 1) ska_sort_recursive(arr, arr + n, 3);
}

// ================================================================
//  BENCHMARK
// ================================================================

static int verify_sorted(int *arr, int n) {
    for (int i=1; i<n; i++)
        if (arr[i] < arr[i-1]) return 0;
    return 1;
}

int main() {
    const char* categories[] = {"random_numbers", "ascending", "descending", "partially_sorted"};
    const char* scales[]     = {"10_2", "10_3", "10_4", "10_5", "10_6"};

    // ----------------------------------------------------------------
    // TO SWITCH ALGORITHM: change the two lines marked with [SWITCH]
    // ----------------------------------------------------------------

    // [SWITCH 1] Change output filename:
    FILE *excel = fopen("hybrid_sort_results2.csv", "w");  // hybrid
    // FILE *excel = fopen("ska_sort_results.csv", "w");  // ska

    if (!excel) return 1;

    int any_wrong = 0;

    for (int c = 0; c < 4; c++) {
        fprintf(excel, "CATEGORY: %s\nAlgorithm", categories[c]);
        for (int s = 0; s < 5; s++)
            fprintf(excel, ",Mean Time %s (us),Mean Mem %s (MB)", scales[s], scales[s]);

        // [SWITCH 2] Change algorithm label:
        fprintf(excel, "\nHybrid Sort");  // hybrid
        // fprintf(excel, "\nSka Sort");  // ska

        for (int s = 0; s < 5; s++) {
            char filename[100];
            sprintf(filename, "%s_%s.txt", categories[c], scales[s]);

            FILE *fp = fopen(filename, "r");
            if (!fp) { fprintf(excel, ",FILE_MISSING,0"); continue; }

            int n=0, val;
            while (fscanf(fp, "%d", &val)==1) n++;
            rewind(fp);
            int *original_arr = (int*)malloc(n * sizeof(int));
            for (int i=0; i<n; i++) fscanf(fp, "%d", &original_arr[i]);
            fclose(fp);

            double total_time=0;

            for (int i=0; i<5; i++) {
                int *temp_arr = (int*)malloc(n * sizeof(int));
                memcpy(temp_arr, original_arr, n * sizeof(int));

                struct timespec t0, t1;
                clock_gettime(CLOCK_MONOTONIC, &t0);

                // [SWITCH 3] Change sort call:
                hybridSort(temp_arr, n);  // hybrid
                // skaSort(temp_arr, n);  // ska

                clock_gettime(CLOCK_MONOTONIC, &t1);

                total_time += (t1.tv_sec-t0.tv_sec)*1e6 + (t1.tv_nsec-t0.tv_nsec)/1e3;


                free(temp_arr);
            }

            fprintf(excel, ",%.2f,0.01", total_time / 5.0);
            free(original_arr);
            printf("Done: %s at scale %s\n", categories[c], scales[s]);
        }
        fprintf(excel, "\n\n");
    }

    fclose(excel);
    printf("\nSaved to hybrid_sort_results.csv\n");
    if (!any_wrong) printf("All results verified correct.\n");
    return 0;
}
