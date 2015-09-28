#include "Piece.h"
#include "BaseImage.h"
#include <random>
#include <ctime>
#define threshold (min((tile / 8).magSQ(),1))

extern BaseImage Image;

std::function<signed short int()> Piece::getBump = std::bind(std::uniform_int_distribution<signed short int>(-3, 3), std::default_random_engine((unsigned int)time(0)));
std::function<COLORREF()> Piece::getColor = []() {
	static std::function<unsigned char()> oneColor = std::bind(std::uniform_int_distribution<unsigned int>(0, 255), std::default_random_engine((unsigned int)time(0)));
	return RGB(oneColor(), oneColor(), oneColor());
};


Piece::Piece(int i, int j) :logPt{ i,j }
{
	
	Locs.push_back({ i,j });
	myColor = getColor();
	myBrush = CreateSolidBrush(myColor);// CreateHatchBrush((i + j) % 6, myColor);
	myRgn = nullptr;
	myClipRgn = nullptr;
	pHeight = 0;
	pWidth = 0;
}

Piece::~Piece()
{
	DeleteObject(myBrush);
	if (myRgn)
		DeleteObject(myRgn);
	if (myClipRgn)
		DeleteObject(myClipRgn);
}

void Piece::MoveDelta(int dx, int dy)
{

	Piece::ij_POINT wnd{wndHeight, wndWidth};
	Piece::ij_POINT img{imgHeight ,imgWidth};

	Piece::ij_POINT rv = physPt*wnd / img;

	OffsetRgn(myClipRgn, -rv.j, -rv.i);
	physPt += ij_POINT{ dy,dx }*img / wnd;

	Piece::ij_POINT fw = physPt*wnd / img;
	OffsetRgn(myClipRgn, fw.j,fw.i);
}

void Piece::Print(HDC hdc)
{
	if (Image.ready)
	{
		Piece::ij_POINT wnd{ wndHeight, wndWidth };
		Piece::ij_POINT img{ imgHeight ,imgWidth };
		Piece::ij_POINT tile{ tileHeight, tileWidth };
		Piece::ij_POINT tSmall{ thSmall, twSmall };
		Piece::ij_POINT pc = Piece::ij_POINT{ pHeight,pWidth }*wnd / img;


		Piece::ij_POINT loc = (physPt - tSmall/2)*wnd / img;
		Piece::ij_POINT bmpLoc = logPt*tile - tSmall;
		HDC memDC = CreateCompatibleDC(hdc);
		HBITMAP OldBit = (HBITMAP)SelectObject(memDC, Image.memBit);
		SelectClipRgn(hdc, myClipRgn);

		StretchBlt(hdc, loc.j, loc.i, pc.j, pc.i, memDC,
			bmpLoc.j,bmpLoc.i,pWidth,pHeight, SRCCOPY);

		FrameRgn(hdc, myClipRgn, (HBRUSH)GetStockObject(BLACK_BRUSH),2, 2);
		SelectObject(memDC, OldBit);
		DeleteDC(memDC);
	}
}

