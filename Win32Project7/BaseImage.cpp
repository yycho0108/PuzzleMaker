#include "BaseImage.h"

BaseImage::BaseImage()
{
	ready = false;
	memBit = nullptr;
}

BaseImage::~BaseImage()
{
	DeleteImage();
}

void BaseImage::DeleteImage()
{
	if (memBit)
		DeleteObject(memBit);
}

void BaseImage::SelectImage()
{
	ready = false;
	DeleteImage();
	TCHAR FileName[MAX_PATH] = {};
	if (QueryImage(FileName))
	{

		memBit = (HBITMAP)LoadImage(NULL, FileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		if (memBit == 0)
		{
			DWORD ErrNum = GetLastError();
			memBit = 0;
		}
		else
			ready = true;
	}
}
bool BaseImage::QueryImage(TCHAR* Path)
{
	OPENFILENAME OFN = {};
	OFN.lStructSize = sizeof(OFN);
	OFN.hwndOwner = hMainWnd;
	OFN.lpstrFilter = TEXT("BMP FILE (*.BMP*)\0*.BMP*\0");
	OFN.lpstrFile = Path;
	OFN.nMaxFile = MAX_PATH;
	OFN.lpstrTitle = TEXT("Please Select Image");
	OFN.dwReserved = 0;
	return GetOpenFileName(&OFN);
}