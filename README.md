_**Imported on Dec 4th 2025 from my student's Git repo (Avi Mathur and Achint Gupta) based on a BTP they did under me in MO2025: https://github.com/AviMathurCodes/CacheSimulator**_

This project implements a multi-level cache and main memory simulator in C++, designed to model realistic cache behavior including hits, misses, writebacks, LRU replacement, and write policies.
Developed as part of the B.Tech. Project under the supervision of [Prof. Venkatnarayan Hariharan](https://snu.edu.in/faculty/venkatnarayan-hariharan/) at Shiv Nadar University.

**The simulator supports:**

1. Configurable L1 and L2 cache sizes, associativity, and block sizes

2. LRU replacement policy

3. Read and write operations with detailed access tracking

4. Verbose mode (-v) for tracing each memory operation

5. Statistics on cache hits/misses, writebacks, and main memory accesses

**Build Instructions**

Prerequisites
Ensure you have:
g++ or clang++ (C++17 compatible)
Make utility (default on Linux/macOS; for Windows use MinGW or WSL)

1. mingw32-make
2. ./CacheSim config.txt memops.txt -v

-v (optional) → enables verbose tracing of each cache access

**Example Run**

config.txt  
1024  
16  
2  
4096  
16  
4  


memops.txt  
R 0x1000  
W 0x1000 55  
R 0x1080  
R 0x1000  

Output (example):  
L1: hits=2, misses=2, writebacks=0  
L2: hits=0, misses=2, writebacks=0  
MainMemory: reads=2, writes=0
