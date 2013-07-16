/*
Copyright:
    Copyright (c) 2010 Netlogic Microsystems.  All rights reserved.

Abstract:

Author:
    Franklin January 2010
    
Revision History:
*/
#include "stdafx.h"
#include <windows.h>
#include "resource.h"
#include "APictureViewer.h"
#include "FullScreen.h"
#include "UserDef.h"

#ifdef DEBUG
#pragma comment(lib, "..\\Common\\AImage\\Debug\\AImage.lib")
#else
#pragma comment(lib, "..\\Common\\AImage\\Release\\AImage.lib")
#endif // DEBUG

#define IDT_TIMER1 9001

//
// macro definitions
//
#ifdef DEBUG
#define OutputMsg 1
#else
#define OutputMsg 0
#endif // DEBUG

#define IDS_WND_CLASS TEXT("PICTUREVIEWER")
#define IDS_WND_TITLE TEXT("Picture Viewer")

#define TITLE_BAR_HEIGHT 0//32 // title bar height
#define MENU_BAR_HEIGHT 56 // bottom menu bar height
#define BTN_WIDTH 80 // button image width
#define CLOSE_BTN_WIDTH 56 // close button image width
#define THB_HSPACE 8 // horizon space between each thumbnail
#define THB_VSPACE 8 // virtical space between each thumbnail
#define THB_WIDTH 192 // thumbnail width
#define THB_HEIGHT 120 // thumbnail height
// horizontal thumbnail numbers
#define THB_HITEMS ((PV_WIN_WIDTH-THB_HSPACE)/(THB_WIDTH+THB_HSPACE))
// vertical thumbnail numbers
#define THB_VITEMS ((PV_WIN_HEIGHT-TITLE_BAR_HEIGHT-MENU_BAR_HEIGHT-THB_VSPACE)/(THB_HEIGHT+THB_VSPACE))
// top y coordinate of the thumbnails display area
#define THB_LEFT ((PV_WIN_WIDTH-(THB_HITEMS*(THB_WIDTH+THB_HSPACE)-THB_HSPACE))/2)
// top y coordinate of the thumbnails display area
#define THB_TOP (TITLE_BAR_HEIGHT+(PV_WIN_HEIGHT-TITLE_BAR_HEIGHT-MENU_BAR_HEIGHT-(THB_VITEMS*(THB_HEIGHT+THB_VSPACE)-THB_VSPACE))/2)

#define DEAFULT_PICTURE_LOCATION TEXT("USB Disk\\IMG\\")
#define DEAFULT_PICTURE_LOCATION2 TEXT("USB Disk2\\IMG\\")

#define PNG_IMAGE_UNDERLINE TEXT("USB Disk\\Resource\\APictureViewer\\underline.png") 
#define PNG_IMAGE_PREVIOUS TEXT("USB Disk\\Resource\\APictureViewer\\previous.png") 
#define PNG_IMAGE_NEXT TEXT("USB Disk\\Resource\\APictureViewer\\next.png") 
#define PNG_IMAGE_CLOSE TEXT("USB Disk\\Resource\\APictureViewer\\close.png") 

//
// global variables
//
extern HINSTANCE hInst;
#if FOR_DAMEI_VERSION
extern HANDLE g_Event;
#endif
HWND g_hWnd = NULL;

BOOL bPictureScanIsDone = FALSE;

UINT g_nTotalItems = 0; // number of picture files
UINT g_nCurItem = 0; // current item
UINT g_nTotalPages = 0; // number of pages
UINT g_nCurPage = 0; // current page
PICTURE_LIST_ITEM *g_lpItem = NULL; // picture list

CCoImage g_uiImage;

// playing control item identifier
enum enumControlItemID {
//	CID_EXIT,
	CID_PREVIOUS,
	CID_NEXT,
};

