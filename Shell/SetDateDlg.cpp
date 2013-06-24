#include "stdafx.h"
#include <windows.h>                 // For all that Windows stuff
#include <prsht.h>                   // Property sheet includes
#include "Shell.h"                 // Program-specific stuff

extern HINSTANCE hInst;
extern WORD iYear;
extern WORD iMonth;
extern WORD iDay;
//extern TCHAR *szTestCase[];

// Message handlers
LRESULT DoInitDlgSD (HWND, UINT, WPARAM, LPARAM);
LRESULT DoCommandSD (HWND, UINT, WPARAM, LPARAM);
LRESULT DoCloseSD (HWND, UINT, WPARAM, LPARAM);

// Command functions
LPARAM DoSDCommandOK (HWND, WORD, HWND, WORD);
LPARAM DoSDCommandCancel (HWND, WORD, HWND, WORD);

// Message dispatch table for MainWindowProc
const struct decodeUINT SDMessages[] = {
    WM_INITDIALOG, DoInitDlgSD,
    WM_COMMAND, DoCommandSD,
};

// Command message dispatch for MainWindowProc
const struct decodeCMD SDCommandItems[] = {
    IDOK,	DoSDCommandOK,
	IDCANCEL,		DoSDCommandCancel,
};

BOOL CALLBACK SDDlgProc (HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) 
{
    INT i;
    //
    // Search message list to see if we need to handle this
    // message.  If in list, call procedure.
    //
    for (i = 0; i < dim(SDMessages); i++) {
        if (wMsg == SDMessages[i].Code)
            return (*SDMessages[i].Fxn)(hWnd, wMsg, wParam, lParam);
    }
    return FALSE;
}

//----------------------------------------------------------------------
// DoInitDlgSD - Process WM_INITDIALOG message for window.
//
LRESULT DoInitDlgSD (HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) 
{
	RECT rt, rt1;
	int DlgWidth, DlgHeight;	// dialog width and height in pixel units
	int NewPosX, NewPosY;

    // trying to center the About dialog
	if (GetWindowRect(hWnd, &rt1)) {
		GetClientRect(GetParent(hWnd), &rt);
		DlgWidth	= rt1.right - rt1.left;
		DlgHeight	= rt1.bottom - rt1.top ;
		NewPosX		= (rt.right - rt.left - DlgWidth)/2;
		NewPosY		= (rt.bottom - rt.top - DlgHeight)/2;
		
		// if the About box is larger than the physical screen 
		if (NewPosX < 0) NewPosX = 0;
		if (NewPosY < 0) NewPosY = 0;
		SetWindowPos(hWnd, 0, NewPosX, NewPosY,
			0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}
	SetWindowText(GetDlgItem(hWnd, IDC_YEAR), L"2011");
	SetWindowText(GetDlgItem(hWnd, IDC_MONTH), L"10");
	SetWindowText(GetDlgItem(hWnd, IDC_DAY), L"10");
	SendDlgItemMessage(hWnd, IDC_SPIN_YEAR, UDM_SETRANGE, 0,MAKELPARAM(2080, 2010));
	SendDlgItemMessage(hWnd, IDC_SPIN_YEAR, UDM_SETBUDDY, (WPARAM)GetDlgItem(hWnd, IDC_YEAR), 0);
	SendDlgItemMessage(hWnd, IDC_SPIN_MONTH, UDM_SETRANGE, 0,MAKELPARAM(12, 1));
	SendDlgItemMessage(hWnd, IDC_SPIN_MONTH, UDM_SETBUDDY, (WPARAM)GetDlgItem(hWnd, IDC_MONTH), 0);
	SendDlgItemMessage(hWnd, IDC_SPIN_DAY, UDM_SETRANGE, 0,MAKELPARAM(31, 1));
	SendDlgItemMessage(hWnd, IDC_SPIN_DAY, UDM_SETBUDDY, (WPARAM)GetDlgItem(hWnd, IDC_DAY), 0);
	return TRUE;
}

//----------------------------------------------------------------------
// DoCommandSD - Process WM_COMMAND message for window.
//
LRESULT DoCommandSD (HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) 
{
	WORD idItem, wNotifyCode;
    HWND hwndCtl;
    INT  i;	

    // Parse the parameters.
    idItem = (WORD) LOWORD (wParam);
    wNotifyCode = (WORD) HIWORD (wParam);
    hwndCtl = (HWND) lParam;

    // Call routine to handle control message.
    for (i = 0; i < dim(SDCommandItems); i++) {
        if (idItem == SDCommandItems[i].Code)
            return (*SDCommandItems[i].Fxn)(hWnd, idItem, hwndCtl, wNotifyCode);
    }	
    return TRUE;
}

//----------------------------------------------------------------------
// DoDestroySD - Process WM_DESTROY message for window.
//
LRESULT DoCloseSD (HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) 
{
	return 0;
}

LPARAM DoSDCommandOK (HWND hWnd, WORD idItem, HWND hwndCtl, WORD wNotifyCode) 
{
	TCHAR szYear[5];
	TCHAR *szStop;
	GetWindowText(GetDlgItem(hWnd, IDC_YEAR), szYear,5);
	TCHAR szMonth[3];
	GetWindowText(GetDlgItem(hWnd, IDC_MONTH), szMonth,3);
	TCHAR szDay[3];
	GetWindowText(GetDlgItem(hWnd, IDC_DAY), szDay,3);
	iYear = (int)wcstod(szYear, &szStop);
	iMonth = (int)wcstod(szMonth, &szStop);
	iDay = (int)wcstod(szDay, &szStop);
	EndDialog(hWnd, IDOK);
	return TRUE;
}

LPARAM DoSDCommandCancel (HWND hWnd, WORD idItem, HWND hwndCtl, WORD wNotifyCode) 
{
	EndDialog(hWnd, IDCANCEL);
	return TRUE;
}