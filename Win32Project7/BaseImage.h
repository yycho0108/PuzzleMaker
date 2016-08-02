#pragma once
#include <Windows.h>
#include "Globals.h"

struct BaseImage
{
	bool ready;
	HBITMAP memBit;

	void SelectImage();
	bool QueryImage(TCHAR* Path);
	void DeleteImage();

	BaseImage();
	~BaseImage();
};