// playing control button structure
struct PICTURE_CTRL_BUTTON g_playCtrl[] = {
//	CID_EXIT, {WINDOW_WIDTH-CLOSE_BTN_WIDTH-4, 2, WINDOW_WIDTH-2, TITLE_BAR_HEIGHT-2}, PNG_IMAGE_CLOSE,
	CID_PREVIOUS, {THB_LEFT, PV_WIN_HEIGHT-MENU_BAR_HEIGHT+4, THB_LEFT+BTN_WIDTH, PV_WIN_HEIGHT-4},PNG_IMAGE_PREVIOUS,
	CID_NEXT, {PV_WIN_WIDTH-THB_LEFT-BTN_WIDTH, PV_WIN_HEIGHT-MENU_BAR_HEIGHT+4, PV_WIN_WIDTH-THB_LEFT, PV_WIN_HEIGHT-4},PNG_IMAGE_NEXT,
};

//
// forward declarations of functions included in this code module
//
ATOM MyRegisterClass(HINSTANCE, LPTSTR);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK PVWndProc(HWND, UINT, WPARAM, LPARAM);
void GetThumbnailRect(UINT nItem, RECT *lprc);
UINT SearchPictureFiles(wchar_t *lpwszPath, PICTURE_LIST_ITEM **lppItem);
bool PictureTypeFilter(wchar_t *lpwszFile);

#if FOR_DAMEI_VERSION
void MediaScanDoneNotifyFS(HWND);
#endif

//
// message handler
//
LRESULT OnWndCreate(HWND, UINT, WPARAM, LPARAM);
LRESULT OnWndDestroy(HWND, UINT, WPARAM, LPARAM);
LRESULT OnWndPaint(HWND, UINT, WPARAM, LPARAM);
LRESULT OnWndTimer(HWND, UINT, WPARAM, LPARAM);
LRESULT OnWndLBtnDown(HWND, UINT, WPARAM, LPARAM);
LRESULT OnWndLBtnUp(HWND, UINT, WPARAM, LPARAM);

//
// message map
//
const struct decodeUINT g_stMsg[] = {
	WM_CREATE, OnWndCreate,
	WM_DESTROY, OnWndDestroy,
	WM_PAINT, OnWndPaint,
	WM_TIMER, OnWndTimer,
	WM_LBUTTONDOWN, OnWndLBtnDown,
	WM_LBUTTONUP, OnWndLBtnUp,
};

//
// WinMain
//
//int APIENTRY _tWinMain (HINSTANCE hInstance,
//	HINSTANCE hPrevInstance,
//	LPTSTR lpCmdLine,
//	int nCmdShow)
//{
//	MSG msg;
//
//	// Perform application initialization:
//	if (!InitInstance(hInstance, nCmdShow)) 
//	{
//		return 0;
//	}
//
//	// Main message loop:
//	while (GetMessage(&msg, NULL, 0, 0)) 
//	{
//		TranslateMessage(&msg);
//		DispatchMessage(&msg);
//	}
//
//	return (int) msg.wParam;
//}

BOOL PVRegisterClass(HINSTANCE hInstance)
{
	WNDCLASS wndclass = {0}; 
	DWORD    wStyle   = 0;

	wndclass.style         = 0;//CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc   = (WNDPROC) PVWndProc; 
	wndclass.hInstance     = hInstance; 
	wndclass.hbrBackground = NULL;//CreateSolidBrush(RGB(0,0,20));;//(HBRUSH) GetStockObject (DKGRAY_BRUSH);
	wndclass.lpszClassName = TEXT ("PictureViewer"); 

	if (!RegisterClass (&wndclass) ) 
	{
		return FALSE; 
	}
	return TRUE;
}

BOOL ShowPVWnd(HWND hWnd)
{
	// Create main window.
    g_hWnd = CreateWindow (TEXT ("PictureViewer"),            // Window class
							TEXT ("Picture Viewer"), // Window title
							WS_VISIBLE|WS_CHILD,           // Style flags
							PV_WIN_LEFT,        // x position
							PV_WIN_TOP,        // y position
							PV_WIN_WIDTH,        // Initial width
							PV_WIN_HEIGHT,        // Initial height
							hWnd,                 // Parent
							NULL,                 // Menu, must be null
							hInst,            // Application instance
							NULL);                // Pointer to create
                                               // parameters
    // Return fail code if window not created.
    if (!IsWindow (g_hWnd)) return FALSE;

    // Standard show and update calls
    ShowWindow (g_hWnd, TRUE);
    UpdateWindow (g_hWnd);	
	
    return TRUE;
}

