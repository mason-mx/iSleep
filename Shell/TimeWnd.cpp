#include "stdafx.h"
#include <windows.h>                 // For all that Windows stuff
#include <prsht.h>                   // Property sheet includes
#include "Shell.h"                 // Program-specific stuff

extern HINSTANCE hInst;
TCHAR g_szTime[10];
TCHAR g_szDate[20];

HWND hWndTW = NULL;
HWND hWndParent = NULL;

// Message handlers
LRESULT DoCreateTW (HWND, UINT, WPARAM, LPARAM);
LRESULT DoPaintTW (HWND, UINT, WPARAM, LPARAM);
LRESULT DoEraseBkTW (HWND, UINT, WPARAM, LPARAM);
LRESULT DoDestroyTW (HWND, UINT, WPARAM, LPARAM);
LRESULT DoTimerTW (HWND, UINT, WPARAM, LPARAM);

// Message dispatch table for MainWindowProc
const struct decodeUINT TWMessages[] = {
    WM_CREATE, DoCreateTW,
	WM_PAINT, DoPaintTW,
    WM_DESTROY, DoDestroyTW,
	WM_TIMER, DoTimerTW,
//	WM_ERASEBKGND, DoEraseBkTW,
};

//======================================================================
// Message-handling procedures for TWWindow
//
//----------------------------------------------------------------------
// TWWndProc - Callback function for application window
//
LRESULT CALLBACK TWWndProc (HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    INT i;
    //
    // Search message list to see if we need to handle this
    // message.  If in list, call procedure.
    //
    for (i = 0; i < dim(TWMessages); i++) {
        if (wMsg == TWMessages[i].Code)
            return (*TWMessages[i].Fxn)(hWnd, wMsg, wParam, lParam);
    }
    return DefWindowProc (hWnd, wMsg, wParam, lParam);
}

// Message handlers
LRESULT DoCreateTW(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) 
{
	//To Hide Task bar
	//HWND hStatusBar=FindWindow(TEXT("HHTaskBar"),NULL);
	//ShowWindow(hStatusBar,SW_HIDE);
	SetTimer(hWnd, IDT_TIMER1,60000,(TIMERPROC) NULL);
	
	return 0;
}

LRESULT DoPaintTW(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) 
{
	RECT rc;
	PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
	
	RECT rc1={TW_TIME_RECT_LEFT,TW_TIME_RECT_TOP,TW_TIME_RECT_RIGHT,TW_TIME_RECT_BOTTOM}; 
	RECT rc2={TW_DATE_RECT_LEFT,TW_DATE_RECT_TOP,TW_DATE_RECT_RIGHT,TW_DATE_RECT_BOTTOM}; 
	HDC hdcMem;
	HBITMAP hbmMem;
	HANDLE hOld;

	GetClientRect(hWnd, &rc);

	// Create an off-screen DC for double-buffering
	hdcMem = CreateCompatibleDC(hdc);
	hbmMem = CreateCompatibleBitmap(hdc, rc.right - rc.left, rc.bottom - rc.top);
	hOld   = SelectObject(hdcMem, hbmMem);
	SetBkMode(hdcMem, TRANSPARENT);

	// Draw into hdcMem
	FillRect(hdcMem, &rc, CreateSolidBrush(RGB(0,0,20)));

	HFONT hFont = CreateFont(TW_TimeFontSize, 0, 0, 0, 0, FALSE, FALSE, FALSE, 1, 0, 0, 0, 0,hWnd);
    HFONT hFontOld = (HFONT)SelectObject(hdcMem, hFont);
    SetTextColor(hdcMem, 0xC0C0C0);
    DrawText(hdcMem, g_szTime, -1, &rc1, DT_CENTER | DT_VCENTER);
    SelectObject(hdcMem, hFontOld);
    DeleteObject(hFont); 
#if MULTIMEDIAFEATURE
	hFont = CreateFont(TW_DateFontSize, 0, 0, 0, 0, FALSE, FALSE, FALSE, 1, 0, 0, 0, 0,hWnd);
    hFontOld = (HFONT)SelectObject(hdcMem, hFont);
    //SetTextColor(hdc, 0xC0C0C0);
    DrawText(hdcMem, g_szDate, -1, &rc2, DT_RIGHT | DT_VCENTER);
    SelectObject(hdcMem, hFontOld);
    DeleteObject(hFont);
#endif
	// Transfer the off-screen DC to the screen
	BitBlt(hdc, 0, 0,rc.right - rc.left, rc.bottom - rc.top, hdcMem, 0, 0, SRCCOPY);
	// Free-up the off-screen DC
	SelectObject(hdcMem, hOld);
	DeleteObject(hbmMem);
	DeleteDC    (hdcMem);

    EndPaint(hWnd, &ps);
	
	return 0;
}

