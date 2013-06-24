#include "stdafx.h"
#include <windows.h>                 // For all that Windows stuff
#include <prsht.h>                   // Property sheet includes
#include "Shell.h"                 // Program-specific stuff

extern HINSTANCE hInst;
extern WORD iHour;
extern WORD iMinute;

// Message handlers
LRESULT DoInitDlgST (HWND, UINT, WPARAM, LPARAM);
LRESULT DoCommandST (HWND, UINT, WPARAM, LPARAM);
LRESULT DoCloseST (HWND, UINT, WPARAM, LPARAM);

// Command functions
LPARAM DoSTCommandOK (HWND, WORD, HWND, WORD);
LPARAM DoSTCommandCancel (HWND, WORD, HWND, WORD);

// Message dispatch table for MainWindowProc
const struct decodeUINT STMessages[] = {
    WM_INITDIALOG, DoInitDlgST,
    WM_COMMAND, DoCommandST,
};

// Command message dispatch for MainWindowProc
const struct decodeCMD STCommandItems[] = {
    IDOK,	DoSTCommandOK,
	IDCANCEL,		DoSTCommandCancel,
};

BOOL CALLBACK STDlgProc (HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) 
{
    INT i;
    //
    // Search message list to see if we need to handle this
    // message.  If in list, call procedure.
    //
    for (i = 0; i < dim(STMessages); i++) {
        if (wMsg == STMessages[i].Code)
            return (*STMessages[i].Fxn)(hWnd, wMsg, wParam, lParam);
    }
    return FALSE;
}

//----------------------------------------------------------------------
// DoInitDlgST - Process WM_INITDIALOG message for window.
//
LRESULT DoInitDlgST (HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) 
{
	RECT rt, rt1;
	int DlgWidth, DlgHeight;	// dialog width and height in pixel units
	int NewPosX, NewPosY;
//	SYSTEMTIME st;
//	TCHAR szOut[128];

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
	SetWindowText(GetDlgItem(hWnd, IDC_HOUR), L"12");
	SetWindowText(GetDlgItem(hWnd, IDC_MIN), L"00");
	SendDlgItemMessage(hWnd, IDC_SPIN_HOUR, UDM_SETRANGE, 0,MAKELPARAM(23, 0));
	SendDlgItemMessage(hWnd, IDC_SPIN_HOUR, UDM_SETBUDDY, (WPARAM)GetDlgItem(hWnd, IDC_HOUR), 0);
	SendDlgItemMessage(hWnd, IDC_SPIN_MIN, UDM_SETRANGE, 0,MAKELPARAM(59, 0));
	SendDlgItemMessage(hWnd, IDC_SPIN_MIN, UDM_SETBUDDY, (WPARAM)GetDlgItem(hWnd, IDC_MIN), 0);

	return TRUE;
}

//----------------------------------------------------------------------
// DoCommandST - Process WM_COMMAND message for window.
//
LRESULT DoCommandST (HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) 
{
	WORD idItem, wNotifyCode;
    HWND hwndCtl;
    INT  i;	

    // Parse the parameters.
    idItem = (WORD) LOWORD (wParam);
    wNotifyCode = (WORD) HIWORD (wParam);
    hwndCtl = (HWND) lParam;

    // Call routine to handle control message.
    for (i = 0; i < dim(STCommandItems); i++) {
        if (idItem == STCommandItems[i].Code)
            return (*STCommandItems[i].Fxn)(hWnd, idItem, hwndCtl, wNotifyCode);
    }	
    return TRUE;
}

//----------------------------------------------------------------------
// DoDestroyST - Process WM_DESTROY message for window.
//
LRESULT DoCloseST (HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) 
{
	return 0;
}

LPARAM DoSTCommandOK (HWND hWnd, WORD idItem, HWND hwndCtl, WORD wNotifyCode) 
{
	TCHAR szHour[3];
	TCHAR *szStop;
	GetWindowText(GetDlgItem(hWnd, IDC_HOUR), szHour,3);
	TCHAR szMinute[3];
	GetWindowText(GetDlgItem(hWnd, IDC_MIN), szMinute,3);
	iHour = (int)wcstod(szHour, &szStop);
	iMinute = (int)wcstod(szMinute, &szStop);
	EndDialog(hWnd, IDOK);
	return TRUE;
}

LPARAM DoSTCommandCancel (HWND hWnd, WORD idItem, HWND hwndCtl, WORD wNotifyCode) 
{
	EndDialog(hWnd, IDCANCEL);
	return TRUE;
}