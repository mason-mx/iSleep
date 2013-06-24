#include "stdafx.h"
#include <windows.h>                 // For all that Windows stuff
#include <prsht.h>                   // Property sheet includes
#include <Pm.h>
#include <windev.h>
#include "Shell.h"                 // Program-specific stuff

extern HINSTANCE hInst;
extern HANDLE g_hBkLight;
extern BOOL g_bBKLOff;

TCHAR g_szAlert[10];
TCHAR g_szLoopAlert[25];
TCHAR g_szOffAlert[20];

#define IDT_TIMER1		9001
#define IDT_TIMER2		9002
#define IDT_TIMER3		9003
HWND hWndAW = NULL;
SYSTEMTIME stAlert = {0};
int AlertIntervalLoop = 30*60000;
int AlertIntervaltoOff = 30*60000;
BOOL AlertIsOn = TRUE;
BOOL LoopAlertIsOn = TRUE;
BOOL OffAlertIsOn = TRUE;
DWORD g_sysVol = 0;

HBITMAP g_hbmBall;

void increaseVol();

// Message handlers
LRESULT DoCreateAW (HWND, UINT, WPARAM, LPARAM);
LRESULT DoPaintAW (HWND, UINT, WPARAM, LPARAM);
LRESULT DoDestroyAW (HWND, UINT, WPARAM, LPARAM);
LRESULT DoTimerAW (HWND, UINT, WPARAM, LPARAM);

// Message dispatch table for MainWindowProc
const struct decodeUINT AWMessages[] = {
    WM_CREATE, DoCreateAW,
	WM_PAINT, DoPaintAW,
    WM_DESTROY, DoDestroyAW,
	WM_TIMER, DoTimerAW,
};

//======================================================================
// Message-handling procedures for AWWindow
//
//----------------------------------------------------------------------
// AWWndProc - Callback function for application window
//
LRESULT CALLBACK AWWndProc (HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    INT i;
    //
    // Search message list to see if we need to handle this
    // message.  If in list, call procedure.
    //
    for (i = 0; i < dim(AWMessages); i++) {
        if (wMsg == AWMessages[i].Code)
            return (*AWMessages[i].Fxn)(hWnd, wMsg, wParam, lParam);
    }
    return DefWindowProc (hWnd, wMsg, wParam, lParam);
}

// Message handlers
LRESULT DoCreateAW(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) 
{
	//HWND hCtrl0_0 = CreateWindow(TEXT("Button"), TEXT("Set Alert"), WS_VISIBLE | WS_CHILD, 10, 200, 100, 25, hWnd, (HMENU)IDC_SETALERT, hInst, 0);
	/*SendMessage(hCtrl0_0, WM_SETFONT, (WPARAM), FALSE);
	HWND hCtrl0_1 = CreateWindowEx(0, UPDOWN_CLASS, 0, WS_VISIBLE | WS_CHILD | UDS_ALIGNLEFT | UDS_ARROWKEYS | UDS_SETBUDDYINT, 160, 84, 18, 34, hwnd, (HMENU)IDC_SPIN1, hInst, 0);
	SendMessage(hCtrl0_1, WM_SETFONT, (WPARAM), FALSE);
	HWND hCtrl0_2 = CreateWindowEx(0, WC_BUTTON, ("OK"), WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_NOTIFY, 22, 178, 50, 26, hwnd, (HMENU)IDC_OK, hInst, 0);
	SendMessage(hCtrl0_2, WM_SETFONT, (WPARAM), FALSE);
	HWND hCtrl0_3 = CreateWindowEx(0, WC_BUTTON, ("Reset"), WS_VISIBLE | WS_CHILD | WS_TABSTOP, 100, 178, 78, 26, hwnd, (HMENU)IDC_CANCE, hInst, 0);
	SendMessage(hCtrl0_3, WM_SETFONT, (WPARAM), FALSE);*/
	g_hbmBall = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP1));
	waveOutGetVolume(0, &g_sysVol);
	return 0;
}

