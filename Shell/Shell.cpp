// Shell.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Shell.h"
#include <windows.h>
#include <Pm.h>
#include <commctrl.h>
#include <windev.h>
#include <dbt.h>
#include <storemgr.h>
#include "MyCalendar.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE			hInst;			// current instance
#ifdef SHELL_AYGSHELL
HWND				g_hWndMenuBar;		// menu bar handle
#else // SHELL_AYGSHELL
HWND				g_hWndCommandBar;	// command bar handle
#endif // SHELL_AYGSHELL

WORD iHour;
WORD iMinute;
WORD iYear;
WORD iMonth;
WORD iDay;

HANDLE g_hBkLight;
HANDLE g_Event;
BOOL g_bBKLOff = FALSE;
BOOL g_MediaScanIsDone = FALSE;

CMyCalendar g_Calendar2;

LRESULT DoCreateShell (HWND, UINT, WPARAM, LPARAM);
LRESULT DoPaintShell (HWND, UINT, WPARAM, LPARAM);
LRESULT DoDestroyShell (HWND, UINT, WPARAM, LPARAM);
LRESULT DoKeyDownShell (HWND, UINT, WPARAM, LPARAM);
LRESULT DoSysKeyDownShell (HWND, UINT, WPARAM, LPARAM);
LRESULT DoDeviceChangeShell (HWND, UINT, WPARAM, LPARAM);

// Forward declarations of functions included in this code module:
ATOM			MyRegisterClass(HINSTANCE, LPTSTR);
BOOL			InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void MediaScan();
void PlayNextMediaFolder();
void USBDetect(HWND);
void MediaScanDoneNotify(HWND);

const struct decodeUINT ShellMessages[] = {
    WM_CREATE, DoCreateShell,
	WM_PAINT, DoPaintShell,
    WM_DESTROY, DoDestroyShell,
	WM_KEYDOWN, DoKeyDownShell,
	WM_SYSKEYDOWN, DoSysKeyDownShell,
	WM_DEVICECHANGE,DoDeviceChangeShell,
};

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR    lpCmdLine,
                   int       nCmdShow)
{
	MSG msg;

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	HACCEL hAccelTable;
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SHELL));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			// TODO: Add your specialized code here and/or call the base class			
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
ATOM MyRegisterClass(HINSTANCE hInstance, LPTSTR szWindowClass)
{
	WNDCLASS wc;

	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SHELL));
	wc.hCursor       = 0;
	wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = szWindowClass;

	return RegisterClass(&wc);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;
    TCHAR szTitle[MAX_LOADSTRING];		// title bar text
    TCHAR szWindowClass[MAX_LOADSTRING];	// main window class name

    hInst = hInstance; // Store instance handle in our global variable

#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
    // SHInitExtraControls should be called once during your application's initialization to initialize any
    // of the device specific controls such as CAPEDIT and SIPPREF.
    SHInitExtraControls();
