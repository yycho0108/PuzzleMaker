#pragma once
#include <Windows.h>
#include "resource.h"
#include "Globals.h"

BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
bool UpdateDimensions(HWND hDlg);