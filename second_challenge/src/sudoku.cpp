/*                    
 *  This file is part of Christian's OpenMP parallel Sudoku Solver
 *  
 *  Copyright (C) 2013 by Christian Terboven <christian@terboven.com>
 *                                                                       
 *  This program is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include <iostream>

#include <stdlib.h>
#include <functional>

#include <omp.h>

#include "SudokuBoard.h"

//a vector of all the founded solutions
std::vector<CSudokuBoard> solutions;

/**
 * A bruteforce recursive solution that generate a task for each node of recursion
 * @param sudoku CSudokuBoard that represent the sudoku to complete
 */
void solve_NoCO(CSudokuBoard *sudoku);
/**
 * A bruteforce recursive solution that generate a task for each node of recursion until complete the first 2 rows
 * @param sudoku CSudokuBoard that represent the sudoku to complete
 * @param depth the actual depth of recursion
 */
void solveCO_Depth(CSudokuBoard *sudoku, int depth);
/**
 * A bruteforce recursive solution that generate a task for each node of recursion until complete the first 2 rows;
 * after start with a serial approach
 * @param sudoku CSudokuBoard that represent the sudoku to complete
 * @param depth the actual depth of recursion
 */
void solveCO_Depth_Serial(CSudokuBoard *sudoku, int depth);
/**
 * A bruteforce recursive solution that generate a task for each node of recursion until complete the first row;
 * after start with a serial approach
 * @param sudoku CSudokuBoard that represent the sudoku to complete
 * @param depth the actual depth of recursion
 */
void solveCO_StrictDepth_StrictSerial(CSudokuBoard *sudoku, int depth);
/**
 * A bruteforce recursive solution that generate a task for each node of recursion until complete the half of first row;
 * after start with a serial approach
 * @param sudoku CSudokuBoard that represent the sudoku to complete
 * @param depth the actual depth of recursion
 */
void solveCO_VeryStrictDepth_VeryStrictSerial(CSudokuBoard *sudoku, int depth);
/**
 * A bruteforce recursive serial solution with inplace modification
 * @param sudoku CSudokuBoard that represent the sudoku to complete
 */
void serialSolve(CSudokuBoard *sudoku);

/**
 * print the number of solutions and every board
 */
void printSolutions() {
    std::cout << "#solutions: " << solutions.size() << std::endl;
    for (CSudokuBoard s: solutions){
        s.printBoard();
    }
}
/**
 * Run func on the given sudoku, print the solutions and clear
 * @param sudoku CSudokuBoard that represent the sudoku to complete
 * @param func the function to run, that require a board and an integer
 * @return the time of execution
 */
double run(CSudokuBoard *sudoku, const std::function<void(CSudokuBoard*, int)>& func){
    double start, end;

    //measure the current time
    start = omp_get_wtime();

    //indicate that the following code is executed in parallel, splitting the work of the master thread
    #pragma omp parallel
    {
        //put others threads in wait for tasks to complete
        #pragma omp single
        {
            func(sudoku, 0);
        }
    }
    end = omp_get_wtime();

    //printSolutions();
    solutions.clear();
    return end - start;
}
/**
 * Run func on the given sudoku, print the solutions and clear
 * @param sudoku CSudokuBoard that represent the sudoku to complete
 * @param func the function to run, that require a board
 * @return the time of execution
 */
double run(CSudokuBoard *sudoku, const std::function<void(CSudokuBoard*)>& func){
    double start = omp_get_wtime(), end;

    #pragma omp parallel
    {
        #pragma omp single
        {
            func(sudoku);
        }
    }
    end = omp_get_wtime();

    //printSolutions();
    solutions.clear();
    return end - start;
}

int main(int argc, char* argv[]) {
    // expect three command line arguments: field size, block size, and input file
    if (argc != 4) {
        std::cout << "Usage: sudoku.exe <field size> <block size> <input filename>" << std::endl;
        std::cout << std::endl;
        return -1;
    }
    auto *sudokuIn = new CSudokuBoard(atoi(argv[1]), atoi(argv[2]));
    if (!sudokuIn->loadFromFile(argv[3])) {
        std::cout << "There was an error reading a Sudoku template from " << argv[3] << std::endl;
        std::cout << std::endl;
        return -1;
    }

    //run all the possible configurations
    std::cout << omp_get_max_threads() << " threads: " << std::endl;
    std::cout << "\tsolve_NoCO: " << run(sudokuIn, solve_NoCO) << "s" << std::endl;
    std::cout << "\tsolveCO_Depth: " << run(sudokuIn, solveCO_Depth) << "s" << std::endl;
    std::cout << "\tsolveCO_Depth_Serial: " << run(sudokuIn, solveCO_Depth_Serial) << "s" << std::endl;
    std::cout << "\tsolveCO_StrictDepth_StrictSerial: " << run(sudokuIn, solveCO_StrictDepth_StrictSerial) << "s" << std::endl;
    std::cout << "\tsolveCO_VeryStrictDepth_VeryStrictSerial: " << run(sudokuIn, solveCO_VeryStrictDepth_VeryStrictSerial) << "s" << std::endl;

    //clear the memory
    delete sudokuIn;

    return 0;
}