#endif // WIN32_PLATFORM_PSPC || WIN32_PLATFORM_WFSP

    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING); 
    LoadString(hInstance, IDC_SHELL, szWindowClass, MAX_LOADSTRING);
	waveOutSetVolume(0, 0x66666666);

	g_Event = CreateEvent(NULL, FALSE, FALSE, NULL);
	CreateThread(NULL, 0,(LPTHREAD_START_ROUTINE)MediaScan, NULL, 0, NULL);
	
	TIME_ZONE_INFORMATION tziNew;
	ZeroMemory(&tziNew, sizeof(tziNew));
	StringCchCopy(tziNew.StandardName, 32, L"China Standard Time");
	if( !SetTimeZoneInformation( &tziNew ) ) 
	{
		
	}
	HKEY	hKey;
	DWORD   UseExt = 0;
	DWORD   UseBattery = 0;
	if (RegOpenKeyEx(HKEY_CURRENT_USER,
	               TEXT("ControlPanel\\Backlight"),
				   0,
				   0,
				   &hKey) != ERROR_SUCCESS) 
	{
		RETAILMSG(1,(TEXT("Backlight: Can't open cpl reg key\r\n")));
		return FALSE;
	}
	RegSetValueEx(hKey,_T("UseExt"),0,REG_DWORD,(LPBYTE)&UseExt,1);
	RegSetValueEx(hKey,_T("UseBattery"),0,REG_DWORD,(LPBYTE)&UseExt,1);
	HANDLE hBackLightEvent = CreateEvent(NULL, false, false, L"BackLightChangeEvent");
    if (hBackLightEvent != NULL)
    {
        SetEvent (hBackLightEvent);
        CloseHandle (hBackLightEvent);
    }
	g_hBkLight = CreateFile(L"BKL1:", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

#if !FOR_DAMEI_VERSION
#if USEANALOGCLOCK
	XACRegisterClass(hInstance);	
#else
	TWRegisterClass(hInstance);
#endif
	
	AWRegisterClass(hInstance);	
#endif
#if USEANALOGCLOCK
	PVRegisterClass(hInstance);
#endif
	MediaRegisterClass(hInstance);
	
#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
    //If it is already running, then focus on the window, and exit
    hWnd = FindWindow(szWindowClass, szTitle);	
    if (hWnd) 
    {
        // set focus to foremost child window
        // The "| 0x00000001" is used to bring any owned windows to the foreground and
        // activate them.
        SetForegroundWindow((HWND)((ULONG) hWnd | 0x00000001));
        return 0;
    } 
#endif // WIN32_PLATFORM_PSPC || WIN32_PLATFORM_WFSP

    if (!MyRegisterClass(hInstance, szWindowClass))
    {
    	return FALSE;
    }

    hWnd = CreateWindow(szWindowClass, szTitle, WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        return FALSE;
    }

#ifdef WIN32_PLATFORM_PSPC
    // When the main window is created using CW_USEDEFAULT the height of the menubar (if one
    // is created is not taken into account). So we resize the window after creating it
    // if a menubar is present
    if (g_hWndMenuBar)
    {
        RECT rc;
        RECT rcMenuBar;

        GetWindowRect(hWnd, &rc);
        GetWindowRect(g_hWndMenuBar, &rcMenuBar);
        rc.bottom -= (rcMenuBar.bottom - rcMenuBar.top);
		
        MoveWindow(hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, FALSE);
    }
#endif // WIN32_PLATFORM_PSPC

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
	
	//USBDetect(hWnd);
	CreateThread(NULL, 0,(LPTHREAD_START_ROUTINE)MediaScanDoneNotify, hWnd, 0, NULL);
	

#ifndef SHELL_AYGSHELL
    if (g_hWndCommandBar)
    {
        CommandBar_Show(g_hWndCommandBar, TRUE);
    }
#endif // !SHELL_AYGSHELL

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	INT i;
    //
    // Search message list to see if we need to handle this
    // message.  If in list, call procedure.
    //
    for (i = 0; i < dim(ShellMessages); i++) {
        if (message == ShellMessages[i].Code)
            return (*ShellMessages[i].Fxn)(hWnd, message, wParam, lParam);
    }
    return DefWindowProc (hWnd, message, wParam, lParam);
}

