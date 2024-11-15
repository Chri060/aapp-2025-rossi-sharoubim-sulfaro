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

#include "SudokuBoard.h"

#include <cstring>

CSudokuBoard::CSudokuBoard(int fsize, int bsize)
        : field_size(fsize), block_size(bsize), solutions(-1)
{
    field = new int[field_size*field_size];
}


CSudokuBoard::CSudokuBoard(const CSudokuBoard& other)
        : field_size(other.getFieldSize()), block_size(other.getBlockSize()), solutions(other.getNumSolutions())
{
    field = new int[field_size*field_size];
    std::memcpy(field, other.field, sizeof(int) * field_size*field_size);
}


CSudokuBoard::~CSudokuBoard(void)
{
    delete[] field;
}


bool CSudokuBoard::loadFromFile(char *filename)
{
    std::ifstream ifile(filename);

    if (!ifile) {
        std::cout << "There was an error opening the input file " << filename << std::endl;
        std::cout << std::endl;
        return false;
    }

    for (int i = 0; i < this->field_size; i++) {
        for (int j = 0; j < this->field_size; j++) {
            ifile >> this->field[ACCESS(i,j)];
        }
    }

    return true;
}


void CSudokuBoard::printBoard()
{
    for(int i = 0; i < field_size; i++) {
        for(int j = 0; j < field_size; j++) {
            std::cout << std::setw(3) <<
                      this->field[ACCESS(i,j)]
                      << " ";
        }
        std::cout << std::endl;
    }
}

bool CSudokuBoard::findEmptyCell(int& row, int& col) const {
    int size = getFieldSize();
    for (int r = 0; r < size; ++r) {
        for (int c = 0; c < size; ++c) {
            if (get(r,c) == 0) {
                row = r;
                col = c;
                return true;               // Found an empty cell
            }
        }
    }
    return false;                          // No empty cell found
}

bool CSudokuBoard::isValidMove(int row, int col, int value) const {
    int sizeF = getFieldSize();
    int sizeB = getBlockSize();

    // Check if the value is already in the same row
    for (int c = 0; c < sizeF; ++c) {
        if (get(row,c) == value) {
            return false;
        }
    }

    // Check if the value is already in the same column
    for (int r = 0; r < sizeF; ++r) {
        if (get(r, col) == value) {
            return false;
        }
    }

    // Check if the value is in the same 3x3 subgrid
    int subGridRowStart = (row / sizeB) * sizeB;
    int subGridColStart = (col / sizeB) * sizeB;

    for (int r = subGridRowStart; r < subGridRowStart + sizeB; ++r) {
        for (int c = subGridColStart; c < subGridColStart + sizeB; ++c) {
            if (get(r,c) == value) {
                return false;
            }
        }
    }

    // If all checks pass, the move is valid
    return true;
}

bool CSudokuBoard::isSolved() {
    // Check if all cells are filled and the board is valid
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            int value = get(row,col);

            // An empty cell indicates the board isn't solved
            if (value == 0) {
                return false;
            }

            // Temporarily clear the current cell for validation
            set(row,col,0);

            // Check if placing the value back is valid
            if (!isValidMove(row, col, value)) {
                set(row,col,value); // Restore the value
                return false;
            }

            // Restore the value after validation
            set(row,col, value);
        }
    }
    return true;
}
