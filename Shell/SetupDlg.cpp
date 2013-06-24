#include "stdafx.h"
#include <windows.h>                 // For all that Windows stuff
#include <prsht.h>                   // Property sheet includes
#include "Shell.h"                 // Program-specific stuff

extern HINSTANCE hInst;
extern WORD iYear;
extern WORD iMonth;
extern WORD iDay;
extern WORD iHour;
extern WORD iMinute;
extern BOOL LoopAlertIsOn;
extern BOOL OffAlertIsOn;

// Message handlers
LRESULT DoInitDlgSetup (HWND, UINT, WPARAM, LPARAM);
LRESULT DoCommandSetup (HWND, UINT, WPARAM, LPARAM);
LRESULT DoCloseSetup (HWND, UINT, WPARAM, LPARAM);

// Command functions
LPARAM DoSetupCommandOK (HWND, WORD, HWND, WORD);
LPARAM DoSetupCommandCancel (HWND, WORD, HWND, WORD);
LPARAM DoSetupCommandSetAlert (HWND, WORD, HWND, WORD);
LPARAM DoSetupCommandSetTime (HWND, WORD, HWND, WORD);

// Message dispatch table for MainWindowProc
const struct decodeUINT SetupMessages[] = {
    WM_INITDIALOG, DoInitDlgSetup,
    WM_COMMAND, DoCommandSetup,
};

// Command message dispatch for MainWindowProc
const struct decodeCMD SetupCommandItems[] = {
    IDOK,	DoSetupCommandOK,
	IDCANCEL,		DoSetupCommandCancel,
	IDC_SETALERT,	DoSetupCommandSetAlert,
	IDC_SETTIME,	DoSetupCommandSetTime,
};

BOOL CALLBACK SetupDlgProc (HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) 
{
    INT i;
    //
    // Search message list to see if we need to handle this
    // message.  If in list, call procedure.
    //
    for (i = 0; i < dim(SetupMessages); i++) {
        if (wMsg == SetupMessages[i].Code)
            return (*SetupMessages[i].Fxn)(hWnd, wMsg, wParam, lParam);
    }
    return FALSE;
}