BOOL ShowFSWnd(HWND hWnd)
{
#if !FOR_DAMEI_VERSION
	SetFilePath(g_lpItem[g_nCurItem].wszPath);
#endif

	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);

	POINT pt = {0, 0};
	ScreenToClient(hWnd, &pt);
	ClientToScreen(hWnd, &pt);

	HWND hWndView = CreateFullScreenViewWindow(pt.x, pt.y, cx, cy, hWnd, hInst);
	ShowWindow(hWndView, SW_SHOW);

#if FOR_DAMEI_VERSION
	CreateThread(NULL, 0,(LPTHREAD_START_ROUTINE)MediaScanDoneNotifyFS, hWndView, 0, NULL);
#endif
	return 1;
}
//
//  registers the window class
//
//ATOM MyRegisterClass(HINSTANCE hInstance, LPTSTR szWindowClass)
//{
//	WNDCLASS wc;
//
//	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
//	wc.lpfnWndProc = WndProc;
//	wc.cbClsExtra = 0;
//	wc.cbWndExtra = 0;
//	wc.hInstance = hInstance;
//	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
//	wc.hCursor = NULL;
//	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
//	wc.lpszMenuName  = NULL;
//	wc.lpszClassName = szWindowClass;
//
//	return RegisterClass(&wc);
//}

//
// Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
//BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
//{
//	HWND hWnd = FindWindow(IDS_WND_CLASS, IDS_WND_TITLE);
//	if (hWnd)
//	{
//		ShowWindow(hWnd, SW_SHOW);
//		g_hWnd = hWnd;
//		return FALSE;
//	}
//
//	g_hInstance = hInstance; // Store instance handle in our global variable
//	if (!MyRegisterClass(hInstance, IDS_WND_CLASS))
//	{
//		return FALSE;
//	}
//
//	int  x = (GetSystemMetrics(SM_CXSCREEN) - WINDOW_WIDTH) / 2;
//	int  y = (GetSystemMetrics(SM_CYSCREEN) - WINDOW_HEIGHT) / 2;
//
//	hWnd = CreateWindow(IDS_WND_CLASS, IDS_WND_TITLE, WS_VISIBLE, x, y, 
//		WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);
//
//	if (!hWnd)
//	{
//		return FALSE;
//	}
//
//	g_hWnd = hWnd;
//	ShowWindow(hWnd, nCmdShow);
//	UpdateWindow(hWnd);
//
//	return TRUE;
//}

//
// process message for the main window.
//
LRESULT CALLBACK PVWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	for (int i = 0; i < dim(g_stMsg); i++)
	{
		if (message == g_stMsg[i].nID)
		{
			return (*g_stMsg[i].Fxn)(hWnd, message, wParam, lParam);
		}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

//
// WM_CREATE
//
LRESULT OnWndCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	SetTimer(hWnd, IDT_TIMER1, 5000, (TIMERPROC) NULL);
	return 0;
}

//
// WM_DESTROY
//
LRESULT OnWndDestroy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PostQuitMessage(0);
	return 0;
}