LRESULT DoEraseBkTW(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) 
{
	RECT rc;
	PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    GetClientRect(hWnd, &rc);

	RETAILMSG(1, (TEXT("DoEraseBkTW.......\r\n")));
	//
	//RECT rc3={300,0,600,300};
	FillRect(hdc, &rc, CreateSolidBrush(RGB(0,0,20)));
	//RECT rc1={0,0,600,300};
	//RECT rc2={10,300,590,500};
	////SetBkColor(hdc, RGB(0,0,20));
 //   SetBkMode(hdc, TRANSPARENT);
	//
 //   HFONT hFont = CreateFont(120, 0, 0, 0, 0, FALSE, FALSE, FALSE, 1, 0, 0, 0, 0,hWnd);
 //   HFONT hFontOld = (HFONT)SelectObject(hdc, hFont);
 //   SetTextColor(hdc, 0xC0C0C0);
 //   DrawText(hdc, g_szTime, -1, &rc1, DT_CENTER | DT_VCENTER);
 //   SelectObject(hdc, hFontOld);
 //   DeleteObject(hFont); 

	//hFont = CreateFont(50, 0, 0, 0, 0, FALSE, FALSE, FALSE, 1, 0, 0, 0, 0,hWnd);
 //   hFontOld = (HFONT)SelectObject(hdc, hFont);
 //   //SetTextColor(hdc, 0xC0C0C0);
 //   DrawText(hdc, g_szDate, -1, &rc2, DT_RIGHT | DT_VCENTER);
 //   SelectObject(hdc, hFontOld);
 //   DeleteObject(hFont);

    EndPaint(hWnd, &ps);

	return 1;
}

LRESULT DoDestroyTW(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) 
{
	KillTimer(hWnd, IDT_TIMER1);
	/*HWND hStatusBar=FindWindow(TEXT("HHTaskBar"),NULL);
	ShowWindow(hStatusBar,SW_SHOW);*/
	return 0;
}

LRESULT DoTimerTW(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (wParam) 
	{ 
		case IDT_TIMER1: 
			RECT rc;
			SYSTEMTIME st = {0};
			GetSystemTime(&st);
			wsprintf (g_szTime, TEXT("%02d:%02d"), st.wHour, st.wMinute);
			wsprintf (g_szDate, TEXT("%d-%02d-%02d"), st.wYear, st.wMonth, st.wDay);
            GetClientRect(hWnd, &rc);
            InvalidateRect(hWnd, &rc, TRUE);
			//UpdateWindow(hWnd);
	}
	return 0;
}


BOOL TWRegisterClass(HINSTANCE hInstance)
{
	WNDCLASS wndclass = {0}; 
	DWORD    wStyle   = 0;

	wndclass.style         = 0;//CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc   = (WNDPROC) TWWndProc; 
	wndclass.hInstance     = hInstance; 
	wndclass.hbrBackground = NULL;//CreateSolidBrush(RGB(0,0,20));;//(HBRUSH) GetStockObject (DKGRAY_BRUSH);
	wndclass.lpszClassName = TEXT ("TimeWindow"); 

	if (!RegisterClass (&wndclass) ) 
	{
		return FALSE; 
	}
	return TRUE;
}

HWND ShowTimeWnd(HWND hWnd)
{
	SYSTEMTIME st = {0};
	GetSystemTime(&st);
	wsprintf (g_szTime, TEXT("%02d:%02d"), st.wHour, st.wMinute);
	wsprintf (g_szDate, TEXT("%d-%02d-%02d"), st.wYear, st.wMonth, st.wDay);
	RETAILMSG(1, (TEXT("ShowTimeWnd.......\r\n")));
	// Create main window.
    hWndTW = CreateWindow (TEXT ("TimeWindow"),            // Window class
							TEXT ("Time Window"), // Window title
							WS_VISIBLE|WS_CHILD,           // Style flags
							TW_WIN_TOP,        // x position
							TW_WIN_LEFT,        // y position
							TW_WIN_WIDTH,        // Initial width
							TW_WIN_HEIGHT,        // Initial height
							hWnd,                 // Parent
							NULL,                 // Menu, must be null
							hInst,            // Application instance
							NULL);                // Pointer to create
                                               // parameters
    // Return fail code if window not created.
    if (!IsWindow (hWndTW)) return NULL;

    // Standard show and update calls
    ShowWindow (hWndTW, TRUE);
    UpdateWindow (hWndTW);	
	
    return hWndTW;
}

HWND ShowImmeTimeWnd(HWND hWnd,SYSTEMTIME immeTime)
{
	hWndParent = hWnd;
	wsprintf (g_szTime, TEXT("%02d:%02d"), immeTime.wHour, immeTime.wMinute);
	wsprintf (g_szDate, TEXT("%d-%02d-%02d"), immeTime.wYear, immeTime.wMonth, immeTime.wDay);
	RETAILMSG(1, (TEXT("ShowImmeTimeWnd.......\r\n")));
	// Create main window.
    hWndTW = CreateWindow (TEXT ("TimeWindow"),            // Window class
							TEXT ("Time Window"), // Window title
							WS_VISIBLE|WS_CHILD,           // Style flags
							TW_WIN_TOP,        // x position
							TW_WIN_LEFT,        // y position
							TW_WIN_WIDTH,        // Initial width
							TW_WIN_HEIGHT,        // Initial height
							hWnd,                 // Parent
							NULL,                 // Menu, must be null
							hInst,            // Application instance
							NULL);                // Pointer to create
                                               // parameters
    // Return fail code if window not created.
    if (!IsWindow (hWndTW)) return NULL;

    // Standard show and update calls
    ShowWindow (hWndTW, TRUE);
    UpdateWindow (hWndTW);	
	
    return hWndTW;
}

BOOL UpdateImmeTimeWnd(SYSTEMTIME immeTime)
{	
	RECT rt;
	wsprintf (g_szTime, TEXT("%02d:%02d"), immeTime.wHour, immeTime.wMinute);
	wsprintf (g_szDate, TEXT("%d-%02d-%02d"), immeTime.wYear, immeTime.wMonth, immeTime.wDay);
	GetClientRect(hWndTW, &rt);
    InvalidateRect(hWndTW, &rt, TRUE);
    return TRUE;
}