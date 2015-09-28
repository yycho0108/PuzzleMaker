#pragma once

#include <functional>
#include "Globals.h"
#include <list>


class Piece
{
	friend class Board;
	enum Dir { Up = -2, Down = 2, Left = -1, Right = 1, None = 0 };

private:
	struct ij_POINT
	{
		float i, j;
		ij_POINT operator+(ij_POINT& other)
		{
			return ij_POINT{ i,j } += other;
		};
		ij_POINT operator-(ij_POINT& other)
		{
			return ij_POINT{ i,j } -= other;
		};
		ij_POINT operator*(ij_POINT& other)
		{
			return ij_POINT{ i,j } *= other;
		};
		ij_POINT operator/(ij_POINT& other)
		{
			return ij_POINT{ i,j } /= other;
		};
		ij_POINT operator*(float a) { return ij_POINT{ i,j } *= a; }
		ij_POINT operator/(float a) { return ij_POINT{ i,j } /= a; }

		ij_POINT& operator+=(ij_POINT& other)
		{
			i += other.i;
			j += other.j;
			return *this;
		};
		ij_POINT& operator-=(ij_POINT& other)
		{
			i -= other.i;
			j -= other.j;
			return *this;
		};
		ij_POINT& operator*=(ij_POINT& other)
		{
			i *= other.i;
			j *= other.j;
			return *this;
		};
		ij_POINT& operator/=(ij_POINT& other)
		{
			i /= other.i;
			j /= other.j;
			return *this;
		};
		ij_POINT& operator*=(float a) { i *= a, j *= a; return *this; }
		ij_POINT& operator/=(float a) { i /= a, j /= a; return *this; }

		ij_POINT(float i = 0, float j = 0) :i(i), j(j) {};
		ij_POINT(int i, int j) :i((float)i), j((float)j) {};
		int magSQ() { return i*i + j*j; }
		double mag() { return sqrt(magSQ()); }
	};
	std::list<ij_POINT> Locs;
	ij_POINT logPt; //true Number(Logical)
	ij_POINT physPt; //current Location(Pxls)
	int pWidth, pHeight;

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
	bool  getBindSite(Piece* other, ij_POINT Ref, ij_POINT RefLoc, ij_POINT &site);
	bool Mate(Piece* other, ij_POINT Site);
public:
	Piece(int i, int j);
	~Piece();


};