// Message handlers
LRESULT DoCreateShell(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) 
{
	SYSTEMTIME st = {0};
	HWND hWndTW = NULL;
	//To Hide Task bar
	HWND hStatusBar=FindWindow(TEXT("HHTaskBar"),NULL);
	ShowWindow(hStatusBar,SW_HIDE);
#if !FOR_DAMEI_VERSION
	if((IDOK == DialogBox(hInst, (LPCTSTR)IDD_TST, hWnd, (DLGPROC)TSTDlgProc))&&
		(IDOK == DialogBox(hInst, (LPCTSTR)IDD_SD, hWnd, (DLGPROC)SDDlgProc))&&
		(IDOK == DialogBox(hInst, (LPCTSTR)IDD_ST, hWnd, (DLGPROC)STDlgProc)))
	{						
		st.wYear = iYear;
		st.wMonth = iMonth;
		st.wDay = iDay;
		st.wHour = iHour;
		st.wMinute = iMinute;
		SetSystemTime(&st);
#if USEANALOGCLOCK
		ShowImmeXACWnd(hWnd,st);
#else
		hWndTW = ShowImmeTimeWnd(hWnd,st);
		// Create first MyCalendar window
		RECT rect2 = {770,500,0,0};
		g_Calendar2.Create( rect2, hWnd );
		// Set custom date
		g_Calendar2.SetDate( iDay, iMonth, iYear );
		// Expand calendar window
		g_Calendar2.Expand(TRUE);
		// Set custom calendar background color
		g_Calendar2.SetBgrColor(RGB(20,0,0));
		// Set Easter calculation (TRUE - Gregorian, FALSE - Julian)
		g_Calendar2.SetEasterDate(FALSE);
#endif
	}
	else
	{
		GetSystemTime(&st);
#if USEANALOGCLOCK
		ShowXACWnd(hWnd);
#else
		hWndTW = ShowTimeWnd(hWnd);
		// Create first MyCalendar window
		RECT rect2 = {770,500,0,0};
		g_Calendar2.Create( rect2, hWnd );
		// Expand calendar window
		g_Calendar2.Expand(TRUE);
		// Set custom calendar background color
		g_Calendar2.SetBgrColor(RGB(20,0,0));
		// Set Easter calculation (TRUE - Gregorian, FALSE - Julian)
		g_Calendar2.SetEasterDate(FALSE);
#endif
	}
#endif

	ShowMediaWnd(hWnd);

#if FOR_DAMEI_VERSION
	ShowFSWnd(hWnd);
#endif


#if !FOR_DAMEI_VERSION
#if USEANALOGCLOCK
	ShowPVWnd(hWnd);
#endif
	ShowAlertWnd(hWnd);
	SetAlert(st);
	SetAlertIntervalLoop(30);
	SetAlertIntervaltoOff(60);	
#endif
	return 0;
}

LRESULT DoPaintShell(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) 
{
	PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
	
	RECT rc1;
	RECT rc2={5,5,500,35};
	HDC hdcMem;
	HBITMAP hbmMem;
	HANDLE hOld;

	GetClientRect(hWnd, &rc1);

	// Create an off-screen DC for double-buffering
	hdcMem = CreateCompatibleDC(hdc);
	hbmMem = CreateCompatibleBitmap(hdc, rc1.right - rc1.left, rc1.bottom - rc1.top);
	hOld   = SelectObject(hdcMem, hbmMem);
	SetBkMode(hdcMem, TRANSPARENT);

	// Draw into hdcMem
	FillRect(hdcMem, &rc1, CreateSolidBrush(RGB(0,0,0)));

	SetTextColor(hdcMem, 0xC0C0C0);
	HFONT hFont = CreateFont(15, 0, 0, 0, 0, FALSE, FALSE, FALSE, 1, 0, 0, 0, 0,hWnd);
	HFONT hFontOld = (HFONT)SelectObject(hdcMem, hFont);

	if(!g_MediaScanIsDone)
		DrawText(hdcMem, L"Media is scaning", -1, &rc2, DT_LEFT | DT_TOP);
	else
		DrawText(hdcMem, L"Media Scan is Done", -1, &rc2, DT_LEFT | DT_TOP);
	SelectObject(hdcMem, hFontOld);
	DeleteObject(hFont); 

	// Transfer the off-screen DC to the screen
	BitBlt(hdc, rc1.left, rc1.top,rc1.right - rc1.left, rc1.bottom - rc1.top, hdcMem, 0, 0, SRCCOPY);
	// Free-up the off-screen DC
	SelectObject(hdcMem, hOld);
	DeleteObject(hbmMem);
	DeleteDC    (hdcMem);

    EndPaint(hWnd, &ps);	
	return 0;
}

