#pragma once
#include <functional>

void SetCursor(int x, int y, int v);
void ReadBoard(std::function<void(int, int, int)> const& setValue, int board);
void PrintEmptyBoard();

void SetCursorEnd();
