#include "ConsoleHelper.h"

#include <cmath>
#include <algorithm>

#define NOMINMAX
#include <Windows.h>

//MSDN's suggestion on how to clear console
void ClearScreen()
{
    auto hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    COORD coordScreen = { 0, 0 };    /* here's where we'll home the cursor */
    BOOL bSuccess;
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi; /* to get buffer info */
    DWORD dwConSize;                 /* number of character cells in the current buffer */

    /* get the number of character cells in the current buffer */
    bSuccess = GetConsoleScreenBufferInfo(hConsole, &csbi);
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

    /* fill the entire screen with blanks */
    bSuccess = FillConsoleOutputCharacter(hConsole, (TCHAR)' ', dwConSize, coordScreen, &cCharsWritten);

    /* get the current text attribute */
    bSuccess = GetConsoleScreenBufferInfo(hConsole, &csbi);

    /* now set the buffer's attributes accordingly */
    bSuccess = FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);

    /* put the cursor at (0, 0) */
    bSuccess = SetConsoleCursorPosition(hConsole, coordScreen);
}

//Print the sudoku board
void PrintHorizontal(bool edge)
{
    if (edge)   printf("|-----------------------|\n");
    else        printf("|-------+-------+-------|\n");
}

void PrintEmptyBoard()
{
    ClearScreen();

    for (auto i = 0; i < 9; ++i)
    {
        if (i % 3 == 0)
        {
            PrintHorizontal(i == 0);
        }
        printf("| . . . | . . . | . . . |\n");
    }
    PrintHorizontal(true);
}

//Determine where a given "x,y" belongs in the console/string
COORD GetCoordinate(int x, int y)
{
    int boxX = x / 3;
    int subX = x % 3;
    SHORT coordX = 2 + boxX * 8 + subX * 2;

    int boxY = y / 3;
    int subY = y % 3;
    SHORT coordY = 1 + boxY * 4 + subY;

    return { coordX, coordY };
}

//Move the cursor to the x/y position
void MoveCursor(int x, int y)
{
    auto hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hConsole, GetCoordinate(x, y));
}

//Set the charactor at the x/y position
void SetCursor(int x, int y, int v)
{
    auto hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    TCHAR val = (v) ? TCHAR('0') + TCHAR(v) : TCHAR('.');
    DWORD size;
    WriteConsoleOutputCharacter(hConsole, &val, 1, GetCoordinate(x, y), &size);
}

//Scan the console input from user to get the sudoku board
void ScanInput(std::function<void(int, int, int)> const& setValue)
{
    MoveCursor(0, 0);

    auto in = GetStdHandle(STD_INPUT_HANDLE);
    auto xPos = 0;
    auto yPos = 0;

    auto moveCursor = [&](int) {
        if (xPos == 9)
        {
            yPos += 1;
            xPos = 0;
        }
        else if (xPos == -1)
        {
            yPos -= 1;
            xPos = 8;
        }
        xPos = std::max(xPos % 9, 0);
        yPos = std::max(yPos % 9, 0);
        MoveCursor(xPos, yPos);
    };

    INPUT_RECORD r;
    DWORD size;
    while (ReadConsoleInput(in, &r, 1, &size) == TRUE)
    {
        if (r.EventType == KEY_EVENT)
        {
            if (r.Event.KeyEvent.bKeyDown)
            {
                switch (r.Event.KeyEvent.wVirtualKeyCode)
                {
                case VK_UP: moveCursor(--yPos); break;
                case VK_DOWN: moveCursor(++yPos); break;
                case VK_LEFT: moveCursor(--xPos); break;
                case VK_RIGHT: moveCursor(++xPos); break;

                case VK_DECIMAL:
                case VK_OEM_PERIOD:
                case VK_NUMPAD0:
                case 0x30: setValue(xPos, yPos, 0); moveCursor(++xPos); break;

                case VK_NUMPAD1:
                case 0x31: setValue(xPos, yPos, 1); moveCursor(++xPos); break;

                case VK_NUMPAD2:
                case 0x32: setValue(xPos, yPos, 2); moveCursor(++xPos); break;

                case VK_NUMPAD3:
                case 0x33: setValue(xPos, yPos, 3); moveCursor(++xPos); break;

                case VK_NUMPAD4:
                case 0x34: setValue(xPos, yPos, 4); moveCursor(++xPos); break;

                case VK_NUMPAD5:
                case 0x35: setValue(xPos, yPos, 5); moveCursor(++xPos); break;

                case VK_NUMPAD6:
                case 0x36: setValue(xPos, yPos, 6); moveCursor(++xPos); break;

                case VK_NUMPAD7:
                case 0x37: setValue(xPos, yPos, 7); moveCursor(++xPos); break;

                case VK_NUMPAD8:
                case 0x38: setValue(xPos, yPos, 8); moveCursor(++xPos); break;

                case VK_NUMPAD9:
                case 0x39: setValue(xPos, yPos, 9); moveCursor(++xPos); break;

                case VK_RETURN: return;

                default: break;
                }
            }
        }
    }
}

//Parse a string array in the same format as the console
void ParseBoard(std::function<void(int, int, int)> const& setValue, const char* board[])
{
    for (auto x = 0; x < 9; ++x)
    {
        for (auto y = 0; y < 9; ++y)
        {
            const auto coord = GetCoordinate(x, y);
            const auto cell = board[coord.Y][coord.X];
            if (cell >= '1' && cell <= '9')
            {
                const auto cellNum = cell - '0';
                setValue(x, y, cellNum);
            }
        }
    }
}

//A few preset boards for easy debugging
static const auto BoardLines = 13;
static const char* board1[BoardLines] = {
    "|-----------------------|",
    "| 8 . 1 | . . . | . . . |",
    "| 2 5 . | . 7 . | . 9 . |",
    "| . 4 . | . . 8 | . 2 6 |",
    "|-------+-------+-------|",
    "| . . 7 | 8 . 5 | . 1 3 |",
    "| . . 5 | . 4 3 | . . 7 |",
    "| . . 3 | 7 9 . | . . 4 |",
    "|-------+-------+-------|",
    "| . 9 . | 4 . 7 | . 6 2 |",
    "| 1 . . | 5 8 6 | . 7 9 |",
    "| . 6 4 | . 1 2 | . . . |",
    "|-----------------------|" };
static const char* board2[BoardLines] = {
    "|-----------------------|",
    "| . . 9 | . . . | . . . |",
    "| . 3 . | 2 . . | . . . |",
    "| . . . | . 8 4 | . 5 1 |",
    "|-------+-------+-------|",
    "| . . . | . 3 2 | 9 . . |",
    "| . . 6 | . . . | . 2 . |",
    "| . 8 . | . . . | 5 7 . |",
    "|-------+-------+-------|",
    "| . . . | . 6 . | 7 3 . |",
    "| 7 . . | 4 . . | . . . |",
    "| 4 . 5 | . . . | . 9 . |",
    "|-----------------------|" };
static const char** boardList[] = { board1, board2 };

//Read the board from built-in or console
void ReadBoard(std::function<void(int, int, int)> const& setValue, int board)
{
    const auto numBoards = _countof(boardList);
    const auto adjustedBoard = board - 1;
    if (adjustedBoard < 0 || adjustedBoard >= numBoards)
    {
        ScanInput(setValue);
    }
    else
    {
        ParseBoard(setValue, boardList[adjustedBoard]);
    }
}

//Move cursor to end of the board so future prints end up at the end
void SetCursorEnd()
{
    auto hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hConsole, COORD{ 0, 13 });
}