void solve_NoCO(CSudokuBoard *sudoku) {
    int row = 0, col = 0;

    //save the found solution
    if (!sudoku->findEmptyCell(&row, &col)){
        //ensure that each is executed by a single thread at a time
        #pragma omp critical
        {
            solutions.push_back(*sudoku);
        }
        return;
    }

    for (int value = 1; value <= sudoku->getFieldSize(); ++value) {
        if (sudoku->isValidMove(row, col, value)) {
            //generate a new task on the next level of recursion
            #pragma omp task
            {
                auto *newBoard = new CSudokuBoard(*sudoku);
                newBoard->set(row, col, value);
                solve_NoCO(newBoard);
            }
        }
    }
}
void solveCO_Depth(CSudokuBoard *sudoku, int depth) {
    int row = 0, col = 0;

    if (!sudoku->findEmptyCell(&row, &col)){
        #pragma omp critical
        {
            solutions.push_back(*sudoku);
        }
        return;
    }

    for (int value = 1; value <= sudoku->getFieldSize(); ++value) {
        if (sudoku->isValidMove(row, col, value)) {
            //generate tasks until the first 2 rows are completed
            #pragma omp task firstprivate(value,row,col,sudoku) final(depth > sudoku->getFieldSize() * 2)
            {
                auto *newBoard = new CSudokuBoard(*sudoku);
                newBoard->set(row, col, value);
                solveCO_Depth(newBoard, depth + 1);
            }
        }
    }
}
void solveCO_Depth_Serial(CSudokuBoard *sudoku, int depth) {
    int row = 0, col = 0;
    //call the serial implementation after the 2 first rows
    if (depth > sudoku->getFieldSize() * 2){
        serialSolve(sudoku);
        return;
    }
    if (!sudoku->findEmptyCell(&row, &col)){
        // never reached practically
        solutions.push_back(*sudoku);
        return;
    }

    for (int value = 1; value <= sudoku->getFieldSize(); ++value) {
        if (sudoku->isValidMove(row, col, value)) {
            #pragma omp task firstprivate(value,row,col,sudoku) final(depth > sudoku->getFieldSize() * 2)
            {
                auto *newBoard = new CSudokuBoard(*sudoku);
                newBoard->set(row, col, value);
                solveCO_Depth_Serial(newBoard, depth + 1);
            }
        }
    }
}
void solveCO_StrictDepth_StrictSerial(CSudokuBoard *sudoku, int depth) {
    int row = 0, col = 0;
    //call the serial implementation after the first rows
    if (depth > sudoku->getFieldSize()){
        serialSolve(sudoku);
        return;
    }
    if (!sudoku->findEmptyCell(&row, &col)){
        // never reached practically
        solutions.push_back(*sudoku);
        return;
    }

    for (int value = 1; value <= sudoku->getFieldSize(); ++value) {
        if (sudoku->isValidMove(row, col, value)) {
            //generate tasks until the first row is completed
            #pragma omp task firstprivate(value,row,col,sudoku) final(depth > sudoku->getFieldSize())
            {
                auto *newBoard = new CSudokuBoard(*sudoku);
                newBoard->set(row, col, value);
                solveCO_StrictDepth_StrictSerial(newBoard, depth + 1);
            }
        }
    }
}
void solveCO_VeryStrictDepth_VeryStrictSerial(CSudokuBoard *sudoku, int depth) {
    int row = 0, col = 0;
    //call the serial implementation after the first half row
    if (depth > sudoku->getFieldSize()/2){
        serialSolve(sudoku);
        return;
    }
    if (!sudoku->findEmptyCell(&row, &col)){
        // never reached practically
        solutions.push_back(*sudoku);
        return;
    }

    for (int value = 1; value <= sudoku->getFieldSize(); ++value) {
        if (sudoku->isValidMove(row, col, value)) {
            //generate tasks until the first half row is completed
            #pragma omp task firstprivate(value,row,col,sudoku) final(depth > sudoku->getFieldSize()/2)
            {
                auto *newBoard = new CSudokuBoard(*sudoku);
                newBoard->set(row, col, value);
                solveCO_StrictDepth_StrictSerial(newBoard, depth + 1);
            }
        }
    }
}
void serialSolve(CSudokuBoard *sudoku) {
    int row = 0, col = 0;
    if (!sudoku->findEmptyCell(&row, &col)){
        solutions.push_back(*new CSudokuBoard(*sudoku));
        return;
    }

    for (int value = 1; value <= sudoku->getFieldSize(); ++value) {
        if (sudoku->isValidMove(row, col, value)) {
            sudoku->set(row, col, value);
            serialSolve(sudoku);
        }
        sudoku->set(row, col, 0);
    }
}