//----------------------------------------------------------------------
// DoInitDlgSetup - Process WM_INITDIALOG message for window.
//
LRESULT DoInitDlgSetup (HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) 
{
	RECT rt, rt1;
	int DlgWidth, DlgHeight;	// dialog width and height in pixel units
	int NewPosX, NewPosY;
	TCHAR szOut[5] = {0};

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

	wsprintf (szOut, TEXT("%d"), 1);
	SetWindowText(GetDlgItem(hWnd, IDC_LOOP),szOut);
	wsprintf (szOut, TEXT("%d"), 1);
	SetWindowText(GetDlgItem(hWnd, IDC_OFF),szOut);

	SendDlgItemMessage(hWnd, IDC_SPIN_LOOP, UDM_SETRANGE, 0,MAKELPARAM(3, 1));
	SendDlgItemMessage(hWnd, IDC_SPIN_LOOP, UDM_SETBUDDY, (WPARAM)GetDlgItem(hWnd, IDC_LOOP), 0);
	SendDlgItemMessage(hWnd, IDC_SPIN_OFF, UDM_SETRANGE, 0,MAKELPARAM(3, 1));
	SendDlgItemMessage(hWnd, IDC_SPIN_OFF, UDM_SETBUDDY, (WPARAM)GetDlgItem(hWnd, IDC_OFF), 0);

	if(LoopAlertIsOn)
	{
		SendMessage(GetDlgItem(hWnd, IDC_CHECK1), BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
	}
	if(OffAlertIsOn)
	{
		SendMessage(GetDlgItem(hWnd, IDC_CHECK2), BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
	}
	return TRUE;
}

//----------------------------------------------------------------------
// DoCommandSetup - Process WM_COMMAND message for window.
//
LRESULT DoCommandSetup (HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) 
{
	WORD idItem, wNotifyCode;
    HWND hwndCtl;
    INT  i;	

    // Parse the parameters.
    idItem = (WORD) LOWORD (wParam);
    wNotifyCode = (WORD) HIWORD (wParam);
    hwndCtl = (HWND) lParam;

    // Call routine to handle control message.
    for (i = 0; i < dim(SetupCommandItems); i++) {
        if (idItem == SetupCommandItems[i].Code)
            return (*SetupCommandItems[i].Fxn)(hWnd, idItem, hwndCtl, wNotifyCode);
    }	
    return TRUE;
}

//----------------------------------------------------------------------
// DoDestroySetup - Process WM_DESTROY message for window.
//
LRESULT DoCloseSetup (HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) 
{
	return 0;
}

LPARAM DoSetupCommandOK (HWND hWnd, WORD idItem, HWND hwndCtl, WORD wNotifyCode) 
{
	//SYSTEMTIME st = {0};
	if(0)
	{
		//TCHAR szYear[5];
		//TCHAR *szStop;
		//GetWindowText(GetDlgItem(hWnd, IDC_YEAR), szYear,5);
		//TCHAR szMonth[3];
		//GetWindowText(GetDlgItem(hWnd, IDC_MONTH), szMonth,3);
		//TCHAR szDay[3];
		//GetWindowText(GetDlgItem(hWnd, IDC_DAY), szDay,3);
		//TCHAR szHour[3];
		//GetWindowText(GetDlgItem(hWnd, IDC_HOUR), szHour,3);
		//TCHAR szMin[3];
		//GetWindowText(GetDlgItem(hWnd, IDC_MIN), szMin,3);
		//st.wYear = (int)wcstod(szYear, &szStop);
		//st.wMonth = (int)wcstod(szMonth, &szStop);
		//st.wDay = (int)wcstod(szDay, &szStop);
		//st.wHour = (int)wcstod(szHour, &szStop);
		//st.wMinute = (int)wcstod(szMin, &szStop);
		//SetSystemTime(&st);
		//UpdateImmeTimeWnd(st);
	}
	if(1)
	{
		//TCHAR szOut[3];
		//TCHAR *szStop;
		//GetWindowText(GetDlgItem(hWnd, IDC_HOUR2), szOut,3);
		//st.wHour = (int)wcstod(szOut, &szStop);

		//GetWindowText(GetDlgItem(hWnd, IDC_MIN2), szOut,3);			
		//st.wMinute = (int)wcstod(szOut, &szStop);
		//SetAlert(st);

		if(LoopAlertIsOn != (BST_CHECKED == SendMessage(GetDlgItem(hWnd, IDC_CHECK1), BM_GETCHECK, 0, 0)))
		{
			if(LoopAlertIsOn)
			{
				//To disable the loop timer
				SetAlertIntervalLoop(-1);
			}
			else
			{
				int loop = GetDlgItemInt(hWnd, IDC_LOOP, NULL, FALSE);
				SetAlertIntervalLoop((loop == 3?60:loop*15));
			}
		}
		if(OffAlertIsOn != (BST_CHECKED == SendMessage(GetDlgItem(hWnd, IDC_CHECK2), BM_GETCHECK, 0, 0)))
		{
			if(OffAlertIsOn)
			{
				//To disable the off timer
				SetAlertIntervaltoOff(-1);
			}
			else
			{
				int off = GetDlgItemInt(hWnd, IDC_OFF, NULL, FALSE);
				SetAlertIntervaltoOff((off == 3?60:off*15));
			}
		}
	}
	EndDialog(hWnd, IDOK);
	return TRUE;
}

LPARAM DoSetupCommandCancel (HWND hWnd, WORD idItem, HWND hwndCtl, WORD wNotifyCode) 
{
	EndDialog(hWnd, IDCANCEL);
	return TRUE;
}

LPARAM DoSetupCommandSetAlert (HWND hWnd, WORD idItem, HWND hwndCtl, WORD wNotifyCode) 
{
	if(IDOK == DialogBox(hInst, (LPCTSTR)IDD_ST, hWnd, (DLGPROC)STDlgProc))
	{
		SYSTEMTIME st = {0};
		st.wHour = iHour;
		st.wMinute = iMinute;
		SetAlert(st);
	}
	return TRUE;
}

LPARAM DoSetupCommandSetTime(HWND hWnd, WORD idItem, HWND hwndCtl, WORD wNotifyCode) 
{
	if((IDOK == DialogBox(hInst, (LPCTSTR)IDD_SD, hWnd, (DLGPROC)SDDlgProc))&&
		(IDOK == DialogBox(hInst, (LPCTSTR)IDD_ST, hWnd, (DLGPROC)STDlgProc)))
	{
		SYSTEMTIME st = {0};
		st.wYear = iYear;
		st.wMonth = iMonth;
		st.wDay = iDay;
		st.wHour = iHour;
		st.wMinute = iMinute;
		UpdateImmeTimeWnd(st);
		SetSystemTime(&st);
	}
	return TRUE;
}