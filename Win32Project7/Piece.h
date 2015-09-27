#pragma once

#include <functional>
#include "Globals.h"

class Piece
{
	friend class Board;
private:
	int i, j; //true Number(Logical)
	float iLoc, jLoc; //current Location(Pxls)

	HBRUSH myBrush;
	COLORREF myColor;
	HRGN myRgn;
	HRGN myClipRgn;
	struct _bump
	{
		short int l : 4, u : 4, r : 4, d : 4;
	} bump;
	static std::function<signed short int()> getBump;
	static std::function<COLORREF()> getColor;

public:
	void getRgn();
	void Print(HDC hdc);
	void MoveDelta(int dx, int dy);
	void updateRgn();
public:
	Piece(int i, int j);
	~Piece();


};