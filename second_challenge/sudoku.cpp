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

std::vector<CSudokuBoard> solutions;

void solve_NoCO(CSudokuBoard *sudoku);
void solveCO_Depth(CSudokuBoard *sudoku, int depth);
void solveCO_Depth_Serial(CSudokuBoard *sudoku, int depth);
void solveCO_Depth_StrictSerial(CSudokuBoard *sudoku, int depth);
void solveCO_StrictDepth_StrictSerial(CSudokuBoard *sudoku, int depth);
void serialSolve(CSudokuBoard *sudoku);

void printSolutions() {
    std::cout << "#solutions: " << solutions.size() << std::endl;
    for (CSudokuBoard s: solutions){
        s.printBoard();
    }
}
double run(CSudokuBoard *board, const std::function<void(CSudokuBoard*, int)>& func){
    double start = omp_get_wtime(), end;

    #pragma omp parallel
    {
        #pragma omp single
        {
            func(board, 0);
        }
    }
    end = omp_get_wtime();

    //printSolutions();
    solutions.clear();
    return end - start;
}
double run(CSudokuBoard *board, const std::function<void(CSudokuBoard*)>& func){
    double start = omp_get_wtime();

    #pragma omp parallel
    {
        #pragma omp single
        {
            func(board);
        }
    }

    //printSolutions();
    solutions.clear();
    return omp_get_wtime() - start;
}

int main(int argc, char* argv[]) {
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

        std::cout << omp_get_max_threads() << " threads:\n";
        std::cout << "solve_NoCO: " << run(sudokuIn, solve_NoCO) << "s" << std::endl;
        std::cout << "solveCO_Depth: " << run(sudokuIn, solveCO_Depth) << "s" << std::endl;
        std::cout << "solveCO_Depth_Serial: " << run(sudokuIn, solveCO_Depth_Serial) << "s" << std::endl;
        std::cout << "solveCO_Depth_StrictSerial: " << run(sudokuIn, solveCO_Depth_StrictSerial) << "s" << std::endl;
        std::cout << "solveCO_StrictDepth_StrictSerial: " << run(sudokuIn, solveCO_StrictDepth_StrictSerial) << "s" << std::endl;

        delete sudokuIn;
    }

    return 0;
}

void solve_NoCO(CSudokuBoard *sudoku) {
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