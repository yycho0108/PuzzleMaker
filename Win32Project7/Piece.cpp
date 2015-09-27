#include "Piece.h"
#include "BaseImage.h"
#include <random>
#include <ctime>

extern BaseImage Image;

std::function<signed short int()> Piece::getBump = std::bind(std::uniform_int_distribution<signed short int>(-3, 3), std::default_random_engine((unsigned int)time(0)));
std::function<COLORREF()> Piece::getColor = []() {
	static std::function<unsigned char()> oneColor = std::bind(std::uniform_int_distribution<unsigned int>(0, 255), std::default_random_engine((unsigned int)time(0)));
	return RGB(oneColor(), oneColor(), oneColor());
};


Piece::Piece(int i, int j) :i(i), j(j)
{
	myColor = getColor();
	myBrush = CreateSolidBrush(myColor);// CreateHatchBrush((i + j) % 6, myColor);
	myRgn = nullptr;
	myClipRgn = nullptr;
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
	OffsetRgn(myClipRgn, -jLoc*wndWidth / imgWidth, -iLoc*wndHeight / imgHeight);
	iLoc += float(dy*imgHeight) / wndHeight;
	jLoc += float(dx*imgWidth)/wndWidth;
	OffsetRgn(myClipRgn, jLoc*wndWidth / imgWidth, iLoc*wndHeight / imgHeight);
}

void Piece::Print(HDC hdc)
{
	int twSmall = tileWidth / 5;
	int thSmall = tileHeight / 5;

	if (Image.ready)
	{
		HDC memDC = CreateCompatibleDC(hdc);
		HBITMAP OldBit = (HBITMAP)SelectObject(memDC, Image.memBit);
		SelectClipRgn(hdc, myClipRgn);
		StretchBlt(hdc, (jLoc - twSmall)*wndWidth / imgWidth, (iLoc - thSmall)*wndHeight / imgHeight,
			(twSmall * 2 + tileWidth)*wndWidth / imgWidth, (thSmall * 2 + tileHeight)*wndHeight/imgHeight,
			memDC, j*tileWidth - twSmall, i*tileHeight - thSmall, (twSmall * 2 + tileWidth), (thSmall * 2 + tileHeight), SRCCOPY);
		FrameRgn(hdc, myClipRgn, (HBRUSH)GetStockObject(BLACK_BRUSH),2, 2);
		SelectObject(memDC, OldBit);
		DeleteDC(memDC);
	}
}

void Piece::getRgn()
{
	iLoc = i*tileHeight;
	jLoc = j*tileWidth;
	int twSmall = tileWidth / 5;
	int thSmall = tileHeight / 5;

	myRgn = CreateRectRgn(0, 0, tileWidth, tileHeight);
	HRGN hRGNSmall = CreateRectRgn(0, 0, twSmall,thSmall);
	OffsetRgn(hRGNSmall, (tileWidth - twSmall) / 2, -thSmall / 2);

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

	free(rawData);
	OffsetRgn(myClipRgn, jLoc*wndWidth / imgWidth, iLoc*wndHeight / imgHeight);
}