//
// WM_PAINT
//
LRESULT OnWndPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);

	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rcClient.right-rcClient.left, rcClient.bottom-rcClient.top);
	HBITMAP hBmpOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

	// fill the back ground
	HBRUSH hBrush = CreateSolidBrush(RGB(0,0,0));
	HBRUSH hBrushOld = (HBRUSH)SelectObject(hdcMem, hBrush);
	FillRect(hdcMem, &rcClient, hBrush);
	SelectObject(hdcMem, hBrushOld);
	DeleteObject(hBrush);

	SetBkMode(hdcMem, TRANSPARENT);
	SetTextColor(hdcMem, TITLE_COLOR);

	//// title
	//RECT rcTitle = {10, 0, WINDOW_WIDTH, TITLE_BAR_HEIGHT};
	//DrawText(hdcMem, L"Picture Viewer", -1, &rcTitle, DT_LEFT | DT_VCENTER);

	//// title bar underline
	//RECT rcTitleLine = {0, TITLE_BAR_HEIGHT-1, WINDOW_WIDTH, TITLE_BAR_HEIGHT};
	//g_uiImage.CoImgDisplayIcon(PNG_IMAGE_UNDERLINE, hdcMem, &rcTitleLine);
	//
	//// bottom menu top underline
	//RECT rcMenu = {0, WINDOW_HEIGHT-MENU_BAR_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT-MENU_BAR_HEIGHT+1};
	//g_uiImage.CoImgDisplayIcon(PNG_IMAGE_UNDERLINE, hdcMem, &rcMenu);

	// draw control buttons
	for (int i = 0; i < dim(g_playCtrl); i++)
	{
		g_uiImage.CoImgDisplayIcon(g_playCtrl[i].lpwszImage, hdcMem, &g_playCtrl[i].rcCtrl);
	}
	
	
	int nItermsPerPage = THB_HITEMS*THB_VITEMS;
	g_nTotalPages = (g_nTotalItems*1000/nItermsPerPage+999)/1000;

	// page number rectangle
	RECT rcPage = {0, PV_WIN_HEIGHT-MENU_BAR_HEIGHT, PV_WIN_WIDTH, PV_WIN_HEIGHT}; 
	wchar_t wszItem[MAX_PATH] = {0};
	wsprintf(wszItem, L"%d / %d", g_nCurPage+1, g_nTotalPages);
	DrawText(hdcMem, wszItem, -1, &rcPage, DT_CENTER|DT_VCENTER);

	BitBlt(hdc, rcClient.left, rcClient.top, rcClient.right-rcClient.left, rcClient.bottom-rcClient.top, 
		hdcMem, rcClient.left, rcClient.top, SRCCOPY);

	SelectObject(hdcMem, hBmpOld);
	DeleteObject(hBitmap);
	DeleteDC(hdcMem);

	if(bPictureScanIsDone)
	{
		for (int i = 0; i < nItermsPerPage; i++)
		{
			int nItem = g_nCurPage*nItermsPerPage+i;
			if (nItem >= g_nTotalItems)
			{
				break;
			}

			RECT rc;
			GetThumbnailRect(nItem, &rc);
			g_uiImage.CoImgDisplayThumbnail(g_lpItem[nItem].wszPath, hdc, &rc);
		}
	}

	EndPaint(hWnd, &ps);
	return 0;
}

//
// WM_TIMER
//
LRESULT OnWndTimer(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (wParam) 
	{ 
		case IDT_TIMER1: 
			g_nCurPage += 1;
			if(g_nCurPage >= g_nTotalItems)
			{
				g_nCurPage = 0;
			}
			RECT rcClient;
			GetClientRect(hWnd, &rcClient);
			InvalidateRect(hWnd, &rcClient, FALSE);
			break;
	}
	return 0;//DefWindowProc(hWnd, message, wParam, lParam);
}

