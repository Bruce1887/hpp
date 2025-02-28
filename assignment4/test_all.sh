#!/bin/bash

# Create the output directory if it doesn't exist
mkdir -p our_outputs

# List of specific input and reference files
input_files=(
  "input_data/ellipse_N_00010.gal"
  "input_data/ellipse_N_00100.gal"
  "input_data/ellipse_N_00500.gal"
  "input_data/ellipse_N_01000.gal"
  "input_data/ellipse_N_02000.gal"
  "input_data/ellipse_N_03000.gal"
)

reference_files=(
  "ref_output_data/ellipse_N_00010_after200steps.gal"
  "ref_output_data/ellipse_N_00100_after200steps.gal"
  "ref_output_data/ellipse_N_00500_after200steps.gal"
  "ref_output_data/ellipse_N_01000_after200steps.gal"
  "ref_output_data/ellipse_N_02000_after200steps.gal"
  "ref_output_data/ellipse_N_03000_after100steps.gal"
)

nsteps_values=(200 200 200 200 200 100)
delta_t=0.00001

# Loop over both input and reference files
for i in "${!input_files[@]}"; do
  input_file="${input_files[$i]}"
  reference_file="${reference_files[$i]}"
  nsteps="${nsteps_values[$i]}"
  N=$(basename "$input_file" | grep -oP '\d+')


  # Check if input file exists
  if [ ! -f "$input_file" ]; then
    echo "Warning: Input file $input_file not found. Skipping..."
    continue
  fi

  # Print the number of steps for this simulation
  echo ""
  echo "----------------------------------------------"
  echo "Running simulation for:"
  echo "- Particles: $N"
  echo "- Steps: $nsteps"
  echo "- Time step: $delta_t"
  echo "----------------------------------------------"

  start_time=$(date +%s.%N)
  # Run the simulation
  echo "Running galsim for $N particles with $nsteps steps and delta_t=$delta_t..."
  ./galsim $N "$input_file" $nsteps $delta_t 0 > /dev/null 2>&1
  end_time=$(date +%s.%N)
  elapsed_time=$(echo "$end_time - $start_time" | bc)
  echo "Elapsed time: $elapsed_time seconds"

  # Check if galsim ran successfully
  if [ $? -ne 0 ]; then
    echo "galsim encountered an error for $input_file. Exiting."
    exit 1
  fi

  # Define output file path. (det här är alltid result.gal, dom kräver det i assignment instruktionerna)
  output_file="result.gal"

  # Check if reference file exists
  if [ ! -f "$reference_file" ]; then
    echo "Warning: Reference file $reference_file not found. Skipping comparison..."
    continue
  fi

  # Run the comparison
  echo "Comparing output against reference file: $reference_file"
  compare_gal_files/./a.out $N "$reference_file" "$output_file"

  # Check if comparison ran successfully
  if [ $? -ne 0 ]; then
    echo "Comparison with $reference_file failed. Exiting."
    exit 1
  fi

done