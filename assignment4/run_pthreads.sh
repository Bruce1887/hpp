#!/bin/bash

# Parameters
delta_t=0.00001
nsteps=100
max_threads=512  # Adjust as needed

# Input files for 5000 and 10000 particles
input_files=("input_data/ellipse_N_05000.gal" "input_data/ellipse_N_10000.gal")

# Loop through input files (5000 and 10000 particles)
for input_file in "${input_files[@]}"; do
    N=$(basename "$input_file" | grep -oP '\d+')

    if [ ! -f "$input_file" ]; then
        echo "Error: Input file $input_file not found!"
        continue
    fi

    best_time=99999  # Initialize with a high value

    # Test different thread counts (1, 2, 4, 8, ..., max_threads)
    for num_threads in 1 2 4 8 16 32 64 128 256 512; do
        if (( num_threads > max_threads )); then
            break
        fi

        echo "Running galsim (Pthreads) with $N particles, $nsteps steps, ∆t=$delta_t, threads=$num_threads..."

        start_time=$(date +%s.%N)
        ./galsim "$N" "$input_file" "$nsteps" "$delta_t" 0 "$num_threads" > /dev/null 2>&1
        end_time=$(date +%s.%N)

        elapsed_time=$(echo "$end_time - $start_time" | bc)

        echo "Simulation completed. Time taken: $elapsed_time seconds."

        # Track best time
        if (( $(echo "$elapsed_time < $best_time" | bc -l) )); then
            best_time=$elapsed_time
        fi

        echo "Best time remains: $best_time seconds."
    done

    echo "Final best time for $N particles: $best_time seconds."
    echo "--------------------------------------------"
done