//
// WM_LBUTTONDOWN
//
LRESULT OnWndLBtnDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc = GetDC(hWnd);
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);

	if (x > THB_LEFT && x < PV_WIN_WIDTH-THB_LEFT && y > THB_TOP && y < PV_WIN_HEIGHT-MENU_BAR_HEIGHT)
	{
		for (int nColum = 0; nColum < THB_HITEMS; nColum++) // colum
		{
			if (x > THB_LEFT+nColum*(THB_WIDTH+THB_HSPACE) && x < THB_LEFT+nColum*(THB_WIDTH+THB_HSPACE)+THB_WIDTH)
			{
				for (int nRow = 0; nRow < THB_VITEMS; nRow++) // row
				{
					if (y > THB_TOP+nRow*(THB_HEIGHT+THB_VSPACE) && y < THB_TOP+nRow*(THB_HEIGHT+THB_VSPACE)+THB_HEIGHT)
					{
						g_nCurItem = g_nCurPage*THB_HITEMS*THB_VITEMS+THB_HITEMS*nRow+nColum;
						if (g_nCurItem < g_nTotalItems)
						{
							RETAILMSG(OutputMsg, (L"LBUTTONDOWN  XY: %d %d - CR: %d %d - NO. %d\r\n", x, y, nColum, nRow, g_nCurItem));
							RECT rcBtn = {THB_LEFT+nColum*(THB_WIDTH+THB_HSPACE), 
								THB_TOP+nRow*(THB_HEIGHT+THB_VSPACE),
								THB_LEFT+nColum*(THB_WIDTH+THB_HSPACE)+THB_WIDTH,
								THB_TOP+nRow*(THB_HEIGHT+THB_VSPACE)+THB_HEIGHT};

							g_uiImage.CoImgDisplayIcon(PNG_IMAGE_FOCUS, hdc, &rcBtn);
							SetFilePath(g_lpItem[g_nCurItem].wszPath);

							int cx = GetSystemMetrics(SM_CXSCREEN);
							int cy = GetSystemMetrics(SM_CYSCREEN);

							POINT pt = {0, 0};
							ScreenToClient(hWnd, &pt);
							ClientToScreen(hWnd, &pt);

							HWND hWndView = CreateFullScreenViewWindow(pt.x, pt.y, cx, cy, hWnd, hInst);
							ShowWindow(hWndView, SW_SHOW);
						}
						break;
					}
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < dim(g_playCtrl); i++)
		{
			if (x > g_playCtrl[i].rcCtrl.left  && x < g_playCtrl[i].rcCtrl.right
				&& y > g_playCtrl[i].rcCtrl.top  && y < g_playCtrl[i].rcCtrl.bottom)
			{
				RECT rcBtn = {g_playCtrl[i].rcCtrl.left, g_playCtrl[i].rcCtrl.top,  g_playCtrl[i].rcCtrl.right, g_playCtrl[i].rcCtrl.bottom};
	
				/*if (i == CID_EXIT)
				{
					SendMessage(hWnd, WM_CLOSE, 0, 0);
				}*/
				if (i == CID_PREVIOUS)
				{
					if (g_nCurPage > 0)
					{
						g_uiImage.CoImgDisplayIcon(PNG_IMAGE_FOCUS, hdc, &rcBtn);
						g_nCurPage -= 1;

						RECT rcClient;
						GetClientRect(hWnd, &rcClient);
						InvalidateRect(hWnd, &rcClient, FALSE);
					}
				}
				else if (i == CID_NEXT)
				{
					if (g_nCurPage < g_nTotalPages-1)
					{
						g_uiImage.CoImgDisplayIcon(PNG_IMAGE_FOCUS, hdc, &rcBtn);
						g_nCurPage += 1;

						RECT rcClient;
						GetClientRect(hWnd, &rcClient);
						InvalidateRect(hWnd, &rcClient, FALSE);
					}
				}
				break;
			}
		}
	}

	ReleaseDC(hWnd, hdc);
	return 0;
}

//
// WM_LBUTTONUP
//
LRESULT OnWndLBtnUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, message, wParam, lParam);
}

//
// get the thumbnail image rectangle
//
void GetThumbnailRect(UINT nItem, RECT *lprc)
{
	int nItermsPerPage = THB_HITEMS*THB_VITEMS;
	int nIndex = nItem-g_nCurPage*nItermsPerPage;

	UINT nIDY = nIndex/THB_HITEMS;
	UINT nIDX = nIndex-nIDY*THB_HITEMS;
	
	lprc->left = THB_LEFT+nIDX*(THB_WIDTH+THB_HSPACE);
	lprc->top = THB_TOP+nIDY*(THB_HEIGHT+THB_VSPACE);
	lprc->right = lprc->left +THB_WIDTH;
	lprc->bottom = lprc->top+THB_HEIGHT;
}