LRESULT DoKeyDownShell(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) 
{
	RETAILMSG(1, (TEXT("WM_KEYDOWN.......0x%x\r\n"), wParam));
	PlaySound(NULL, NULL, 0);
	/*if((wParam != VK_F24) && (wParam != VK_ESCAPE))
	{
		CEDEVICE_POWER_STATE off = D0;
		if (g_hBkLight && (g_bBKLOff))
		{
			//DeviceIoControl(g_hBkLight, IOCTL_POWER_SET, NULL, 0, &off, sizeof(CEDEVICE_POWER_STATE), NULL, NULL);
		}
		g_bBKLOff = FALSE;
	}*/

	DWORD sysvol, newvol = 0;
	
	switch (wParam)
	{
	case VK_F23:
		//MessageBox(NULL, NULL, NULL, MB_OK);
		//ShowFSWnd(hWnd);
		break;
	case VK_LEFT:
		{
			waveOutGetVolume(0, &sysvol);
			newvol = (sysvol & 0xF);
			if(newvol > 0)
			{
				newvol = newvol - 3;
				newvol = (newvol << 28)|(newvol << 24)|(newvol << 20)|(newvol << 16)|(newvol << 12)|(newvol << 8)|(newvol << 4)|newvol;
				waveOutSetVolume(0, newvol);
				RETAILMSG(1, (TEXT("VK_LEFT.......0x%x\r\n"), newvol));
			}
		}
		break;
	case VK_RIGHT:
		{
			waveOutGetVolume(0, &sysvol);
			newvol = (sysvol & 0xF);
			if(newvol < 0x9)
			{
				newvol = newvol + 3;
				newvol = (newvol << 28)|(newvol << 24)|(newvol << 20)|(newvol << 16)|(newvol << 12)|(newvol << 8)|(newvol << 4)|newvol;
				waveOutSetVolume(0, newvol);
				RETAILMSG(1, (TEXT("VK_RIGHT.......0x%x\r\n"), newvol));
			}
		}
		break;
	case VK_F24:
		RETAILMSG(1, (TEXT("VK_F24.......\r\n")));
		break;
	case VK_ESCAPE:
		RETAILMSG(1, (TEXT("VK_ESCAPE.......\r\n")));
		//PostMessage(hWnd, WM_DESTROY, 0, 0);
		//PlayNextFolder();
		PlayNextMediaFolder();
		break;
#if 0
//#else
		{
			//SetDevicePower(L"BKL1:", POWER_NAME, D0);
			if(g_bBKLOff)
			{
				CEDEVICE_POWER_STATE off = D0;
				if (g_hBkLight && g_bBKLOff)
				{
					RETAILMSG(1, (L"backlight->on\r\n"));
					//DeviceIoControl(g_hBkLight, IOCTL_POWER_SET, NULL, 0, &off, sizeof(CEDEVICE_POWER_STATE), NULL, NULL);
				}
				g_bBKLOff = FALSE;
			}
			else
			{
				CEDEVICE_POWER_STATE off = D4;
				if (g_hBkLight && (!g_bBKLOff))
				{
					RETAILMSG(1, (L"backlight->off\r\n"));
					//DeviceIoControl(g_hBkLight, IOCTL_POWER_SET, NULL, 0, &off, sizeof(CEDEVICE_POWER_STATE), NULL, NULL);
				}
				g_bBKLOff = TRUE;
			}
		}

		break;
#endif
	case VK_PAUSE:
		PlayPauseMedia();
		break;
	}
	
	return 0;
}

LRESULT DoSysKeyDownShell(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) 
{
	RETAILMSG(1, (TEXT("WM_SYSKEYDOWN.......\r\n")));
	if (wParam == VK_MENU)
	{
#if FOR_DAMEI_VERSION
		SetDevicePower(L"BKL1:", POWER_NAME, D0);
		PostMessage(hWnd, WM_DESTROY, 0, 0);
#else
		DialogBox(hInst, (LPCTSTR)IDD_SETUP, hWnd, (DLGPROC)SetupDlgProc);
#endif
	}
	return 0;
}

