#pragma once
#include <utility>

namespace Solver
{
	struct Cell
	{
		int X;
		int Y;
	};

	struct CellGuess : Cell
	{
		CellGuess(int x, int y, int v) : Cell{ x, y }, V(v) {}
		int V;
	};

	class Board
	{
	public:
		static Board GetBoard(int board);
		Board();
		Board(Board const& other);

		void PrintBoard() const;
		bool SolveKnown();
		bool IsSolved() const;
		bool IsValid() const;
		bool SetInitialData();

		CellGuess MakeGuess();
		void ClearGuess(CellGuess const& guess);

	private:
		void SetCell(int x, int y, int v);
		void InitCell(int x, int y, int v);
		unsigned short GetCellMask(int x, int y) const;
		Cell FindEmptyCell();

		unsigned short GetRowMask(int y) const;
		unsigned short GetColMask(int x) const;
		unsigned short GetBoxMask(int bx, int by) const;

		char m_board[9][9];
		unsigned short m_rowMask[9];
		unsigned short m_colMask[9];
		unsigned short m_boxMask[3][3];
		unsigned short m_cellMask[9][9];
		char m_lastEmptyRow;
	};

	void SolveBoard(Board const& board);
}