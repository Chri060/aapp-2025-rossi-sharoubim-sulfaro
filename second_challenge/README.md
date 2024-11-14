# Sudoku solver
Implement a Sudoku solver that explores all possible board positions in parallel using OpenMP, efficiently utilizing multithreading and parallel tasks.

## Requirements
The requirements are as follows:
- **Use OpenMP tasks**: utilize OpenMP tasks to manage the solving process across different positions, with tasks dynamically created for each cell as potential values are tested.
- **Synchronization**: implement synchronization mechanisms to ensure thread safety when accessing and updating shared resources, such as the Sudoku board state.
- **Parallel region with a single creator, multiple executors**: structure the parallel region with one main thread that spawns multiple executor threads to work on different parts of the board.
- **Cut-off mechanism**: add a mechanism to limit task creation, reducing task overhead when reaching certain depths in the search or when a solution is deemed improbable.
- **Test different configurations**: experiment with various OpenMP configurations (such as number of threads, task granularity, and cut-off thresholds) to optimize the solver's performance.