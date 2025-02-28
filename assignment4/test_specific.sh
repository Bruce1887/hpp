#!/bin/bash

# Check for correct number of arguments
if [ "$#" -ne 5 ]; then
  echo "Usage: $0 <N> <input_file> <reference_file> <nsteps> <delta_t>"
  exit 1
fi

# Assign input arguments to variables
N=$1
input_file=$2
reference_file=$3
nsteps=$4
delta_t=$5

# Run the first command
echo "Running galsim with $N particles for $nsteps steps and delta_t=$delta_t..."
./galsim $N $input_file $nsteps $delta_t 0

# Check if galsim ran successfully
if [ $? -ne 0 ]; then
  echo "galsim encountered an error. Exiting."
  exit 1
fi

# Define the output file path
output_file="our_outputs/output_$(basename $input_file)"

# Run the second command
echo "Running a.out to compare results..."
compare_gal_files/./a.out $N $reference_file $output_file

# Check if a.out ran successfully
if [ $? -ne 0 ]; then
  echo "a.out encountered an error. Exiting."
  exit 1
fi

echo "Simulation and comparison completed successfully!"