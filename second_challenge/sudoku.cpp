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

#include <stdio.h>
#include <stdlib.h>

#include <omp.h>

#include "SudokuBoard.h"

int found_sudokus = 0;
std::vector<CSudokuBoard> solutions;

void solve_NoCO(CSudokuBoard *sudoku);
void solveCO_Depth(CSudokuBoard *sudoku, int depth);
void solveCO_Depth_Serial(CSudokuBoard *sudoku, int depth);
void solveCO_Depth_StrictSerial(CSudokuBoard *sudoku, int depth);
void solveCO_StrictDepth_StrictSerial(CSudokuBoard *sudoku, int depth);
void serialSolve(CSudokuBoard *sudoku);

int main(int argc, char* argv[]) {
    // measure the time
    double t3, t4;

    // expect three command line arguments: field size, block size, and input file
    if (argc != 4) {
        std::cout << "Usage: sudoku.exe <field size> <block size> <input filename>" << std::endl;
        std::cout << std::endl;
        return -1;
    }
    else {
        auto *sudokuIn = new CSudokuBoard(atoi(argv[1]), atoi(argv[2]));
        if (!sudokuIn->loadFromFile(argv[3])) {
            std::cout << "There was an error reading a Sudoku template from " << argv[3] << std::endl;
            std::cout << std::endl;
            return -1;
        }

        // print the Sudoku board template
//        std::cout << "Given Sudoku template" << std::endl;
//        sudokuIn->printBoard();

        // solve the Sudoku by finding (and printing) all solutions
        t3 = omp_get_wtime();

        #pragma omp parallel
        {
            #pragma omp single
            {
                solveCO_Depth_StrictSerial(sudokuIn, 0);
            }
        }

        t4 = omp_get_wtime();

        delete sudokuIn;
    }

    // print the time
    std::cout << "Parallel computation took " << t4 - t3 << " seconds ("
              << omp_get_max_threads() << " threads)." << std::endl;

    //print the solutions
    std::cout << "#solutions: " << found_sudokus << std::endl << std::endl;
//    for (CSudokuBoard s: solutions){
//        s.printBoard();
//    }

    return 0;
}

void solve_NoCO(CSudokuBoard *sudoku) {
    int row = 0, col = 0;

    if (!sudoku->findEmptyCell(&row, &col)){
        #pragma omp critical
        {
            found_sudokus++;
            solutions.push_back(*sudoku);
        }
        return;
    }

    for (int value = 1; value <= sudoku->getFieldSize(); ++value) {
        if (sudoku->isValidMove(row, col, value)) {
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
            found_sudokus++;
            solutions.push_back(*sudoku);
        }
        return;
    }

    for (int value = 1; value <= sudoku->getFieldSize(); ++value) {
        if (sudoku->isValidMove(row, col, value)) {
            #pragma omp task if(depth % sudoku->getBlockSize() == 0 && depth <= sudoku->getFieldSize() * sudoku->getBlockSize())
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
    if (depth > sudoku->getFieldSize() * (sudoku->getFieldSize() - sudoku->getBlockSize())){
        serialSolve(sudoku);
        return;
    }
    if (!sudoku->findEmptyCell(&row, &col)){
        // never reached practically
        found_sudokus++;
        solutions.push_back(*sudoku);
        return;
    }

    for (int value = 1; value <= sudoku->getFieldSize(); ++value) {
        if (sudoku->isValidMove(row, col, value)) {
            #pragma omp task if(depth % sudoku->getBlockSize() == 0)
            {
                auto *newBoard = new CSudokuBoard(*sudoku);
                newBoard->set(row, col, value);
                solveCO_Depth_Serial(newBoard, depth + 1);
            }
        }
    }
}
void solveCO_Depth_StrictSerial(CSudokuBoard *sudoku, int depth) {
    int row = 0, col = 0;
    if (depth > sudoku->getFieldSize() * sudoku->getBlockSize()){
        serialSolve(sudoku);
        return;
    }
    if (!sudoku->findEmptyCell(&row, &col)){
        // for safety, never reached practically
        found_sudokus++;
        solutions.push_back(*sudoku);
        return;
    }

    for (int value = 1; value <= sudoku->getFieldSize(); ++value) {
        if (sudoku->isValidMove(row, col, value)) {
            #pragma omp task if(depth % sudoku->getBlockSize() == 0)
            {
                auto *newBoard = new CSudokuBoard(*sudoku);
                newBoard->set(row, col, value);
                solveCO_Depth_StrictSerial(newBoard, depth + 1);
            }
        }
    }
}
void solveCO_StrictDepth_StrictSerial(CSudokuBoard *sudoku, int depth) {
    int row = 0, col = 0;
    if (depth > sudoku->getFieldSize() * sudoku->getFieldSize()){
        serialSolve(sudoku);
        return;
    }
    if (!sudoku->findEmptyCell(&row, &col)){
        // never reached practically
        found_sudokus++;
        solutions.push_back(*sudoku);
        return;
    }

    for (int value = 1; value <= sudoku->getFieldSize(); ++value) {
        if (sudoku->isValidMove(row, col, value)) {
            #pragma omp task if(depth % sudoku->getFieldSize() == 0)
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
        found_sudokus++;
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