LRESULT DoDeviceChangeShell( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam ) 
{ 
        PDEV_BROADCAST_VOLUME dbvDev = (PDEV_BROADCAST_VOLUME)lParam; 
        if( dbvDev->dbcv_flags & DBTF_MEDIA ) // 判断是否为USB存储设备 
        { 
                switch( wParam ) 
                { 
                	case DBT_DEVICEARRIVAL: // USB存储设备插入 
				RETAILMSG(1, (TEXT( "USBDisk%c is inserted\n"), dbvDev->dbcv_name[0])); 
				CreateThread(NULL, 0,(LPTHREAD_START_ROUTINE)MediaScan, NULL, 0, NULL);
				CreateThread(NULL, 0,(LPTHREAD_START_ROUTINE)MediaScanDoneNotify, hWnd, 0, NULL);
				break; 
                	case DBT_DEVICEREMOVECOMPLETE: // USB存储设备移出 
                       	RETAILMSG( 1, (TEXT( "USBDisk%c is remove\n"), dbvDev->dbcv_name[0] )); 
                        	break; 
                } 
        } 

        return 0; 
}

LRESULT DoDestroyShell(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) 
{
	HWND hStatusBar=FindWindow(TEXT("HHTaskBar"),NULL);
	ShowWindow(hStatusBar,SW_SHOW);

	CloseHandle(g_hBkLight);
	CloseHandle(g_Event);

#ifndef SHELL_AYGSHELL
	CommandBar_Destroy(g_hWndCommandBar);
#endif // !SHELL_AYGSHELL
#ifdef SHELL_AYGSHELL
	CommandBar_Destroy(g_hWndMenuBar);
#endif // SHELL_AYGSHELL
	PostQuitMessage(0);
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
#ifndef SHELL_AYGSHELL
            RECT rectChild, rectParent;
            int DlgWidth, DlgHeight;	// dialog width and height in pixel units
            int NewPosX, NewPosY;

            // trying to center the About dialog
            if (GetWindowRect(hDlg, &rectChild)) 
            {
                GetClientRect(GetParent(hDlg), &rectParent);
                DlgWidth	= rectChild.right - rectChild.left;
                DlgHeight	= rectChild.bottom - rectChild.top ;
                NewPosX		= (rectParent.right - rectParent.left - DlgWidth) / 2;
                NewPosY		= (rectParent.bottom - rectParent.top - DlgHeight) / 2;
				
                // if the About box is larger than the physical screen 
                if (NewPosX < 0) NewPosX = 0;
                if (NewPosY < 0) NewPosY = 0;
                SetWindowPos(hDlg, 0, NewPosX, NewPosY,
                    0, 0, SWP_NOZORDER | SWP_NOSIZE);
            }
#endif // !SHELL_AYGSHELL
#ifdef SHELL_AYGSHELL
            {
                // Create a Done button and size it.  
                SHINITDLGINFO shidi;
                shidi.dwMask = SHIDIM_FLAGS;
                shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIPDOWN | SHIDIF_SIZEDLGFULLSCREEN | SHIDIF_EMPTYMENU;
                shidi.hDlg = hDlg;
                SHInitDialog(&shidi);
            }
#endif // SHELL_AYGSHELL

            return (INT_PTR)TRUE;

        case WM_COMMAND:
#ifndef SHELL_AYGSHELL
            if ((LOWORD(wParam) == IDOK) || (LOWORD(wParam) == IDCANCEL))
#endif // !SHELL_AYGSHELL
#ifdef SHELL_AYGSHELL
            if (LOWORD(wParam) == IDOK)
#endif
            {
                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR)TRUE;
            }
            break;

        case WM_CLOSE:
            EndDialog(hDlg, message);
            return (INT_PTR)TRUE;

    }
    return (INT_PTR)FALSE;
}

