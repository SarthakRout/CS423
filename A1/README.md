# A1

1. Unzip the traces file in the folde where the source code is located
    
    ```
    Struture:
    A1
    |--README.md
    |--analyse.cpp
    |--cache.cpp
    |--cache.h
    |--traces
            |--bzip2.log_l1misstrace_0
            |--..
    ```

2. Compile the executable
```bash
g++ -o analyse analyse.cpp cache.cpp cache.h 
```
3. To run the executable for problem 1, pass either 'i' for Inclusive Policy, 'n' for Not-Inclusive-Not-Exclusive Policy, 'e' for Exclusive Policy to the executable. It will use this policy and process all trace files.
```bash
./analyse 'i'
```
4. It outputs the Hits and Misses for both layers of cache for each of the six applications.
```
Application to analyse traces
Choosing Inclusive Policy

Analysing trace for bzip2
Count: 10657625
Printing Statistics
Layer 2 Hits and Misses:
5259459(49%)	5398166(51%)
Layer 3 Hits and Misses:
3951778(73%)	1446388(27%)

Analysing trace for gcc
Count: 14610809
Printing Statistics
Layer 2 Hits and Misses:
11574348(79%)	3036461(21%)
Layer 3 Hits and Misses:
1663059(54%)	1373402(46%)

Analysing trace for gromacs
Count: 3431511
Printing Statistics
Layer 2 Hits and Misses:
3094660(90%)	336851(10%)
Layer 3 Hits and Misses:
166320(49%)	170531(51%)

Analysing trace for h264ref
Count: 2348572
Printing Statistics
Layer 2 Hits and Misses:
1378894(58%)	969678(42%)
Layer 3 Hits and Misses:
627532(64%)	342146(36%)

Analysing trace for hmmer
Count: 3509764
Printing Statistics
Layer 2 Hits and Misses:
1766343(50%)	1743421(50%)
Layer 3 Hits and Misses:
1352195(77%)	391226(23%)

Analysing trace for sphinx3
Count: 10753445
Printing Statistics
Layer 2 Hits and Misses:
1933096(17%)	8820349(83%)
Layer 3 Hits and Misses:
612987(6%)	8207362(94%)

```

## Problem 2

1. You can run the executable with arguments '2' to show results for problem 2
```bash
./analyse 2
```

2. Output
```
```