LRESULT DoPaintAW(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) 
{
	PAINTSTRUCT ps;
	BITMAP bm;
    HDC hdc = BeginPaint(hWnd, &ps);
	RECT rc1={AW_ALERT_RECT_LEFT,AW_ALERT_RECT_TOP,AW_ALERT_RECT_RIGHT,AW_ALERT_RECT_BOTTOM};
	RECT rc2={AW_MISC_RECT_LEFT,AW_MISC_RECT_TOP,AW_MISC_RECT_RIGHT,AW_MISC_RECT_BOTTOM};    

	HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, g_hbmBall);	

    GetObject(g_hbmBall, sizeof(bm), &bm);

    BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

    SelectObject(hdcMem, hbmOld);	
    DeleteDC(hdcMem);

    SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, 0xC0C0C0);
	if(AlertIsOn)
	{
		HFONT hFont = CreateFont(AW_AlertFontSize, 0, 0, 0, 0, FALSE, FALSE, FALSE, 1, 0, 0, 0, 0,hWnd);
		HFONT hFontOld = (HFONT)SelectObject(hdc, hFont);
		DrawText(hdc, g_szAlert, -1, &rc1, DT_RIGHT | DT_TOP);
		SelectObject(hdc, hFontOld);
		DeleteObject(hFont); 
	}
	if(LoopAlertIsOn)
	{
		HFONT hFont = CreateFont(AW_MiscFontSize, 0, 0, 0, 0, FALSE, FALSE, FALSE, 1, 0, 0, 0, 0,hWnd);
		HFONT hFontOld = (HFONT)SelectObject(hdc, hFont);
		DrawText(hdc, g_szLoopAlert, -1, &rc2, DT_RIGHT | DT_TOP);
		SelectObject(hdc, hFontOld);
		DeleteObject(hFont); 		
	}
	if(OffAlertIsOn)
	{
		HFONT hFont = CreateFont(AW_MiscFontSize, 0, 0, 0, 0, FALSE, FALSE, FALSE, 1, 0, 0, 0, 0,hWnd);
		HFONT hFontOld = (HFONT)SelectObject(hdc, hFont);
		DrawText(hdc, g_szOffAlert, -1, &rc2, DT_RIGHT | DT_BOTTOM);
		SelectObject(hdc, hFontOld);
		DeleteObject(hFont); 		
	}   

    EndPaint(hWnd, &ps);
	return 0;
}


LRESULT DoDestroyAW(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) 
{
	KillTimer(hWnd, IDT_TIMER1);
	KillTimer(hWnd, IDT_TIMER2);
	DeleteObject(g_hbmBall);
	/*HWND hStatusBar=FindWindow(TEXT("HHTaskBar"),NULL);
	ShowWindow(hStatusBar,SW_SHOW);*/
	return 0;
}

LRESULT DoTimerAW(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (wParam) 
	{ 
		case IDT_TIMER1: 
			{
				SYSTEMTIME st = {0};
				GetSystemTime(&st);
				if((st.wHour == stAlert.wHour)&&(st.wMinute == stAlert.wMinute))
				{
					CreateThread(NULL, 0,(LPTHREAD_START_ROUTINE)increaseVol, NULL,0, NULL);
					PlayMedia();
					//To do set backlight to be on
					g_bBKLOff = FALSE;
					CEDEVICE_POWER_STATE off = D0;
					if (g_hBkLight)
					{
						RETAILMSG(1, (L"backlight->on\r\n"));
						//DeviceIoControl(g_hBkLight, IOCTL_POWER_SET, NULL, 0, &off, sizeof(CEDEVICE_POWER_STATE), NULL, NULL);					
					}
				}
				
			}
			break;
		case IDT_TIMER2: 
			PlaySound(TEXT("\\LBA_NAND\\ni.wav"), NULL, SND_ASYNC);
			break;
		case IDT_TIMER3:
			{
				//To do set backlight to be off
				g_bBKLOff = TRUE;
				CEDEVICE_POWER_STATE off = D4;
				if (g_hBkLight)
				{
					RETAILMSG(1, (L"backlight->off\r\n"));
					//DeviceIoControl(g_hBkLight, IOCTL_POWER_SET, NULL, 0, &off, sizeof(CEDEVICE_POWER_STATE), NULL, NULL);					
				}
				PauseMedia();
				waveOutGetVolume(0, &g_sysVol);
				KillTimer(hWndAW, IDT_TIMER3);
			}
			break;
	}
	return 0;
}

