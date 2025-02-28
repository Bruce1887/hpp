#!/bin/bash

# Start timer
start_time=$(date +%s.%N)

# Run the galsim command
echo "Running galsim with 3000 particles for 100 steps and delta_t=0.00001..."
output_time=$(./galsim 3000 input_data/ellipse_N_03000.gal 100 0.00001 0)

# Check if galsim ran successfully
if [ $? -ne 0 ]; then
  echo "galsim encountered an error. Exiting."
  exit 1
fi

# End timer and calculate execution time
end_time=$(date +%s.%N)
execution_time=$(echo "$end_time - $start_time" | bc)

echo "Simulation completed. Time taken: $execution_time seconds."

# Get the CPU model name
cpu_model=$(lscpu | grep "Model name" | sed 's/Model name:\s*//')

# Check if best_timing.txt exists
if [ ! -f best_timing.txt ]; then
  echo "best_timing.txt not found. Creating the file with initial high value."
  echo "99999999" > best_timing.txt  # Initialize with a very large value
  echo "$cpu_model" >> best_timing.txt  # Add the CPU model on the second line
fi

# Read the current best time from the file
best_time=$(head -n 1 best_timing.txt)

# Compare the current execution time with the best time
if (( $(echo "$execution_time < $best_time" | bc -l) )); then
  echo "New best time: $execution_time seconds."
  echo "$execution_time" > best_timing.txt  # Save the new best time on the first line
  echo "$cpu_model" >> best_timing.txt  # Ensure the CPU model remains on the second line
else
  echo "Best time remains: $best_time seconds."
fi

echo "Simulation and comparison completed successfully!"
