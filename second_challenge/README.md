# Sudoku Solver

This project implements a parallel Sudoku solver using OpenMP, designed to efficiently explore all possible board positions through multithreading and task parallelism. The solver dynamically creates tasks for each cell as potential values are tested, allowing multiple parts of the board to be processed concurrently. Thread safety is ensured through synchronization mechanisms when accessing or updating the shared board state. The parallel region is structured with a single main thread that spawns multiple executor threads, and a cut-off mechanism limits task creation at deeper search levels or when a solution is unlikely, reducing overhead and improving efficiency. Various OpenMP configurations, including the number of threads, task granularity, and cut-off thresholds, can be tuned to optimize the solver’s performance.

## Installation

This project is implemented in C++ and requires a compiler with OpenMP support. 
To set it up locally, clone the repository:
```bash
git clone https://github.com/Chri060/aapp-2025-rossi-sharoubim-sulfaro
cd your-repo
```
Make sure you have a C++ compiler installed.

The project uses a Makefile. To build the solver, run:
```bash
make
```

## Usage

Run the application with: 
```bash
./bin/sudoku
```