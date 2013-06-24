#include "stdafx.h"
#include <windows.h>                 // For all that Windows stuff
#include <prsht.h>                   // Property sheet includes
#include "Shell.h"                 // Program-specific stuff

extern HINSTANCE hInst;
//extern TCHAR *szTestCase[];

// Message handlers
LRESULT DoInitDlgTST (HWND, UINT, WPARAM, LPARAM);
LRESULT DoCommandTST (HWND, UINT, WPARAM, LPARAM);
LRESULT DoCloseTST (HWND, UINT, WPARAM, LPARAM);
LRESULT DoKeyDownTST (HWND, UINT, WPARAM, LPARAM);

// Command functions
LPARAM DoTSTCommandOK (HWND, WORD, HWND, WORD);
LPARAM DoTSTCommandCancel (HWND, WORD, HWND, WORD);

// Message dispatch table for MainWindowProc
const struct decodeUINT TSTMessages[] = {
    WM_INITDIALOG, DoInitDlgTST,
    WM_COMMAND, DoCommandTST,
	WM_KEYDOWN, DoKeyDownTST,
};

// Command message dispatch for MainWindowProc
const struct decodeCMD TSTCommandItems[] = {
    IDOK,	DoTSTCommandOK,
	IDCANCEL,		DoTSTCommandCancel,
};

BOOL CALLBACK TSTDlgProc (HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) 
{
    INT i;
    //
    // Search message list to see if we need to handle this
    // message.  If in list, call procedure.
    //
    for (i = 0; i < dim(TSTMessages); i++) {
        if (wMsg == TSTMessages[i].Code)
            return (*TSTMessages[i].Fxn)(hWnd, wMsg, wParam, lParam);
    }
    return FALSE;
}

//----------------------------------------------------------------------
// DoInitDlgTST - Process WM_INITDIALOG message for window.
//
LRESULT DoInitDlgTST (HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) 
{
	RECT rt, rt1;
	int DlgWidth, DlgHeight;	// dialog width and height in pixel units
	int NewPosX, NewPosY;
	SYSTEMTIME st;
	TCHAR szOut[128];

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
	GetSystemTime(&st);
	wsprintf (szOut, TEXT("%d-%d-%d %d: %d"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
	SetWindowText(GetDlgItem(hWnd, IDC_CURTIME), szOut);
	return TRUE;
}

//----------------------------------------------------------------------
// DoCommandTST - Process WM_COMMAND message for window.
//
LRESULT DoCommandTST (HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) 
{
	WORD idItem, wNotifyCode;
    HWND hwndCtl;
    INT  i;	

    // Parse the parameters.
    idItem = (WORD) LOWORD (wParam);
    wNotifyCode = (WORD) HIWORD (wParam);
    hwndCtl = (HWND) lParam;

    // Call routine to handle control message.
    for (i = 0; i < dim(TSTCommandItems); i++) {
        if (idItem == TSTCommandItems[i].Code)
            return (*TSTCommandItems[i].Fxn)(hWnd, idItem, hwndCtl, wNotifyCode);
    }	
    return TRUE;
}

//----------------------------------------------------------------------
// DoDestroyTST - Process WM_DESTROY message for window.
//
LRESULT DoCloseTST (HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) 
{
	return 0;
}

LRESULT DoKeyDownTST (HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) 
{
	if (wParam == VK_ESCAPE)
	{
		//SendMessage(hWnd, WM_CLOSE, 0, 0);
	}
	return 0;
}

LPARAM DoTSTCommandOK (HWND hWnd, WORD idItem, HWND hwndCtl, WORD wNotifyCode) 
{
	EndDialog(hWnd, IDOK);
	return TRUE;
}

LPARAM DoTSTCommandCancel (HWND hWnd, WORD idItem, HWND hwndCtl, WORD wNotifyCode) 
{
	EndDialog(hWnd, IDCANCEL);
	return TRUE;
}