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
