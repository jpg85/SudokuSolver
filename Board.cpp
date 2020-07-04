#include <stdio.h>
#include <cmath>
#include <algorithm>
#include <intrin.h>
#include <deque>
#include <stack>

#include "Board.h"
#include "ConsoleHelper.h"

namespace Solver
{
    unsigned short GenMask(int v)
    {
        //Bit position is shifted by one since 0 isn't counted
        return (~(unsigned short)(1u << (v - 1))) & 0x1ff;
    }

    //Initialize everything to ones except for board, which is initialized empty
    Board::Board()
        : m_lastEmptyRow(0)
    {
        std::memset(m_board, 0, sizeof(m_board));
        std::fill(std::begin(m_rowMask), std::end(m_rowMask), 0x1ff);
        std::fill(std::begin(m_colMask), std::end(m_colMask), 0x1ff);
        std::memset(m_boxMask, 0xff, sizeof(m_boxMask));
        std::memset(m_cellMask, 0xff, sizeof(m_cellMask));
    }

    //Copy is an exact memory copy
    Board::Board(Board const& other)
        : m_lastEmptyRow(other.m_lastEmptyRow)
    {
        std::memcpy(m_board, other.m_board, sizeof(m_board));
        std::memcpy(m_rowMask, other.m_rowMask, sizeof(m_rowMask));
        std::memcpy(m_colMask, other.m_colMask, sizeof(m_colMask));
        std::memcpy(m_boxMask, other.m_boxMask, sizeof(m_boxMask));
        std::memcpy(m_cellMask, other.m_cellMask, sizeof(m_cellMask));
    }

    //Check that no empty spots remain on the board
    bool Board::IsSolved() const
    {
        for (auto x = 0; x < 9; ++x)
        {
            if (std::count(m_board[x], m_board[x] + 9, 0) != 0) { return false; }
        }
        return true;
    }

    //This is a weak check. If the original board contains an error, this won't detect it
    //It assumes that if a cell is filled it is valid.
    //Perhaps a deeper check is warranted at initialization to make sure board is initially valid.
    bool Board::IsValid() const
    {
        for (auto x = 0; x < 9; ++x)
        {
            for (auto y = 0; y < 9; ++y)
            {
                if (m_board[x][y] == 0 && GetCellMask(x, y) == 0) { return false; }
            }
        }

        return true;
    }

    //Use the ConsoleHelper to read in the board, either from pre-defined strings or from the console
    Board Board::GetBoard(int board)
    {
        auto b = Board();
        PrintEmptyBoard();

        auto setCursor = [&](int x, int y, int v) { b.SetCell(x, y, v); };
        ReadBoard(setCursor, board);

        return b;
    }

    //Initialize all the masks on the board
    void Board::SetInitialData()
    {
        std::deque<CellGuess> nonEmptyCells;
        std::deque<Cell> emptyCells;

        for (auto y = 0; y < 9; ++y)
        {
            for (auto x = 0; x < 9; ++x)
            {
                const auto cell = m_board[x][y];
                if (cell == 0) { emptyCells.push_back({ x, y }); }
                else { nonEmptyCells.emplace_back(x,y,cell); }
            }
        }

        for (auto const& cell : nonEmptyCells)
        {
            const auto mask = GenMask(cell.V);
            m_rowMask[cell.Y] &= mask;
            m_colMask[cell.X] &= mask;
            m_boxMask[cell.X / 3][cell.Y / 3] &= mask;
        }

        for (auto const& cell : emptyCells)
        {
            m_cellMask[cell.X][cell.Y] &= m_colMask[cell.X] & m_rowMask[cell.Y] & m_boxMask[cell.X / 3][cell.Y / 3];
        }
        m_lastEmptyRow = emptyCells.front().Y;
    }

    //Sets a cell and updates the mask
    //This is important since masks aren't updated anywhere else
    void Board::SetCell(int x, int y, int v)
    {
        m_board[x][y] = v;
        const auto mask = GenMask(v);

        m_colMask[x] &= mask;
        m_rowMask[y] &= mask;
        m_boxMask[x / 3][y / 3] &= mask;
        m_cellMask[x][y] = 0;

        SetCursor(x, y, v);
    }

