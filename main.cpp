#include <stdio.h>
#include <chrono>
#include "Board.h"

int main(int argc, char* argv[])
{
    auto boardNum = 0;
    if (argc > 1)
    {
        boardNum = atoi(argv[argc - 1]);
    }

    auto board = Solver::Board::GetBoard(boardNum);

    const auto begin = std::chrono::high_resolution_clock::now();
    Solver::SolveBoard(board);
    const auto end = std::chrono::high_resolution_clock::now();

    printf("Completed in %gs\n", std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / 1e3);
    return 0;
}
