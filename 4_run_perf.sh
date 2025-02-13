#!/bin/bash

# List of thread counts to test
thread_counts=(2 4 6 8 10 15 20 30 50 100)

# Output file for storing perf results
output_file="4_perf_results.csv"

# Write header to the output file
echo "Threads,CPU-Clock,Task-Clock,L1-Dcache-Loads,L1-Dcache-Load-Misses,Cache-References,Cache-Misses,Context-Switches" > $output_file

# Loop through each thread count
for threads in "${thread_counts[@]}"; do
    echo "Running with $threads threads..."

    # Run the program with perf (forcing numeric locale to avoid thousands separators)
    LC_NUMERIC=C sudo perf stat -e cpu-clock,task-clock,L1-dcache-loads,L1-dcache-load-misses,cache-references,cache-misses,context-switches -o temp_perf_output.txt ./4_mixed_bound $threads > /dev/null 2>&1

    # Extract metrics from perf output
    cpu_clock=$(grep "cpu-clock" temp_perf_output.txt | awk '{print $1}')
    task_clock=$(grep "task-clock" temp_perf_output.txt | awk '{print $1}')
    l1_dcache_loads=$(grep "L1-dcache-loads" temp_perf_output.txt | awk '{print $1}')
    l1_dcache_misses=$(grep "L1-dcache-load-misses" temp_perf_output.txt | awk '{print $1}')
    cache_refs=$(grep "cache-references" temp_perf_output.txt | awk '{print $1}')
    cache_misses=$(grep "cache-misses" temp_perf_output.txt | awk '{print $1}')
    context_switches=$(grep "context-switches" temp_perf_output.txt | awk '{print $1}')

    # Append results to the output file
    echo "$threads,$cpu_clock,$task_clock,$l1_dcache_loads,$l1_dcache_misses,$cache_refs,$cache_misses,$context_switches" >> $output_file

    # Clean up temporary file
    sudo rm temp_perf_output.txt
done

echo "Perf results saved to $output_file"
