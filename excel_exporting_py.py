import time
import csv
import os
import gc
import sys

# Importing functions from your files
from merge import mergeSort
from quick import quickSort
from pancake import pancake_sort

# Configuration
categories = ["random_numbers", "ascending", "descending", "partially_sorted"]
scales = ["10_2", "10_3", "10_4", "10_5"]
algo_names = ["Merge Sort", "Quick Sort", "Pancake Sort"]

def run_benchmarks():
    # Set recursion limit for large N to avoid crashes
    sys.setrecursionlimit(2000000) 

    with open('comprehensive_analysis_V1_PY.csv', 'w', newline='') as excel:
        writer = csv.writer(excel)

        for cat in categories:
            print(f"\n" + "="*60)
            print(f"🚀 STARTING CATEGORY: {cat}")
            print("="*60)
            writer.writerow([f"CATEGORY: {cat}"])
            
            header = ["Algorithm"]
            for s in scales:
                header.extend([f"Time {s} (us)", f"Status {s}"])
            writer.writerow(header)

            for name in algo_names:
                row = [name]
                print(f"\n🔹 Testing: {name}")

                for s in scales:
                    filename = f"{cat}_{s}.txt"
                    print(f"  📂 {filename:25} | ", end="", flush=True)

                    if not os.path.exists(filename):
                        print("❌ FILE MISSING")
                        row.extend(["FILE_MISSING", "N/A"])
                        continue

                    # Read data
                    with open(filename, 'r') as fp:
                        arr = [int(x) for x in fp.read().split()]
                    
                    n = len(arr)

                    # --- SAFETY TIMEOUTS ---
                    # Skipping Pancake Sort for N >= 10^5 to prevent hours of waiting
                    if name == "Pancake Sort" and n >= 100000:
                        print(f"⏩ SKIPPED (N={n} too slow for Pancake)")
                        row.extend(["TIMEOUT", "0"])
                        continue

                    time_sum = 0
                    success = True

                    print(f"Runs: ", end="", flush=True)
                    for i in range(1, 6):
                        print(f"{i}..", end="", flush=True)
                        temp_arr = list(arr)
                        gc.collect() 
                        
                        start = time.perf_counter()
                        try:
                            if name == "Merge Sort":
                                mergeSort(temp_arr, 0, n - 1)
                            elif name == "Quick Sort" and (cat == "ascending" and (s == "10_2"or s == "10_3" or s == "10_4")) or cat == "random_numbers" or (cat == "descending" and (s == "10_2"or s == "10_3" or s == "10_4")) or cat == "partially_sorted":
                                quickSort(temp_arr, 0, n - 1)
                            elif name == "Pancake Sort":
                                pancake_sort(temp_arr)
                        except Exception as e:
                            print(f"❌ Error: {e}")
                            success = False
                            break

                        end = time.perf_counter()
                        time_sum += (end - start) * 1_000_000
                        del temp_arr

                    if success:
                        mean_time = time_sum / 5
                        print(f" ✅ {mean_time:10.2f} us")
                        row.extend([f"{mean_time:.2f}", "SUCCESS"])
                    else:
                        row.extend(["ERROR", "CRASH"])

                writer.writerow(row)
            writer.writerow([]) 

    print("\n" + "="*60)
    print("🏁 BENCHMARKING COMPLETE! Check your CSV file.")
    print("="*60)

if __name__ == "__main__":
    run_benchmarks()