BOOL AWRegisterClass(HINSTANCE hInstance)
{
	WNDCLASS wndclass = {0}; 
	DWORD    wStyle   = 0;

	wndclass.style         = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc   = (WNDPROC) AWWndProc; 
	wndclass.hInstance     = hInstance; 
	wndclass.hbrBackground = CreateSolidBrush(RGB(20,0,0));
	wndclass.lpszClassName = TEXT ("AlertWindow"); 

	if (!RegisterClass (&wndclass) ) 
	{
		return FALSE; 
	}
	return TRUE;
}

BOOL ShowAlertWnd(HWND hWnd)
{
	// Create main window.
    hWndAW = CreateWindow (TEXT ("AlertWindow"),            // Window class
							TEXT ("Alert Window"), // Window title
							WS_VISIBLE|WS_CHILD,           // Style flags
							AW_WIN_LEFT,        // x position
							AW_WIN_TOP,        // y position
							AW_WIN_WIDTH,        // Initial width
							AW_WIN_HEIGHT,        // Initial height
							hWnd,                 // Parent
							NULL,                 // Menu, must be null
							hInst,            // Application instance
							NULL);                // Pointer to create
                                               // parameters
    // Return fail code if window not created.
    if (!IsWindow (hWndAW)) return FALSE;

    // Standard show and update calls
    ShowWindow (hWndAW, TRUE);
    UpdateWindow (hWndAW);	
	
    return TRUE;
}

BOOL SetAlert(SYSTEMTIME st)
{
	stAlert.wHour = st.wHour;
	stAlert.wMinute = st.wMinute;
	wsprintf (g_szAlert, TEXT("@%02d:%02d"), stAlert.wHour, stAlert.wMinute);
	RECT rt;
	if(NULL != hWndAW)
	{
		GetClientRect(hWndAW, &rt);
		InvalidateRect(hWndAW, &rt, TRUE);
		KillTimer(hWndAW, IDT_TIMER1);
		SetTimer(hWndAW, IDT_TIMER1,60000,(TIMERPROC) NULL);
	}
	
	AlertIsOn = TRUE;

    return TRUE;
}

BOOL SetAlertIntervalLoop(int Interval)
{
	if(-1 == Interval)
		LoopAlertIsOn = FALSE;
	else
		LoopAlertIsOn = TRUE;
	AlertIntervalLoop = 60000*Interval;
	wsprintf (g_szLoopAlert, TEXT("Rest Alert in %dmins"), Interval);
	RECT rt;
	if(NULL != hWndAW)
	{
		GetClientRect(hWndAW, &rt);
		InvalidateRect(hWndAW, &rt, TRUE);
		KillTimer(hWndAW, IDT_TIMER2);
		if(LoopAlertIsOn)
		{			
			if(AlertIntervalLoop > 0)
				SetTimer(hWndAW, IDT_TIMER2,AlertIntervalLoop,(TIMERPROC) NULL);
		}
	}
    return TRUE;
}

BOOL SetAlertIntervaltoOff(int Interval)
{
	if(-1 == Interval)
		OffAlertIsOn = FALSE;
	else
		OffAlertIsOn = TRUE;
	AlertIntervaltoOff = 60000*Interval;
	wsprintf (g_szOffAlert, TEXT("Off in %dmins"), Interval);
	RECT rt;
	if(NULL != hWndAW)
	{
		GetClientRect(hWndAW, &rt);
		InvalidateRect(hWndAW, &rt, TRUE);
		KillTimer(hWndAW, IDT_TIMER3);
		if(OffAlertIsOn)
		{			
			if(AlertIntervaltoOff > 0)
				SetTimer(hWndAW, IDT_TIMER3,AlertIntervaltoOff,(TIMERPROC) NULL);
		}
	}	
    return TRUE;
}

void increaseVol()
{
	DWORD curVol = 0;
	while(curVol < g_sysVol){
		waveOutSetVolume(0, curVol);
		curVol += g_sysVol/8;
	}	
}