void Piece::getRgn()
{
	Piece::ij_POINT tile{ tileHeight, tileWidth };
	physPt = logPt*tile;

	myRgn = CreateRectRgn(0, 0, tile.j, tile.i);
	HRGN hRGNSmall = CreateRectRgn(0, 0, twSmall,thSmall);
	OffsetRgn(hRGNSmall, (tile.j - twSmall) / 2, -thSmall / 2);

	if (bump.u)
		CombineRgn(myRgn, myRgn, hRGNSmall, bump.u > 0 ? RGN_OR : RGN_DIFF);
	OffsetRgn(hRGNSmall, -tile.j / 2, tile.i / 2);
	if (bump.l)
		CombineRgn(myRgn, myRgn, hRGNSmall, bump.l > 0 ? RGN_OR : RGN_DIFF);
	OffsetRgn(hRGNSmall, tile.j / 2, tile.i / 2);
	if (bump.d)
		CombineRgn(myRgn, myRgn, hRGNSmall, bump.d > 0 ? RGN_OR : RGN_DIFF);
	OffsetRgn(hRGNSmall, tile.j / 2, -tile.i / 2);
	if (bump.r)
		CombineRgn(myRgn, myRgn, hRGNSmall, bump.r > 0 ? RGN_OR : RGN_DIFF);

	pWidth = tile.j + twSmall;
	pHeight = tile.i + thSmall;


	//OffsetRgn(myRgn, jLoc, iLoc);
	DeleteObject(hRGNSmall);
#ifdef RgnInCenter
	myRgn = CreateRectRgn(0, 0, tileWidth, tileHeight);
	HRGN hRGNSmall = CreateRectRgn(0, 0, twSmall, thSmall);
	OffsetRgn(myRgn, -tilewidth / 2, -tileHeight / 2);
	OffsetRgn(hRgnSmall, -twSmall / , -thSmall / 2);
	OffsetRgn(hRgnSmall, tilWidth / 2, 0);
	if (bump.u)
		CombineRgn(myRgn, myRgn, hRGNSmall, bump.u > 0 ? RGN_OR : RGN_DIFF);
	OffsetRgn(hRGNSmall, -tileWidth / 2, tileHeight / 2);
	if (bump.l)
		CombineRgn(myRgn, myRgn, hRGNSmall, bump.l > 0 ? RGN_OR : RGN_DIFF);
	OffsetRgn(hRGNSmall, tileWidth / 2, tileHeight / 2);
	if (bump.d)
		CombineRgn(myRgn, myRgn, hRGNSmall, bump.d > 0 ? RGN_OR : RGN_DIFF);
	OffsetRgn(hRGNSmall, tileWidth / 2, -tileHeight / 2);
	if (bump.r)
		CombineRgn(myRgn, myRgn, hRGNSmall, bump.r > 0 ? RGN_OR : RGN_DIFF);
	//OffsetRgn(myRgn, jLoc, iLoc);
	DeleteObject(hRGNSmall);
	//now rgn set to origin

#endif
}

void Piece::updateRgn()
{
	XFORM XF = {};
	XF.eM11 = (float)wndWidth / imgWidth;
	XF.eM22 = (float)wndHeight / imgHeight;

	//OffsetRgn(myRgn, jLoc, iLoc);
	int regionSize = GetRegionData(myRgn, sizeof(RGNDATA), NULL);
	void* rawData = malloc(regionSize);
	LPRGNDATA RD = (LPRGNDATA)rawData;
	GetRegionData(myRgn, regionSize, RD);
	DeleteObject(myClipRgn);
	myClipRgn = ExtCreateRegion(&XF, regionSize, RD);
	pWidth = RD->rdh.rcBound.right - RD->rdh.rcBound.left + twSmall;
	pHeight = RD->rdh.rcBound.bottom - RD->rdh.rcBound.top + thSmall;
	free(rawData);
	OffsetRgn(myClipRgn, physPt.j*wndWidth / imgWidth, physPt.i*wndHeight / imgHeight);
}

bool Piece::getBindSite(Piece* other, ij_POINT Ref, ij_POINT RefLoc, ij_POINT& site)
{
	if (this != other)
	{
		ij_POINT tile{ tileHeight,tileWidth };
		if (((other->physPt - RefLoc) - (other->logPt - Ref)*tile).magSQ() < threshold)
		{
			for (auto p : Locs)
			{
				for (auto q : other->Locs)
				{
					if ((p - q).magSQ() == 1) //quick check for neighborhood.
					{
						site = other->logPt - logPt;
						return true;
					}
				}
			}
		}	
	}
	return false;
}

bool Piece::Mate(Piece* other, ij_POINT site)
{
	if (site.i < 0)
		OffsetRgn(myRgn, 0, tileHeight * abs(site.i));
	else if (site.i > 0)
		OffsetRgn(other->myRgn, 0, tileHeight*abs(site.i));

	if (site.j < 0)
		OffsetRgn(myRgn, tileWidth * abs(site.j), 0);
	else if (site.j > 0)
		OffsetRgn(other->myRgn, tileWidth*abs(site.j),0);

	//Locs.splice(Locs.end(), other->Locs);
	physPt.i = min(physPt.i, other->physPt.i);
	physPt.j = min(physPt.j, other->physPt.j);
	logPt.i = min(logPt.i, other->logPt.i);
	logPt.j = min(logPt.j, other->logPt.j);

	CombineRgn(myRgn, other->myRgn, myRgn, RGN_OR);
	//updateRgn();
	return true;
}