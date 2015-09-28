#include "Dialog.h"
#include "BaseImage.h"

BaseImage Image;

int brdWidth=0, brdHeight=0;
int imgWidth=0, imgHeight=0;
int tileWidth=0, tileHeight=0;
int twSmall = 0, thSmall = 0;
BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		return true;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:
		{
			Image.SelectImage();
			BITMAP Bit;
			GetObject(Image.memBit, sizeof(BITMAP), &Bit);
			imgHeight = Bit.bmHeight;
			imgWidth = Bit.bmWidth;
			SetDlgItemInt(hDlg, IDC_EDIT3, imgHeight, FALSE);
			SetDlgItemInt(hDlg, IDC_EDIT4, imgWidth, FALSE);
			UpdateDimensions(hDlg);
			break;
		}
		case IDC_BUTTON2:
		{
			UpdateDimensions(hDlg);
			break;
		}
		case IDOK:
			EndDialog(hDlg, UpdateDimensions(hDlg)?IDOK:IDCANCEL);
			break;
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			break;
		}
		return true;
	}
	case WM_DESTROY:
		return true;
	}
	return false;
}

bool UpdateDimensions(HWND hDlg)
{
	BOOL check = true;
	brdWidth = GetDlgItemInt(hDlg, IDC_EDIT1,&check, FALSE);
	brdHeight = GetDlgItemInt(hDlg, IDC_EDIT2,&check,FALSE);

	if (check && brdWidth >0 && brdHeight >0)
	{
		tileWidth = imgWidth / brdWidth;
		tileHeight = imgHeight / brdHeight;
		SetDlgItemInt(hDlg, IDC_EDIT5, tileWidth, FALSE);
		SetDlgItemInt(hDlg, IDC_EDIT6, tileHeight, FALSE);
		twSmall = tileWidth / 5;
		thSmall = tileHeight / 5;
		return true;
	}
	else
	{
		SetDlgItemText(hDlg, IDC_EDIT5, TEXT("N/A"));
		SetDlgItemText(hDlg, IDC_EDIT6, TEXT("N/A"));
		return false;
	}
}