    //Apply all masks to figure out what numbers are valid for a cell
    unsigned short Board::GetCellMask(int x, int y) const
    {
        return m_cellMask[x][y] & m_colMask[x] & m_rowMask[y] & m_boxMask[x / 3][y / 3];
    }

    //Continue brute force through each cell seeing if the mask indicates the cell is known
    //Every time a cell is updated, have to restart the process again
    //Also check if a cell needs a number but mask indicates no number can be placed
    bool Board::SolveKnown()
    {
        auto newInfo = true;
        while (newInfo)
        {
            newInfo = false;
            for (auto x = 0; x < 9; ++x)
            {
                for (auto y = 0; y < 9; ++y)
                {
                    if (m_board[x][y] != 0) { continue; }

                    unsigned long cellMask = GetCellMask(x, y);
                    if (cellMask == 0)
                    {
                        return false;
                    }
                    else
                    {
                        unsigned long pos;
                        _BitScanForward(&pos, cellMask);
                        const auto shiftedMask = cellMask >> pos;
                        if (shiftedMask == 1)
                        {
                            newInfo = true;
                            SetCell(x, y, pos + 1);
                        }
                    }
                }
            }
        }

        return true;
    }

    //Find an empty cell on the board
    Cell Board::FindEmptyCell()
    {
        for (auto y = m_lastEmptyRow; y < 9; ++y)
        {
            for (auto x = 0; x < 9; ++x)
            {
                if (m_board[x][y] == 0)
                {
                    m_lastEmptyRow = y;
                    return { x,y };
                }
            }
        }
        throw std::exception("No empty cell found");
    }

    //Find an empty cell and guess one of the valid numbers for that cell
    CellGuess Board::MakeGuess()
    {
        auto cell = FindEmptyCell();
        const auto cellMask = GetCellMask(cell.X, cell.Y);

        unsigned long guess;
        _BitScanForward(&guess, cellMask);
        SetCell(cell.X, cell.Y, guess + 1);
        return CellGuess(cell.X, cell.Y, (int)guess + 1);
    }

    //If a guess doesn't work, clear the bit for that guess in that cell
    void Board::ClearGuess(CellGuess const& guess)
    {
        m_cellMask[guess.X][guess.Y] &= GenMask(guess.V);
        PrintBoard();
    }

    //Print every value in the board
    void Board::PrintBoard() const
    {
        for (auto x = 0; x < 9; ++x)
        {
            for (auto y = 0; y < 9; ++y)
            {
                SetCursor(x, y, m_board[x][y]);
            }
        }
    }

    struct Move
    {
        Board Board;
        CellGuess Guess;
    };

    //SolveBoard avoids recursion since it could go fairly deep
    //Instead, use a stack to keep copies of the board on the heap
    //Copy the board each time a guess is made so we can go back if it is wrong
    //Turned out to be surprisingly simple algorithm:
    // 1. Make a guess
    // 2. Solve all the known cells
    // 3. If it proves invalid, then clear the guess on the previou board
    // 4. If the previous build is now invalid, pop it, and clear its guess on the previous board
    // 5. Otherwise, push board on the stack and loop again if it isn't solved
    void SolveBoard(Board const& board)
    {
        std::stack<Move> boardStack;
        boardStack.push({ board, Solver::CellGuess(0, 0, 0) });
        auto guesses = 0;

        boardStack.top().Board.SetInitialData();
        boardStack.top().Board.SolveKnown();

        while (!boardStack.empty() && !boardStack.top().Board.IsSolved())
        {
            auto tempBoard = boardStack.top();
            auto guess = tempBoard.Board.MakeGuess();
            guesses += 1;

            if (!tempBoard.Board.SolveKnown())
            {
                boardStack.top().Board.ClearGuess(guess);
                while (!boardStack.top().Board.IsValid())
                {
                    guess = boardStack.top().Guess;
                    boardStack.pop();
                    boardStack.top().Board.ClearGuess(guess);
                }
            }
            else
            {
                boardStack.push({ tempBoard.Board, guess });
            }
        }

        if (boardStack.empty())
        {
            SetCursorEnd();
            printf("Failed to solve board after %d guesses\n", guesses);
        }
        else
        {
            boardStack.top().Board.PrintBoard();
            SetCursorEnd();
            printf("Solved board after %d guesses\n", guesses);
        }
    }
}
