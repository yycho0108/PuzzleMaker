#include <Windows.h>
#include <windowsx.h>

#include "Globals.h"
#include "resource.h"
#include "Dialog.h"
#include "Board.h"

//Operation Stuff
ATOM RegisterCustomClass(HINSTANCE& hInstance);
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
enum { ID_CHILDWINDOW = 100 };

LPCTSTR Title = L"Puzzles";
HINSTANCE g_hInst;
HWND hMainWnd;

//Game Stuff
bool LoadGame();
Board* mainBoard = nullptr;
int wndWidth, wndHeight;
bool maximized = false;

ATOM RegisterCustomClass(HINSTANCE& hInstance)
{
	WNDCLASS wc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)GetSysColorBrush(COLOR_WINDOW);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = Title;
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wc.style = CS_VREDRAW | CS_HREDRAW;
	return RegisterClass(&wc);
}
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	g_hInst = hInstance;
	RegisterCustomClass(hInstance);
	hMainWnd = CreateWindow(Title, Title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	ShowWindow(hMainWnd, nCmdShow);

	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static POINT prevPoint;
	static POINT curPoint;

	switch (iMsg)
	{
	case WM_CREATE:
		hMainWnd = hWnd;
		if (!LoadGame())
			return -1;
		break;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case ID_FILE_NEWPUZZLE:
			if (!LoadGame())
				DestroyWindow(hWnd);
			else
				InvalidateRect(hWnd, NULL, true);
			break;
		case ID_FILE_EXIT:
			DestroyWindow(hWnd);
			break;
		}
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		RECT R = { 0,0,1000,1000 };
		FillRect(hdc, &R, (HBRUSH)GetStockObject(WHITE_BRUSH));
		mainBoard->Print(hdc);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_LBUTTONDOWN:
	{
		HDC hdc = GetDC(hWnd);
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		if (mainBoard->getPiece(x, y))
		{
			prevPoint = curPoint = {x,y};
		}
		ReleaseDC(hWnd, hdc);
		break;
	}
	case WM_MOUSEMOVE:
	{
		curPoint = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		mainBoard->movePiece(curPoint.x - prevPoint.x, curPoint.y - prevPoint.y);
		prevPoint = curPoint;
		break;
	}
	case WM_LBUTTONUP:
	{
		mainBoard->releasePiece();
		break;
	}
	case WM_SIZE:
	{
		if (wParam == SIZE_MAXIMIZED)
		{
			wndWidth = LOWORD(lParam);
			wndHeight = HIWORD(lParam);
			maximized = true;
			mainBoard->updateRgn();
			InvalidateRect(hWnd, NULL, true);
		}
		else if (wParam == SIZE_RESTORED)
		{
			if (maximized)
			{
				wndWidth = LOWORD(lParam);
				wndHeight = HIWORD(lParam);
				mainBoard->updateRgn();
				InvalidateRect(hWnd, NULL, true);
			}
			maximized = false;
		}
		break;
	}
	case WM_EXITSIZEMOVE:
	{
		RECT R;
		GetClientRect(hWnd, &R);
		wndWidth = R.right - R.left;
		wndHeight = R.bottom - R.top;
		mainBoard->updateRgn();
		InvalidateRect(hWnd, NULL, true);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, iMsg, wParam, lParam);
	}
	return 0;
}

//what to add:
//mate(puzzle pieces join)
//menu
//music?
//save state...maybe.
// # of bumps.


bool LoadGame()
{
	mainBoard = nullptr;

	if (DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), hMainWnd, DlgProc) != IDOK)
		return false;
	mainBoard = new Board();

	return mainBoard ? true : false;
}