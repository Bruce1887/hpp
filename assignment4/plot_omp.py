import matplotlib.pyplot as plt
import numpy as np

# OpenMP data
threads = [1, 2, 4, 8, 16, 32, 64, 128, 256, 512]
time_5000_openmp = [
    1.653035685, 1.302624891, 0.899952501, 0.611060516, 0.564410657, 0.597382380,
    0.746627714, 1.095011497, 1.648206767, 2.911924295
]

time_10000_openmp = [
    6.469786939, 4.996413334, 3.195655542, 2.056073494, 1.882640544, 1.863809813,
    1.883213647, 2.335827308, 3.439413673, 4.812878396
]
# Compute speedup
speedup_5000 = [time_5000_openmp[0] / t for t in time_5000_openmp]
speedup_10000 = [time_10000_openmp[0] / t for t in time_10000_openmp]

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
