#import matplotlib.pyplot as plt
#import numpy as np
#
## Given execution times (seconds) for different thread counts
#threads = [1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024]
#
## Execution times for different problem sizes (example: 5000 and 10000 particles)
#openmp_times_5000 = [1.665, 1.2717, 0.8089, 0.5708, 0.5699, 0.5835, 0.7287, 1.0338, 1.6575, 2.972, 5.7911]
#openmp_times_10000 = [3.221, 2.110, 1.205, 0.789, 0.688, 0.722, 0.880, 1.275, 2.198, 3.985, 7.891]
#
#pthreads_times_5000 = [1.6004, 0.8669, 0.4874, 0.3683, 0.3624, 0.3929, 0.4702, 0.6618, 1.2007, 2.5263, 9.1562]
#pthreads_times_10000 = [3.110, 1.789, 0.955, 0.640, 0.599, 0.631, 0.745, 1.020, 1.845, 3.510, 8.250]
#
## Define sequential times for speedup calculations
#sequential_time_5000 = openmp_times_5000[0]  # Assuming 1-thread time as baseline
#sequential_time_10000 = openmp_times_10000[0]
#
## Compute speedup
#openmp_speedup_5000 = [sequential_time_5000 / t for t in openmp_times_5000]
#openmp_speedup_10000 = [sequential_time_10000 / t for t in openmp_times_10000]
#
#pthreads_speedup_5000 = [sequential_time_5000 / t for t in pthreads_times_5000]
#pthreads_speedup_10000 = [sequential_time_10000 / t for t in pthreads_times_10000]
#
## Create plots for 5000 and 10000 particles
#fig, axs = plt.subplots(2, 1, figsize=(10, 12))
#
## Plot for 5000 particles
#axs[0].plot(threads, openmp_speedup_5000, marker='o', linestyle='-', color='black', label='OpenMP (5000 Particles)')
#axs[0].plot(threads, pthreads_speedup_5000, marker='s', linestyle='-', color='gray', label='Pthreads (5000 Particles)')
#axs[0].set_xlabel('Number of Threads')
#axs[0].set_ylabel('Speedup')
#axs[0].set_title('Speedup Comparison for 5000 Particles')
#axs[0].set_xscale('log', base=2)  # Log scale for better visualization
#axs[0].legend()
#axs[0].grid(True, which='both', linestyle='--', linewidth=0.5)
#
## Plot for 10000 particles
#axs[1].plot(threads, openmp_speedup_10000, marker='o', linestyle='-', color='black', label='OpenMP (10000 Particles)')
#axs[1].plot(threads, pthreads_speedup_10000, marker='s', linestyle='-', color='gray', label='Pthreads (10000 Particles)')
#axs[1].set_xlabel('Number of Threads')
#axs[1].set_ylabel('Speedup')
#axs[1].set_title('Speedup Comparison for 10000 Particles')
#axs[1].set_xscale('log', base=2)  # Log scale for better visualization
#axs[1].legend()
#axs[1].grid(True, which='both', linestyle='--', linewidth=0.5)
#
#plt.tight_layout()
#plt.show()
#

import matplotlib.pyplot as plt
import numpy as np

# OpenMP data
threads = [1, 2, 4, 8, 16, 32, 64, 128, 256, 512]
time_5000 = [1.652005050, 1.281856227, 0.831154986, 0.592588957, 0.581400527, 0.600181186, 0.744374684, 1.034412646, 1.607100007, 2.793282620]
time_10000 = [6.934415876, 5.167852571, 3.360851226, 2.132520015, 1.802162514, 1.996085969, 1.910280053, 2.284491114, 3.169263331, 5.274252641]

# Compute speedup
speedup_5000 = [time_5000[0] / t for t in time_5000]
speedup_10000 = [time_10000[0] / t for t in time_10000]

# Ideal speedup
ideal_speedup = threads

plt.figure(figsize=(8, 6))
plt.plot(threads, speedup_5000, marker='o', linestyle='-', label='5000 Particles')
plt.plot(threads, speedup_10000, marker='s', linestyle='-', label='10000 Particles')
plt.plot(threads, ideal_speedup, linestyle='--', color='gray', label='Ideal Speedup')
plt.ylim(0, 10)  # Limit y-axis

plt.xscale("log", base=2)
plt.xticks(threads, threads, rotation=45)
plt.xlabel("Number of Threads")
plt.ylabel("Speedup")
plt.title("OpenMP Speedup")
plt.legend()
plt.grid(True, which="both", linestyle="--", linewidth=0.5)
plt.show()
