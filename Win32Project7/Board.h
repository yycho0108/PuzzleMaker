#pragma once
#include "Globals.h"
#include "Piece.h"
#include <vector>
#include <list>


class Board
{
	std::vector<std::vector<Piece*>> pzl;
	std::list<Piece*> pzl_list;
	Piece* drag;
public:
	Board();
	~Board();
	void Print(HDC hdc);
	void updateRgn();
	bool getPiece(int x, int y);
	bool getPiece(POINT pt);
	void movePiece(int dx, int dy);
	bool releasePiece();
};