//
// search media files on certain location
//
UINT SearchPictureFiles(wchar_t *lpwszPath, PICTURE_LIST_ITEM **lppItem)
{
	WIN32_FIND_DATA wfd;
	wchar_t wszPath[MAX_PATH] = {0};

	wcscpy(wszPath, lpwszPath);
	wcscat(wszPath, L"*.*");

	HANDLE hFind = FindFirstFile(wszPath, &wfd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	int nCount = -1;
	PICTURE_LIST_ITEM *lpItem = NULL;

	if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY && PictureTypeFilter(wfd.cFileName))
	{
		nCount = 0;
		lpItem = new PICTURE_LIST_ITEM[1];
		lpItem->nIndex = nCount;
		wcscpy(lpItem[lpItem->nIndex ].wszName, wfd.cFileName);
		wcscpy(lpItem[lpItem->nIndex ].wszPath, lpwszPath);
		wcscat(lpItem[lpItem->nIndex ].wszPath, wfd.cFileName);
		RETAILMSG(OutputMsg, (L"Media file [%d] %s\r\n", nCount, lpItem[nCount].wszName));
	}

	while (FindNextFile(hFind, &wfd))
	{
		if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY && PictureTypeFilter(wfd.cFileName))
		{
			PICTURE_LIST_ITEM *lpTemp = new PICTURE_LIST_ITEM[nCount+1];
			memcpy(lpTemp, lpItem, (nCount+1)*sizeof(PICTURE_LIST_ITEM));

			nCount += 1;
			delete []lpItem;

			lpItem = new PICTURE_LIST_ITEM[nCount+1];
			memcpy(lpItem, lpTemp, nCount*sizeof(PICTURE_LIST_ITEM));
			delete []lpTemp;

			lpItem->nIndex = nCount;
			wcscpy(lpItem[lpItem->nIndex].wszName, wfd.cFileName);
			wcscpy(lpItem[lpItem->nIndex].wszPath, lpwszPath);
			wcscat(lpItem[lpItem->nIndex].wszPath, wfd.cFileName);
			RETAILMSG(OutputMsg, (L"Media file [%d] %s\r\n", nCount, lpItem[lpItem->nIndex].wszName));
		}
	}

	*lppItem = lpItem;
	FindClose(hFind);
	return (nCount+1);
}

UINT FindPictures(int index)
{	
	if(index == 0)
	{
		g_nTotalItems = SearchPictureFiles(DEAFULT_PICTURE_LOCATION, &g_lpItem);
	}
	else if(index == 1)//USB DISK2
	{
		g_nTotalItems = SearchPictureFiles(DEAFULT_PICTURE_LOCATION2, &g_lpItem);
	}
	bPictureScanIsDone = TRUE;
	RECT rcClient;
	GetClientRect(g_hWnd, &rcClient);
	InvalidateRect(g_hWnd, &rcClient, FALSE);
	return g_nTotalItems;
}

//
// media file type filter
//
bool PictureTypeFilter(wchar_t *lpwszFile)
{
	int nLen = wcslen(lpwszFile);
	if (!wcscmp(lpwszFile + nLen - 4, L".bmp") ||
		!wcscmp(lpwszFile + nLen - 4, L".BMP") ||
		!wcscmp(lpwszFile + nLen - 4, L".jpg") ||
		!wcscmp(lpwszFile + nLen - 4, L".JPG") ||
		!wcscmp(lpwszFile + nLen - 5, L".jpeg") ||
		!wcscmp(lpwszFile + nLen - 5, L".JPEG") ||
		!wcscmp(lpwszFile + nLen - 4, L".png") ||
		!wcscmp(lpwszFile + nLen - 4, L".PNG") ||
		!wcscmp(lpwszFile + nLen - 4, L".gif") ||
		!wcscmp(lpwszFile + nLen - 4, L".GIF"))
	{
		return true;
	}

	return false;
}

bool RandomImage()
{
	if (g_nCurItem < g_nTotalItems)
	{
		g_nCurItem = rand()%g_nTotalItems;		
	}
	else //impossible
	{
		g_nCurItem = 0;
	}
	SetFilePath(g_lpItem[g_nCurItem].wszPath);
	return true;
}

bool NextImage()
{
	if (g_nCurItem < g_nTotalItems-1)
	{
		g_nCurItem += 1;
		SetFilePath(g_lpItem[g_nCurItem].wszPath);
		return true;
	}

	return false;
}

bool PreviousImage()
{
	if (g_nCurItem > 0)
	{
		g_nCurItem -= 1;
		SetFilePath(g_lpItem[g_nCurItem].wszPath);
		return true;
	}

	return false;
}

#if FOR_DAMEI_VERSION
void MediaScanDoneNotifyFS(HWND hWnd)
{
	if(WaitForSingleObject(g_Event, INFINITE) == WAIT_OBJECT_0)
	{
		RandomImage();
		RECT rcClient;
		GetClientRect(hWnd, &rcClient);
		InvalidateRect(hWnd, &rcClient, TRUE);
		RETAILMSG(1,(TEXT("MediaScan is Done, Refresh Full Screen\r\n")));
		SetEvent(g_Event);//To notify main shell window to refresh screen
	}
}
#endif