# Select i-th

The selection problem consists of identifying the element of a specified rank in a set of n unique numbers. Given an integer i (where 1 ≤ i ≤ n), the objective is to find the element that is larger than exactly i-1 other elements in the set.

This project implements two approaches to solve the problem. The first is a deterministic method using the Median of Medians algorithm, while the second is a randomized approach based on Quicksort partitioning. Both implementations are benchmarked using the Google Benchmark library to measure and display their asymptotic time complexity. A comprehensive suite of tests ensures correctness, and performance comparisons are made using identical test cases to highlight differences between the deterministic and randomized algorithms.

## Installation

This notebook assumes a working C++ kernel is available. 
If using a local environment or a cloud platform like Google Colab with C++ support, you can execute the code cells directly. 
There is no additional setup required beyond having a C++ compiler (like g++) installed.

All tests and benchmarks are included in the notebook, so running all cells will automatically execute the algorithms, validate correctness, and display performance metrics.