#include "Board.h"



Board::Board()
{
	pzl.reserve(brdHeight);

	for (int i = 0; i < brdHeight; ++i)
	{
		pzl.push_back(std::vector<Piece*>());
		pzl[i].reserve(brdWidth);
		for (int j = 0; j < brdWidth; ++j)
		{
			Piece* tmp = new Piece(i, j);
			pzl[i].push_back(tmp);
			pzl_list.push_back(tmp);
		}
	}

	for (int i = 0; i < brdHeight; ++i)
	{
		for (int j = 0; j < brdWidth; ++j)
		{

			pzl[i][j]->bump.u = i ? -pzl[i - 1][j]->bump.d : 0;
			pzl[i][j]->bump.l = j ? -pzl[i][j - 1]->bump.r : 0;
			pzl[i][j]->bump.d = i == brdHeight - 1 ? 0 : Piece::getBump();
			pzl[i][j]->bump.r = j == brdWidth - 1 ? 0 : Piece::getBump();
			pzl[i][j]->getRgn();
		}
	}

	RECT R;
	GetClientRect(hMainWnd, &R);
	wndWidth = R.right - R.left;
	wndHeight = R.bottom - R.top;
	updateRgn();

	drag = nullptr;
}

Board::~Board()
{
	for (auto p : pzl_list)
		delete p;
}

void Board::Print(HDC hdc)
{
	for (auto i = pzl_list.rbegin(); i != pzl_list.rend(); ++i)
	{
		auto p = *i;
		p->Print(hdc);
	}
}

void Board::updateRgn()
{
	for (auto p : pzl_list)
	{
		p->updateRgn();
	}
}

bool Board::getPiece(int x, int y)
{
	drag = nullptr;
	for (auto i = pzl_list.begin(); i != pzl_list.end(); ++i)
	{
		auto p = *i;
		if (PtInRegion(p->myClipRgn, x, y))
		{
			pzl_list.erase(i);
			pzl_list.push_front(p); // first in Z order now.
			drag = p;
			return true;
		}
	}
	return false;
}

bool Board::getPiece(POINT pt)
{
	return getPiece(pt.x, pt.y);
}

void Board::movePiece(int dx, int dy)
{
	if (drag)
	{
		InvalidateRgn(hMainWnd, drag->myClipRgn, TRUE);
		drag->MoveDelta(dx, dy);
		InvalidateRgn(hMainWnd, drag->myClipRgn, FALSE);
	}
}

void Board::releasePiece()
{
	drag = nullptr;
}
bool Board::checkMate()
{
	if (drag)
	{
		Piece::ij_POINT o = drag->logPt;
		Piece::ij_POINT oLoc = drag->physPt;

		std::list<Piece::ij_POINT> locAdd;
		
		for (auto i = pzl_list.begin(); i != pzl_list.end();)
		{
			auto p = *i;
			Piece::ij_POINT site;
			if (drag->getBindSite(p,o,oLoc,site))
			{
				drag->Mate(p, site);
				locAdd.splice(locAdd.end(), p->Locs);
				i = pzl_list.erase(i);
				delete(p);
				continue;
			}
			++i;
		}
		drag->Locs.splice(drag->Locs.end(), locAdd);
		drag->updateRgn();
		InvalidateRgn(hMainWnd, drag->myClipRgn, TRUE);
	}
	
	if (pzl_list.size() == 1)
	{
		SetRectRgn(pzl_list.front()->myRgn, 0, 0, imgWidth, imgHeight);
		SetRectRgn(pzl_list.front()->myClipRgn, 0, 0, wndWidth, wndHeight);
		return true;
	}
	return false;
}

void Board::rotatePiece(bool CLOCK)
{
	if (drag)
		drag->rotate(CLOCK);
}