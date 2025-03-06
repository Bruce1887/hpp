import matplotlib.pyplot as plt
import numpy as np

# OpenMP data
threads = [1, 2, 4, 8, 16, 32, 64, 128, 256, 512]
time_5000_pthreads = [
    1.642149371, 0.935512938, 0.575216446, 0.514215694, 0.435372596, 0.471532616,
    0.650922127, 1.088540444, 1.613133345, 3.081281003
]

time_10000_pthreads = [
    7.251622246, 3.788014937, 2.238831188, 1.586039843, 1.533322360, 1.654662786,
    1.886249949, 2.194617433, 3.013771567, 4.929445641
]
# Compute speedup
speedup_5000 = [time_5000_pthreads[0] / t for t in time_5000_pthreads]
speedup_10000 = [time_10000_pthreads[0] / t for t in time_10000_pthreads]

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
plt.title("Pthreads Speedup")
plt.legend()
plt.grid(True, which="both", linestyle="--", linewidth=0.5)
plt.show()