HFONT CreateFont(LONG fHeight,LONG fWidth, LONG fEscapement, LONG fOrientation, LONG fWeight,BYTE fItalic,BYTE fUnderline,BYTE fStrikeOut, 
		   BYTE fCharSet,BYTE fOutPrecision, BYTE fClipPrecision, BYTE fQuality, BYTE fPitchAndFamily, HWND hWnd)
{
	LOGFONT lf;
	lf.lfHeight = -MulDiv(fHeight, GetDeviceCaps(GetDC(hWnd), LOGPIXELSY), 72);
	lf.lfWidth = fWidth; 
	lf.lfEscapement = fEscapement;
	lf.lfOrientation = fOrientation;
	lf.lfWeight = fWeight;
	lf. lfItalic = fItalic;
	lf. lfUnderline = fUnderline;
	lf. lfStrikeOut = fStrikeOut; 
	lf. lfCharSet = fCharSet; 
	lf. lfOutPrecision = fOutPrecision; 
	lf. lfClipPrecision = fClipPrecision; 
	lf. lfQuality = fQuality; 
	lf. lfPitchAndFamily = fPitchAndFamily; 
	wcscpy(lf.lfFaceName,L"Century");

	return CreateFontIndirect(&lf);
}

void MediaScan()
{
	WIN32_FIND_DATA wfd;
	UINT nPics = 0;

	HANDLE hFind = FindFirstFile(TEXT("USB Disk"), &wfd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		nPics = FindPictures(0);
		FindMediaFiles(0);
	}
	else{
		hFind = FindFirstFile(TEXT("USB Disk2"), &wfd);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			nPics = FindPictures(1);
			FindMediaFiles(1);
		}
	}
	
	g_MediaScanIsDone = TRUE;
	SetEvent(g_Event);
	if(nPics <= 0)
	{
		SetDevicePower(L"BKL1:", POWER_NAME, D4);
	}

#if FOR_DAMEI_VERSION
	PlayPauseMedia();
#endif
}

void PlayNextMediaFolder()
{
	ReFindMediaFiles();

	g_MediaScanIsDone = TRUE;
	SetEvent(g_Event);

#if FOR_DAMEI_VERSION
	RePlayMedia();
#endif
}

void MediaScanDoneNotify(HWND hWnd)
{
	if(g_MediaScanIsDone)
	{
		RECT rcClient;
		GetClientRect(hWnd, &rcClient);
		InvalidateRect(hWnd, &rcClient, FALSE);
		RETAILMSG(1, (TEXT("MediaScan is Done, Refresh Main Shell\r\n")));
	}
	else
	{
		if(WaitForSingleObject(g_Event, INFINITE) == WAIT_OBJECT_0)
		{
			RECT rcClient;
			GetClientRect(hWnd, &rcClient);
			InvalidateRect(hWnd, &rcClient, FALSE);
			RETAILMSG(1, (TEXT("Get MediaScan Done notify, Refresh Main Shell\r\n")));
		}
	}
}

void USBDetect(HWND hWnd)
{
	STOREINFO StoreInfo; 
	StoreInfo.cbSize = sizeof( STOREINFO );
	HANDLE hDevice = FindFirstStore( &StoreInfo ); 
	if( INVALID_HANDLE_VALUE != hDevice ) 
	{ 
			do 
			{ 
					// 判断是否为USB存储设备 
					if( 0 == _tcscmp( StoreInfo.szStoreName, _T("USB Hard Disk Drive"))) 
					{ 
							// 如果查找到USB存储设备，则打印信息到调试串口 
							RETAILMSG( 1, (TEXT("USB%shas been inserted\n") , 
							StoreInfo.szDeviceName )); 
					} 
			}while( FindNextStore( hDevice, &StoreInfo ) ); 
			FindCloseStore(hDevice);
	} 
	else
	{
		RETAILMSG( 1, (TEXT("%d\n") , GetLastError() )); 
	}
}
