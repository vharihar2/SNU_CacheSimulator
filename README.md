This project implements a multi-level cache and main memory simulator in C++, designed to model realistic cache behavior including hits, misses, writebacks, LRU replacement, and write policies.
Developed as part of the B.Tech. Project under the supervision of [Prof. Venkatnarayan Hariharan](https://snu.edu.in/faculty/venkatnarayan-hariharan/) at Shiv Nadar University.

**The simulator supports:**

1. Configurable L1 and L2 cache sizes, associativity, and block sizes

2. LRU replacement policy

3. Read and write operations with detailed access tracking

4. Verbose mode (-v) for tracing each memory operation

5. Statistics on cache hits/misses, writebacks, and main memory accesses

**Project Structure**

├── src/
│   ├── Cache.cpp
│   ├── Cache.h
│   ├── MainMemory.cpp
│   ├── MainMemory.h
│   ├── Block.cpp
│   ├── Block.h
│   ├── main.cpp
│   └── Makefile
│
├── config.txt         # Cache configuration file
├── memops.txt         # Memory operation trace file
└── README.md

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
L1_SIZE = 1024
L1_ASSOC = 2
L1_BLOCK_SIZE = 16
L2_SIZE = 4096
L2_ASSOC = 4
L2_BLOCK_SIZE = 16
REPLACEMENT_POLICY = LRU

memops.txt
R 0x1000
W 0x1000 = 55
R 0x1080
R 0x1000

Output (example):
L1: hits=2, misses=2, writebacks=0
L2: hits=0, misses=2, writebacks=0
MainMemory: reads=2, writes=0




* Copyright (c) 2025, Shiv Nadar University, Delhi NCR, India. All Rights

* Reserved. Permission to use, copy, modify and distribute this software for

* educational, research, and not-for-profit purposes, without fee and without a

* signed license agreement, is hereby granted, provided that this paragraph and

* the following two paragraphs appear in all copies, modifications, and

* distributions.

*

* IN NO EVENT SHALL SHIV NADAR UNIVERSITY BE LIABLE TO ANY PARTY FOR DIRECT,

* INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST

* PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE.

*

* SHIV NADAR UNIVERSITY SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT

* NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A

* PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS PROVIDED "AS IS". SHIV

* NADAR UNIVERSITY HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,

* ENHANCEMENTS, OR MODIFICATIONS.


Revision History:

Date          By                     Change Notes
13 Nov 2025   Avi Mathur             Completed Cache class implementation

